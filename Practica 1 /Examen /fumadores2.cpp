#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

const int numero_fumadores = 4;
int total_fumado = 0;
Semaphore *semFumadores[numero_fumadores], semEstanquero(0), semCrit(1);

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
  while( true )
  {
    int ingrediente = aleatorio<0, numero_fumadores-1>();
    cout << "\tEstanquero: " << ingrediente << endl;
    semFumadores[ingrediente]->sem_signal();
    semEstanquero.sem_wait();
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
  bool aviso_despues;
   while( true )
   {
     semFumadores[num_fumador]->sem_wait();
     aviso_despues = (total_fumado % 2) == 0;

     if (!aviso_despues) {
       cout << "Avisando antes de fumar, fumador " << num_fumador << ". Total fumado: " << total_fumado << endl;
       semEstanquero.sem_signal();
     }

     fumar(num_fumador);

     if (aviso_despues) {
       cout << "Avisando después de fumar. Total fumado: " << total_fumado << endl;
       semEstanquero.sem_signal();
     }

     semCrit.sem_wait();			  //Abrir SC
		 total_fumado++;
		 semCrit.sem_signal();			//Cerrar SC
   }
}

//----------------------------------------------------------------------

int main()
{
  for (int i=0; i<numero_fumadores; i++) {
    semFumadores[i] = new Semaphore(0);
  }

  thread FThreads[numero_fumadores], estanquero (funcion_hebra_estanquero);

  for (int i=0; i<numero_fumadores; i++) {
    FThreads[i] = thread ( funcion_hebra_fumador, i);
  }
  for (int i=0; i<numero_fumadores; i++) {
    FThreads[i].join();
  }
}
