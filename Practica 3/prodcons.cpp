// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: prodcons2.cpp
// Implementación del problema del productor-consumidor con
// un proceso intermedio que gestiona un buffer finito y recibe peticiones
// en orden arbitrario
// (versión con un único productor y un único consumidor)
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <mpi.h>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
   num_prods             = 4 ,
   num_cons              = 5 ,
   id_buffer             = 0 ,
   id_productor_max      = num_prods ,
   num_procesos_esperado = 1 + num_cons + num_prods ,
   num_items             = 100,
   num_items_prod        = num_items/num_prods,
   num_items_cons        = num_items/num_cons,
   tam_vector            = 40,
   etiq_prod             = 1,
   etiq_cons             = 2,
   etiq_aux              = 0;


// IMPORTANTE: Se tiene que cumplir: num_prods * num_items_prod = num_cons * num_items_cons

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
// ---------------------------------------------------------------------
// producir produce los numeros en secuencia (1,2,3,....)
// y lleva espera aleatorio
int producir(int id_propio)
{
   static int contador = (id_propio-1) * num_items_prod ;
   sleep_for( milliseconds( aleatorio<10,100>()) );
   contador++ ;
   cout << "Productor " << id_propio << " ha producido valor " << contador << endl << flush;
   return contador ;
}
// ---------------------------------------------------------------------

void funcion_productor(int id_propio)
{
   for ( int i= 0 ; i < num_items_prod; i++ )
   {
      // producir valor
      int valor_prod = producir(id_propio);
      // enviar valor
      cout << "Productor " << id_propio << " va a enviar valor " << valor_prod << endl << flush;
      MPI_Ssend( &valor_prod, 1, MPI_INT, id_buffer, etiq_prod, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void consumir( int valor_cons , int id_propio)
{
   // espera bloqueada
   sleep_for( milliseconds( aleatorio<110,200>()) );
   cout << "\tConsumidor " << id_propio << " ha consumido valor " << valor_cons << endl << flush ;
}
// ---------------------------------------------------------------------

void funcion_consumidor(int id_propio)
{
   int         peticion,
               valor_rec = 1 ;
   MPI_Status  estado ;

   for( int i=0 ; i < num_items_cons; i++ )
   {
      MPI_Ssend( &peticion,  1, MPI_INT, id_buffer, etiq_cons, MPI_COMM_WORLD);
      MPI_Recv ( &valor_rec, 1, MPI_INT, id_buffer, etiq_aux, MPI_COMM_WORLD,&estado );
      cout << "\tConsumidor " << id_propio << " ha recibido valor " << valor_rec << endl << flush ;
      consumir( valor_rec, id_propio);
   }
}
// ---------------------------------------------------------------------

void funcion_buffer()
{
   int        buffer[tam_vector],      // buffer con celdas ocupadas y vacías
              valor,                   // valor recibido o enviado
              primera_libre       = 0, // índice de primera celda libre
              primera_ocupada     = 0, // índice de primera celda ocupada
              num_celdas_ocupadas = 0; // número de celdas ocupadas
   MPI_Status estado ;                 // metadatos del mensaje recibido

   int tope = num_items*2, etiqueta;

   for( int i=0 ; i < tope ; i++ )  {
      // 1. determinar si puede enviar solo prod., solo cons, o todos
      if ( num_celdas_ocupadas == 0 )               // si buffer vacío
         etiqueta = etiq_prod ;       // $~~~$ solo prod.
      else if ( num_celdas_ocupadas == tam_vector ) // si buffer lleno
         etiqueta = etiq_cons ;      // $~~~$ solo cons.
      else                                          // si no vacío ni lleno
        etiqueta = MPI_ANY_TAG ;     // $~~~$ cualquiera

      // 2. recibir un mensaje del emisor o emisores aceptables

      MPI_Recv( &valor, 1, MPI_INT, MPI_ANY_SOURCE, etiqueta, MPI_COMM_WORLD, &estado );

      // 3. procesar el mensaje recibido

      switch ( estado.MPI_TAG ) {// leer emisor del mensaje en metadatos
        case etiq_prod:
          buffer[primera_libre] = valor ;
          primera_libre = (primera_libre+1) % tam_vector ;
          num_celdas_ocupadas++ ;
          cout << "\t\tBuffer ha recibido valor " << valor << endl ;
          break;

        case etiq_cons:
          valor = buffer[primera_ocupada] ;
          primera_ocupada = (primera_ocupada+1) % tam_vector ;
          num_celdas_ocupadas-- ;
          cout << "\t\tBuffer va a enviar valor " << valor << endl ;
          MPI_Ssend( &valor, 1, MPI_INT, estado.MPI_SOURCE, etiq_aux, MPI_COMM_WORLD);
          break;
      }
   }
}

// ---------------------------------------------------------------------

int main( int argc, char *argv[] )
{
   int id_propio, num_procesos_actual;

   // inicializar MPI, leer identif. de proceso y número de procesos
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
   MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );

   if ( num_procesos_esperado == num_procesos_actual )
   {
      // ejecutar la operación apropiada a 'id_propio'
      if ( id_propio == id_buffer ) {
        funcion_buffer();
      } else if ( id_propio <= id_productor_max ) {
        funcion_productor(id_propio);
      } else {
        funcion_consumidor(id_propio);
      }
   }
   else
   {
      if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
      { cout << "el número de procesos esperados es:    " << num_procesos_esperado << endl
             << "el número de procesos en ejecución es: " << num_procesos_actual << endl
             << "(programa abortado)" << endl ;
      }
   }

   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}
