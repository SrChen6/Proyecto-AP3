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
typedef vector<Coords>     VectCoords; 

void read_instance(const char* file) {
  ifstream in(file);
  Pair Telar;
  in >> Telar.first >> Telar.second;
  Map n;
  int ni, pi, qi;
  while (Telar.second != 0) {
    in >> ni >> pi >> qi;
    Telar.second -= ni;
    n[{pi, qi}] = ni;
  }
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

  // Variables d'entrada i sortida
  Pair Telar;
  Map n;

  int L;
  Coords pieza;
  VectCoords organizacion;
  
  // Lectura de archivo de entrada
  inp >> Telar.first >> Telar.second;
  int ni, pi, qi;
  while (Telar.second != 0) {
    inp >> ni >> pi >> qi;
    Telar.second -= ni;
    n[{pi, qi}] = ni;
  }

  // Finalización del cronómetro
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  write_ans(elapsed_seconds, L, )
  outp.close();
}