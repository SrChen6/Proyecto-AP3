#include <iostream>
#include <vector>
#include <assert.h> 
#include <fstream> //Lectura de archivos
#include <map> //Equivalente a diciconarios de python
#include <chrono> //Cronometra el tiempo de ejecución
#include <algorithm>
using namespace std;

typedef pair<int, int> Pair; //Tuplas
struct CompareByFirst {
    bool operator()(const Pair& a, const Pair& b) const {
        if (a.first != b.first) {
            return a.first > b.first;
        }
        return a.second < b.second;
    }
};
typedef map<Pair, int, CompareByFirst>    Map; //Diccionarios
typedef pair<Pair, Pair>     Coords; // Posición de una pieza
typedef vector<Coords>     VectCoords; //Conjunto de piezas posicionadas

// GLOBALS 
int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones + numero de piezas
int L=INT_MAX; //Longitud ans parcial
VectCoords disp = {}; //disposicion de ans parcial/total

// Inicio de cronómetro
auto start = chrono::steady_clock::now();

double finish_time(){
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  return elapsed_seconds;
}

void read_instance(char** file) {
  ifstream inp(file[1]);
  inp >> W >> N;
  int ni, pi, qi;
  while (N != 0) {
    inp >> ni >> pi >> qi;
    N -= ni;
    n[{pi, qi}] = ni;
  }
}

void write_ans(char** argv, double elapsed_seconds){
  ofstream outp(argv[2]);
  outp << elapsed_seconds << endl << L << endl;
  for (Coords bloc : disp){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
  }
}

bool compareBySecond(const pair<int, int>& a, const pair<int, int>& b) {
    return a.second < b.second; // Compare based on the second element
}

// Dado un telero, la anchura de una pieza y una posicion, devuelve si se puede añadir
bool can_add(const vector<int>& front, int a, int i){
  for (int j = 0; j < a; j++){
    if (i+j >= W) return false; // Si sobresale lateralmente
    if (front[i] < front[i+j]) return false; // Si solapa con otras piezas
  }
  return true;
}

// Añade todas las piezas, de ancho a fino, en la posición más baja posible
void greedy(char** argv){
  vector<int> front(W, 0);
  
  for(pair<Pair, int> blocs : n){
    for (int repes = 0; repes < blocs.second; repes++){
      bool been_put = false;

      Pair p = blocs.first; // Dimensions del bloc
      int a = p.first; int b = p.second;

      // Buscar de más bajo a más alto
      vector<Pair> order(W);
      for (int i = 0; i < int(W); ++i) order[i] = {i, front[i]};
      sort(order.begin(), order.end(), compareBySecond);  

      for (Pair pos : order){
        int i = pos.first;

        if (!been_put && can_add(front, a, i)){
        
          disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});
          int pivot = front[i]; // Posicion desde donde se añade la pieza
          for (int j=0; j<a; ++j) front[i+j]= pivot+b;
          been_put = true;
        }


      }
    }
  }
  L = *max_element(front.cbegin(), front.cend());
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
  
  greedy(argv); // Escribe en disp la posicion de las piezas
  
  double elapsed_seconds = finish_time();
  write_ans(argv, elapsed_seconds);
}