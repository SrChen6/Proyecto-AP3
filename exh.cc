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

int W, N;
Map n;
int L;

void read_instance(const char* file) {
  ifstream in(file);
  in >> W >> N;
  int ni, pi, qi;
  while (N != 0) {
    in >> ni >> pi >> qi;
    N -= ni;
    n[{pi, qi}] = ni;
  }
}

void write_ans(string argv, double elapsed_seconds, VectCoords disposicion){
  //TODO: reset output file HOLA
  ofstream outp(argv);
  outp << elapsed_seconds << endl << L << endl;
  for (pair bloc : disposicion){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
  }
}

void exh_search(int W, int N, map<Pair, int> n, vector<int> front){

}

int main(int argc, char** argv) {

  // Escritura de formato de ejecución
  if (argc == 1) {
    cout << "Makes a sanity check of a solution" << endl;
    cout << "Usage: " << argv[0] << " INPUT_FILE OUTPUT_FILE" << endl;
    exit(0);
  }
  assert(argc == 3);

  // Inicio de cronómetro
  auto start = chrono::steady_clock::now();

  // Apertura de archivos
  ofstream outp(argv[2]);
  ifstream inp(argv[1]);

  // Test para ver el output file
  L = 500;
  Coords pieza = {{0,0}, {1,1}};
  VectCoords disposicion = {pieza};
  
  // Lectura de archivo de entrada
  read_instance(argv[1]);

  // Longitud a la que se ha llegado en cada columna
  vector<int> front(W, 0); 

  //Ejecución de cerca exhaustiva
  exh_search(W, N, n, front);

  //Test para ver el output file
  cout << n[{1,2}];
  for (auto& dim : n){
    disposicion.push_back({dim.first, {-1, -1}});
  }

  // Finalización del cronómetro
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;

  //Escribir la solución
  write_ans(argv[2], elapsed_seconds, disposicion);
  outp.close();
}