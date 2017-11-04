// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 2. Introducción a los monitores en C++11.
//
// archivo: monitor_em.cpp
// Ejemplo de monitores en C++11 sin variables condición
// (solo con encapsulamiento y exclusión mutua)
//
//  -- MContador1 : sin e.m.
//  -- MContador2 : con e.m. mediante acceso directo a cerrojos
//  -- MContador3 : con e.m. mediante guardas de cerrojo
//
// Historial:
// Creado en Julio de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>

using namespace std ;

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

// *****************************************************************************
// clase contador, sin exclusión mutua

class MContador1
{
   private:
   int cont ;

   public:
   MContador1( int valor_ini ) ;
   void incrementa();
   int leer_valor();
} ;
// -----------------------------------------------------------------------------

MContador1::MContador1( int valor_ini )
{
   cont = valor_ini ;
}
// -----------------------------------------------------------------------------

void MContador1::incrementa()
{
   cont ++ ;
}
// -----------------------------------------------------------------------------

int MContador1::leer_valor()
{
   return cont ;
}

// *****************************************************************************
// clase contador, con exclusión mutua mediante manipulación directa del cerrojo

class MContador2
{
   private:
   std::mutex cerrojo_mon ;
   int cont ;

   public:
   MContador2( int valor_ini ) ;
   void incrementa();
   int leer_valor();
} ;
// -----------------------------------------------------------------------------

MContador2::MContador2( int valor_ini )
{
   cont = valor_ini ;
}
// -----------------------------------------------------------------------------

void MContador2::incrementa()
{
   cerrojo_mon.lock();
   cont ++ ;
   cerrojo_mon.unlock();
}
// -----------------------------------------------------------------------------

int MContador2::leer_valor()
{
   cerrojo_mon.lock();
   int resultado = cont ;
   cerrojo_mon.unlock();
   return resultado ;
}

// *****************************************************************************
// clase contador, con exclusión mutua mediante guardas

class MContador3
{
   private:
   std::mutex cerrojo_mon ;
   int cont ;

   public:
   MContador3( int valor_ini ) ;
   void incrementa();
   int leer_valor();
} ;
// -----------------------------------------------------------------------------

MContador3::MContador3( int valor_ini )
{
   cont = valor_ini ;
}
// -----------------------------------------------------------------------------

void MContador3::incrementa()
{
   unique_lock<mutex> guarda( cerrojo_mon );
   cont ++ ;
}
// -----------------------------------------------------------------------------

int MContador3::leer_valor()
{
   unique_lock<mutex> guarda( cerrojo_mon );
   return cont ;
}

// *****************************************************************************

const int num_incrementos = 10000 ;

void funcion_hebra( MContador1 * cont )
{
   for( int i = 0 ; i < num_incrementos ; i++ )
      cont->incrementa();
}
// *****************************************************************************

int main()
{
   MContador1 contador(0) ;

   thread hebra1( funcion_hebra, &contador ),
          hebra2( funcion_hebra, &contador );

   hebra1.join();
   hebra2.join();

   cout  << "Monitor contador:" << endl
         << "valor esperado == " << 2*num_incrementos << endl
         << "valor obtenido == " << contador.leer_valor() << endl ;
}
