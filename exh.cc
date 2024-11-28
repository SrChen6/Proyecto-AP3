#include <iostream>
#include <vector>
#include <assert.h> 
#include <fstream> //Lectura de archivos
#include <map> //Equivalente a diciconarios de python
#include <chrono> //Cronometra el tiempo de ejecución
using namespace std;

typedef pair<int, int> Pair; //Tuplas
typedef map<Pair, int>    Map; //Diccionarios
typedef pair<Pair, Pair>     Coords; // Posición de una pieza en la solución
typedef vector<Coords>     VectCoords; //Conjunto de piezas posicionadas


int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones + numero de piezas
int L=100; //Longitud ans parcial
int best_L;
VectCoords disp = {}; //disposicion de ans parcial/total
VectCoords best_disp = {};
int k; // Numero de piezas que falta añadir

// Inicio de cronómetro
auto start = chrono::steady_clock::now();

void read_instance(char** file) {
  ifstream in(file[1]);
  in >> W >> N;
  k = N;
  int ni, pi, qi;
  while (N != 0) {
    in >> ni >> pi >> qi;
    N -= ni;
    n[{pi, qi}] = ni;
  }
  cout << "W: "<<W<<" N: "<<k<<endl;

}

void write_ans(char** argv, double elapsed_seconds){
  ofstream outp(argv[2]);
  outp << elapsed_seconds << endl << best_L << endl;
  for (Coords bloc : best_disp){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
  }
}

void exh_search(char** argv, vector<int> front){
  if(k=0){ //Si ha añadido todas las piezas 
    if (L < best_L){
      best_L = L;
      // best_disp = disp;

      auto end = chrono::steady_clock::now();
      auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
      double elapsed_seconds = elapsed.count() / 1000.0;

      //Escribir la solución
      write_ans(argv, elapsed_seconds);
      }
  }
  else{
    //Caso recursivo
  }
}

int main(int argc, char** argv) {

  // Formato de ejecución
  if (argc == 1) {
    cout << "Makes a sanity check of a solution" << endl;
    cout << "Usage: " << argv[0] << " INPUT_FILE OUTPUT_FILE" << endl;
    exit(0);
  }

  assert(argc == 3);

  read_instance(argv);

  // Longitud a la que se ha llegado en cada columna
  vector<int> front(W, 0); 

  exh_search(argv, front);


}