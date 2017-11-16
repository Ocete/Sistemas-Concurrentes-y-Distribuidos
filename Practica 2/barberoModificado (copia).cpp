// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 2. Introducción a los monitores en C++11.
//
// archivo: prodcons_1.cpp
// Ejemplo de un monitor en C++11 con semántica SC, para el problema
// del productor/consumidor, con un único productor y un único consumidor.
// Opcion FIFO (queue)
//
// Historial:
// Creado en Julio de 2017
// -----------------------------------------------------------------------------


#include <iostream>
#include <iomanip>
#include <cassert>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include "HoareMonitor.hpp"
using namespace HM;
using namespace std ;

static const int num_clientes = 10;
static const int num_barberos = 3;
int clientes_esperando;
static const int MAX_ESPERA = 3;

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//-------------------------------------------------------------------------
void EsperaAleatoria()
{
   // calcular milisegundos aleatorios de duración de la espera
   chrono::milliseconds espera( aleatorio<20,200>() );

   // espera bloqueada un tiempo igual a ''espera' milisegundos
   this_thread::sleep_for( espera );
}

void CortarPeloACliente() {
  EsperaAleatoria();
}

void EsperarFueraBarberia() {
  chrono::milliseconds espera( aleatorio<40,400>() );
  this_thread::sleep_for( espera );
}

// ****************************************************************************
class MonitorBarbero : public HoareMonitor {
private:
  CondVar cond_clientes, cond_barberos[num_barberos], cond_silla[num_barberos];
  bool silla_vacia[num_barberos];
  int BuscarSitio();
 public:
   MonitorBarbero ();
   void CortarPelo(int i_cliente);
   void SiguienteCliente(int i_barbero);
   void FinCliente(int i_barbero);
} ;
// -----------------------------------------------------------------------------

int MonitorBarbero::BuscarSitio ()
{
  int hueco = -1, i = 0;
  while (hueco == -1 && i < num_barberos) {
    if (silla_vacia[i]) {
      hueco = i;
    }
    i++;
  }

  return hueco;
}
// -----------------------------------------------------------------------------

MonitorBarbero::MonitorBarbero ()
{
  cond_clientes = newCondVar();
  for (int i=0; i<num_barberos; i++) {
    cond_barberos[i] = newCondVar();
    cond_silla[i] = newCondVar();
    silla_vacia[i] = true;
  }
  clientes_esperando = 0;
}
// -----------------------------------------------------------------------------

void MonitorBarbero::CortarPelo(int i_cliente) {
  cout << "Cliente " << i_cliente << " entra a la barberia" << endl;
  clientes_esperando++;
  int silla = BuscarSitio();
  if (!cond_clientes.empty() || silla == -1) {
    cond_clientes.wait();
  }
  clientes_esperando--;
  silla = BuscarSitio();
  silla_vacia[silla] = false;
  cond_barberos[silla].signal();
  cout << "Cliente " << i_cliente << " se sienta" << endl;
  cond_silla[silla].wait();
  cout << "Cliente " << i_cliente << " se levanta" << endl;
}
// -----------------------------------------------------------------------------

void MonitorBarbero::SiguienteCliente(int i_barbero) {
  if (cond_clientes.empty()) {
    cout << "\t\t\tEl barbero " << i_barbero << " duerme" << endl;
    cond_barberos[i_barbero].wait();
    cout << "\t\t\tEl barbero " << i_barbero << " se despierta" << endl;
  } else {
    cond_clientes.signal();
  }
  cout << "\t\t\tEl barbero " << i_barbero << " sienta un nuevo cliente" << endl;
}
// -----------------------------------------------------------------------------

void MonitorBarbero::FinCliente(int i_barbero) {
  cout << "\t\t\tEl barbero " << i_barbero << " despide a un cliente" << endl;
  silla_vacia[i_barbero] = true;
  cond_silla[i_barbero].signal();
}
// -----------------------------------------------------------------------------

void funcion_hebra_barbero(MRef<MonitorBarbero> barberia, int i_barbero)
{
  while ( true ) {
    barberia->SiguienteCliente(i_barbero);
    CortarPeloACliente();
    barberia->FinCliente(i_barbero);
  }
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_cliente( MRef<MonitorBarbero> barberia, int i_cliente )
{
   while( true )
   {
     while (clientes_esperando >= MAX_ESPERA) {
       cout << "Cliente " << i_cliente << " va a darse una vuelta" << endl;
       //this_thread::sleep_for(chrono::seconds(1));
       EsperarFueraBarberia();
     }
     barberia->CortarPelo(i_cliente);
     EsperarFueraBarberia();
   }
}
//----------------------------------------------------------------------

int main()
{
   auto monitor = Create<MonitorBarbero>();

   thread clienteThreads[num_clientes], barberoThreads[num_barberos];

   for (int i=0; i<num_barberos; i++) {
     barberoThreads[i] = thread ( funcion_hebra_barbero, monitor, i);
   }
   EsperaAleatoria();
   for (int i=0; i<num_clientes; i++) {
     clienteThreads[i] = thread ( funcion_hebra_cliente, monitor, i);
   }
   for (int i=0; i<num_clientes; i++) {
     clienteThreads[i].join();
   }
   for (int i=0; i<num_barberos; i++) {
     barberoThreads[i].join();
   }
}
