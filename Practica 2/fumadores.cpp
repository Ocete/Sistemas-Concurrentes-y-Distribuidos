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

static const int num_fumadores = 3;


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
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

//**********************************************************************
class MonitorFumador : public HoareMonitor {
private:
  int ingrediente_en_mostrador;
  CondVar cond_fumadores[num_fumadores], cond_estanquero;
  bool vacio, lleno;

 public:
   MonitorFumador();
   void PonerIngrediente(int ingrediente);
   void ObtenerIngrediente(int ingrediente);
   void EsperarRecogida();
} ;
// -----------------------------------------------------------------------------

MonitorFumador::MonitorFumador ()
{
  ingrediente_en_mostrador = -1;
  for (int i=0; i<num_fumadores; i++) {
    cond_fumadores[i] = newCondVar();
  }
   cond_estanquero = newCondVar();
}
// -----------------------------------------------------------------------------

void MonitorFumador::PonerIngrediente(int ingrediente) {
  ingrediente_en_mostrador = ingrediente;
  cond_fumadores[ingrediente].signal();
}
// -----------------------------------------------------------------------------

void MonitorFumador::ObtenerIngrediente(int ingrediente) {
  if (ingrediente_en_mostrador != ingrediente) {
    cond_fumadores[ingrediente].wait();
  }
  ingrediente_en_mostrador = -1;
  cond_estanquero.signal();
}
// -----------------------------------------------------------------------------

void MonitorFumador::EsperarRecogida() {
  if (ingrediente_en_mostrador != -1) {
      cond_estanquero.wait();
  }
}
// -----------------------------------------------------------------------------

void funcion_hebra_estanquero(MRef<MonitorFumador> monitor)
{
  while( true )
  {
    int ingrediente = aleatorio<0, num_fumadores-1>();
    cout << "\tEstanquero: " << ingrediente << endl;
    monitor->PonerIngrediente(ingrediente);
    monitor->EsperarRecogida();
  }
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( MRef<MonitorFumador> monitor, int num_fumador )
{
   while( true )
   {
     monitor->ObtenerIngrediente(num_fumador);
     fumar(num_fumador);
   }
}
//----------------------------------------------------------------------

int main()
{
   auto monitor = Create<MonitorFumador>();

   thread FThreads[num_fumadores], estanquero (funcion_hebra_estanquero, monitor);

   for (int i=0; i<num_fumadores; i++) {
     FThreads[i] = thread ( funcion_hebra_fumador, monitor, i);
   }
   for (int i=0; i<num_fumadores; i++) {
     FThreads[i].join();
   }
}
