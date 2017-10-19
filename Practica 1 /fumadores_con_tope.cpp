#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

const int numero_fumadores = 3;
const int ingredientes_totales = 100;
Semaphore *semFumadores[numero_fumadores], semEstanquero(0);
bool keep_going;

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

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(  )
{
  for (int i=0; i<ingredientes_totales; i++)
  {
    int ingrediente = aleatorio<0, numero_fumadores-1>();
    cout << "\tEstanquero: " << ingrediente << " (en total " << i << ")" << endl;
    semFumadores[ingrediente]->sem_signal();
    semEstanquero.sem_wait();
  }
  keep_going = false;
  for (int i=0; i<numero_fumadores; i++) {
    semFumadores[i]->sem_signal();
  }
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

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( keep_going )
   {
     semFumadores[num_fumador]->sem_wait();
     if (keep_going) {
       semEstanquero.sem_signal();
       fumar(num_fumador);
     }
   }
}

//----------------------------------------------------------------------

int main()
{
  for (int i=0; i<numero_fumadores; i++) {
    semFumadores[i] = new Semaphore(0);
  }

  thread FThreads[numero_fumadores], estanquero (funcion_hebra_estanquero);
  keep_going = true;

  for (int i=0; i<numero_fumadores; i++) {
    FThreads[i] = thread ( funcion_hebra_fumador, i);
  }

  estanquero.join();
  for (int i=0; i<numero_fumadores; i++) {
    FThreads[i].join();
  }

  cout << "Fin del programa con todas las hebras terminadas." << endl;
}
