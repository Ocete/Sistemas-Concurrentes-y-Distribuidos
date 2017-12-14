// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: filosofos-plantilla.cpp
// Implementación del problema de los filósofos (sin camarero).
// Plantilla para completar.
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------


#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
   num_filosofos = 5 ,
   num_procesos  = 2*num_filosofos + 1,
   etiq_sentarse = 0,
   etiq_levantarse = 1,
   etiq_aceptado = 2,
   etiq_no_aceptado = 3;

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

void funcion_filosofos( int id )
{
  MPI_Status estado ;
  int id_ten_izq = (id+1)% num_procesos,
      id_ten_der = (id-1),
      id_cam = 0,
      value = 0;

  if (id_ten_izq == 0)
    id_ten_izq = 1;


  while ( true )
  {

    // Solicita levantarse
    cout <<"Filósofo " <<id << " solicita sentarse" <<endl;
    do {
      MPI_Ssend( &value, 1, MPI_INT, id_cam, etiq_sentarse, MPI_COMM_WORLD );
      MPI_Recv ( &value, 1, MPI_INT, id_cam, MPI_ANY_TAG, MPI_COMM_WORLD, &estado );
    } while (estado.MPI_TAG == etiq_no_aceptado);

    // Solicita tenedores
    cout <<"Filósofo " <<id << " solicita ten. izq." <<id_ten_izq <<endl;
    MPI_Ssend( &value, 1, MPI_INT, id_ten_izq, 0, MPI_COMM_WORLD );

    cout <<"Filósofo " <<id <<" solicita ten. der." <<id_ten_der <<endl;
    MPI_Ssend( &value, 1, MPI_INT, id_ten_der, 0, MPI_COMM_WORLD );

    // Come
    cout <<"Filósofo " <<id <<" comienza a comer" <<endl ;
    sleep_for( milliseconds( aleatorio<10,100>() ) );

    // Suelta tenedores
    cout <<"Filósofo " <<id <<" suelta ten. izq. " <<id_ten_izq <<endl;
    MPI_Ssend ( &value, 1, MPI_INT, id_ten_izq, 0, MPI_COMM_WORLD );

    cout<< "Filósofo " <<id <<" suelta ten. der. " <<id_ten_der <<endl;
    MPI_Ssend ( &value, 1, MPI_INT, id_ten_der, 0, MPI_COMM_WORLD );

    // Solicita levantarse
    cout <<"Filósofo " <<id << " solicita levantarse" <<endl;
    do {
      MPI_Ssend( &value, 1, MPI_INT, id_cam, etiq_levantarse, MPI_COMM_WORLD );
      MPI_Recv ( &value, 1, MPI_INT, id_cam, MPI_ANY_TAG, MPI_COMM_WORLD, &estado );
    } while (estado.MPI_TAG == etiq_no_aceptado);


    // Piensa
    cout << "Filosofo " << id << " comienza a pensar" << endl;
    sleep_for( milliseconds( aleatorio<10,100>() ) );
 }
}
// ---------------------------------------------------------------------

void funcion_tenedores( int id )
{
  int valor, id_filosofo ;  // valor recibido, identificador del filósofo
  MPI_Status estado ;       // metadatos de las dos recepciones

  while ( true )
  {
     MPI_Recv ( &valor, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &estado );
     id_filosofo = estado.MPI_SOURCE;
     cout <<"\tTen. " << id <<" ha sido cogido por filo. " <<id_filosofo <<endl;

     MPI_Recv ( &valor, 1, MPI_INT, id_filosofo, 0, MPI_COMM_WORLD, &estado );
     cout <<"\tTen. "<< id << " ha sido liberado por filo. " <<id_filosofo <<endl ;
  }
}
// ---------------------------------------------------------------------

void funcion_camarero()
{
  int valor, etiq_aceptable, etiq_respuesta, filosofos_sentados = 0;  // valor recibido, identificador del filósofo
  MPI_Status estado ;       // metadatos de las dos recepciones
  int contador = 0;

  while ( true )
  {
     if (filosofos_sentados == num_filosofos-1) {
       etiq_aceptable = etiq_levantarse;
     } else {
       etiq_aceptable = MPI_ANY_TAG;
     }

     do {
       MPI_Recv ( &valor, 1, MPI_INT, MPI_ANY_SOURCE, etiq_aceptable, MPI_COMM_WORLD, &estado );
       etiq_respuesta = estado.MPI_SOURCE/2 % 2 == contador % 2 ? etiq_aceptado : etiq_no_aceptado;
       MPI_Ssend ( &valor, 1, MPI_INT, estado.MPI_SOURCE, etiq_respuesta, MPI_COMM_WORLD );
     } while (etiq_respuesta == etiq_no_aceptado);

     if (estado.MPI_TAG == etiq_sentarse) {
       filosofos_sentados++;
       cout <<"\t\tCamararero sienta a filosofo " << estado.MPI_SOURCE << ". Filosofos sentados: " << filosofos_sentados << endl;
     } else if (estado.MPI_TAG == etiq_levantarse) {
       filosofos_sentados--;
       cout <<"\t\tCamararero levanta a filosofo " << estado.MPI_SOURCE << ". Filosofos sentados: " << filosofos_sentados << endl;
     } else {
       cout <<"\t\tCamararero ha recibido un mensaje erróneo" << endl;
       return;
     }

     contador++;
  }
}
// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int id_propio, num_procesos_actual ;

   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
   MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


   if ( num_procesos == num_procesos_actual )
   {
      // ejecutar la función correspondiente a 'id_propio'
      if ( id_propio == 0)
        funcion_camarero ();
      else if ( id_propio % 2 == 0 )     // si es par
         funcion_filosofos( id_propio ); //   es un filósofo
      else                               // si es impar
         funcion_tenedores( id_propio ); //   es un tenedor
   }
   else
   {
      if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
      { cout << "el número de procesos esperados es:    " << num_procesos << endl
             << "el número de procesos en ejecución es: " << num_procesos_actual << endl
             << "(programa abortado)" << endl ;
      }
   }

   MPI_Finalize( );
   return 0;
}

// ---------------------------------------------------------------------
