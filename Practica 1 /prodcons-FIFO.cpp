#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//**********************************************************************
// variables compartidas

const int num_items = 40 ,   // número de items
	       	tam_vec = 10 ;   // tamaño del buffer
unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}, // contadores de verificación: consumidos
					buffer[tam_vec];

Semaphore semProd(tam_vec), semCons(0);
int indexProd = 0, indexCons = 0;
int numero_prod = 4, numero_cons = 4;

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

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

int producir_dato()
{
   static int contador = 0 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "producido: " << contador << endl << flush ;

   cont_prod[contador] ++ ;
   return contador++ ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned dato )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "                  consumido: " << dato << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

//----------------------------------------------------------------------

void  funcion_hebra_productora(int num_items_prod)
{
   for( unsigned i = 0 ; i < num_items_prod ; i++ )
   {
		 int dato = producir_dato();
		 semProd.sem_wait();
			buffer[indexProd] = dato;
	 		indexProd = (indexProd + 1) % tam_vec;
		 semCons.sem_signal();
   }
}

//----------------------------------------------------------------------

void funcion_hebra_consumidora(int num_items_cons)
{
   for( unsigned i = 0 ; i < num_items_cons ; i++ )
   {
      int dato;
			semCons.sem_wait();
			dato = buffer[indexCons];
			indexCons = (indexCons + 1) % tam_vec;
			semProd.sem_signal();
      consumir_dato( dato ) ;
    }
}
//----------------------------------------------------------------------z

int main()
{
   cout << "--------------------------------------------------------" << endl
        << "Problema de los productores-consumidores (solución LIFO)." << endl
        << "--------------------------------------------------------" << endl
        << flush ;


	thread prodThreads[numero_prod], consThreads[numero_cons];

	for (int i=0; i<numero_prod; i++) {
		prodThreads[i] = thread ( funcion_hebra_productora, num_items/numero_prod);
	}
	for (int i=0; i<numero_prod; i++) {
		consThreads[i] = thread ( funcion_hebra_consumidora, num_items/numero_cons);
	}

	for (int i=0; i<numero_prod; i++) {
		prodThreads[i].join();
		consThreads[i].join();
	}

   test_contadores();
}
