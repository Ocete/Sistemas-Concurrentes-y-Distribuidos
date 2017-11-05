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

static const int num_clientes = 5;


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

void EsperarFueraBarberia(int i_cliente) {
  EsperaAleatoria();
}

//**********************************************************************
class MonitorBarbero : public HoareMonitor {
private:
  CondVar cond_clientes, cond_barbero, cond_silla;
  bool silla_vacia;

 public:
   MonitorBarbero ();
   void CortarPelo(int i_cliente);
   void SiguienteCliente();
   void FinCliente();
} ;
// -----------------------------------------------------------------------------

MonitorBarbero::MonitorBarbero ()
{
  silla_vacia = true;
  cond_clientes = newCondVar();
  cond_barbero = newCondVar();
  cond_silla = newCondVar();
}
// -----------------------------------------------------------------------------

void MonitorBarbero::CortarPelo(int i_cliente) {
  cout << "Cliente " << i_cliente << " entra a la barberia" << endl;
  if (!cond_clientes.empty() || !silla_vacia) { // DUDA: si un cliente llega cuando justo la silla se vacia pero hay  gente esperando, tiene que revisar la cola, no?
    cond_clientes.wait();
  }
  silla_vacia = false;
  cond_barbero.signal();
  cout << "Cliente " << i_cliente << " se sienta" << endl;
  cond_silla.wait();
  cout << "Cliente " << i_cliente << " se levanta" << endl;
}
// -----------------------------------------------------------------------------

void MonitorBarbero::SiguienteCliente() {
  if (cond_clientes.empty()) {
    cout << "\t\t\tEl barbero duerme" << endl;
    cond_barbero.wait();
  } else {
    cond_clientes.signal();
  }
  cout << "\t\t\tEl barbero sienta un nuevo cliente" << endl;
}
// -----------------------------------------------------------------------------

void MonitorBarbero::FinCliente() {
  cout << "\t\t\tEl barbero despide a un cliente" << endl;
  silla_vacia = true;
  cond_silla.signal();
}
// -----------------------------------------------------------------------------

void funcion_hebra_barbero(MRef<MonitorBarbero> barberia)
{
  int i = 0;
  while( i < 100000 )
  {
    barberia->SiguienteCliente();
    CortarPeloACliente();
    barberia->FinCliente();
    i++;
  }
  cout << "FFFFFFFFFFFFFFFFFFFFFIIIIIIIIIIIIIIIIIIIIINNNNNNNNNNNNNNNNN" << endl;
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_cliente( MRef<MonitorBarbero> barberia, int i_cliente )
{
   while( true )
   {
     barberia->CortarPelo(i_cliente);
     EsperarFueraBarberia(i_cliente);
   }
}
//----------------------------------------------------------------------

int main()
{
   auto monitor = Create<MonitorBarbero>();

   thread CThreads[num_clientes], barbero (funcion_hebra_barbero, monitor);

   for (int i=0; i<num_clientes; i++) {
     CThreads[i] = thread ( funcion_hebra_cliente, monitor, i);
   }
   for (int i=0; i<num_clientes; i++) {
     CThreads[i].join();
   }
}
