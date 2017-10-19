// -----------------------------------------------------------------------------
// Sistemas concurrentes y Distribuidos.
// Seminario 1. Programación Multihebra y Semáforos.
//
// Ejemplo 9 (ejemplo9.cpp)
// Calculo concurrente de una integral. Plantilla para completar.
//
// Historial:
// Creado en Abril de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <chrono>  // incluye now, time\_point, duration
#include <future>
#include <vector>
#include <cmath>

using namespace std ;
using namespace std::chrono;

const long m  = 1024l*1024l*1024l,
           n  = 2;


// -----------------------------------------------------------------------------
// evalua la función $f$ a integrar ($f(x)=4/(1+x^2)$)
double f( double x )
{
  return 4.0/(1.0+x*x) ;
}
// -----------------------------------------------------------------------------
// calcula la integral de forma secuencial, devuelve resultado:
double calcular_integral_secuencial()
{
   double suma = 0.0 ;                        // inicializar suma
   for( long i = 0 ; i < m ; i++)            // para cada $i$ entre $0$ y $m-1$:
      suma += f( (i+double(0.5)) /m );         //   $~$ añadir $f(x_i)$ a la suma actual
   return suma/m ;                            // devolver valor promedio de $f$
}

// -----------------------------------------------------------------------------
// función que ejecuta cada hebra: recibe $i$ ==índice de la hebra, ($0\leq i<n$)
double funcion_hebra(int num_hebra, bool reparto_entrelazado) {
  double suma = 0.0;
  if (!reparto_entrelazado) {
    long bloque = m/n, inicio=num_hebra*bloque, tope = inicio+bloque;
    for(long i=inicio; i<tope; i++)
       suma += f((i+double(0.5))/m);
  } else {
    for(long i=num_hebra; i<m; i+=n)
       suma += f((i+double(0.5))/m);
  }
  return suma;
}

// -----------------------------------------------------------------------------
// calculo de la integral de forma concurrente
double calcular_integral_concurrente(bool reparto_entrelazado) {
  future<double> futuros[n  ];
  for(int i=1; i<n; i++ ) {
    futuros[i] = async(launch::async, funcion_hebra, i, reparto_entrelazado);
  }
  double result = funcion_hebra(0, reparto_entrelazado);
  for(int i=1 ; i<n ; i++) {
     result += futuros[i].get();
   }
   return result/m;
}
// -----------------------------------------------------------------------------

int main()
{

  time_point<steady_clock> inicio_sec  = steady_clock::now() ;
  const double             result_sec  = calcular_integral_secuencial(  );
  time_point<steady_clock> fin_sec     = steady_clock::now() ;
  double x = sin(0.4567);
  time_point<steady_clock> inicio_conc_sec = steady_clock::now() ;
  const double             result_conc_sec = calcular_integral_concurrente(0);
  time_point<steady_clock> fin_conc_sec    = steady_clock::now() ;
  time_point<steady_clock> inicio_conc_ent = steady_clock::now() ;
  const double             result_conc_ent = calcular_integral_concurrente(1);
  time_point<steady_clock> fin_conc_ent    = steady_clock::now() ;
  duration<float,milli>    tiempo_sec  = fin_sec  - inicio_sec ,
                           tiempo_conc_sec = fin_conc_sec - inicio_conc_sec,
                           tiempo_conc_ent = fin_conc_ent - inicio_conc_ent;
  const float              porc_sec        = 100.0*tiempo_conc_sec.count()/tiempo_sec.count() ;
  const float              porc_ent       = 100.0*tiempo_conc_ent.count()/tiempo_sec.count() ;

  constexpr double pi = 3.14159265358979323846l ;

  cout << "Número de muestras (m)   : " << m << endl
       << "Número de hebras (n)     : " << n << endl
       << setprecision(18)
       << "Valor de PI              : " << pi << endl
       << "Resultado secuencial     : " << result_sec  << endl
       << "Resultado concurrente S. : " << result_conc_sec << endl
       << "Resultado concurrente E. : " << result_conc_ent << endl
       << setprecision(5)
       << "Tiempo secuencial        : " << tiempo_sec.count()  << " milisegundos. " << endl
       << "Tiempo concurrente Sec.  : " << tiempo_conc_sec.count() << " milisegundos. " << endl
       << "Tiempo concurrente Ent.  : " << tiempo_conc_ent.count() << " milisegundos. " << endl
       << setprecision(4)
       << "Porcentaje t.concS/t.sec.: " << porc_sec << "%" << endl
       << "Porcentaje t.concE/t.sec.: " << porc_ent << "%" << endl;
}
