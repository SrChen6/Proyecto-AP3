#include <iostream>
#include <vector>
#include <assert.h> 
#include <fstream> //Lectura de archivos
#include <map> //Equivalente a diciconarios de python
#include <chrono> //Cronometra el tiempo de ejecución
#include <algorithm>
using namespace std;

typedef pair<int, int> Pair; //Tuplas
typedef map<Pair, int>    Map; //Diccionarios
typedef pair<Pair, Pair>     Coords; // Posición de una pieza en la solución
typedef vector<Coords>     VectCoords; //Conjunto de piezas posicionadas

// GLOBALS 
int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones + numero de piezas
int L=999999; //Longitud ans parcial
int best_L = 0;
VectCoords disp = {}; //disposicion de ans parcial/total
VectCoords best_disp = {};

// Inicio de cronómetro
auto start = chrono::steady_clock::now();

void read_instance(char** file, int& k) {
  ifstream inp(file[1]);
  inp >> W >> N;
  k = N;
  int ni, pi, qi;
  while (N != 0) {
    inp >> ni >> pi >> qi;
    N -= ni;
    n[{pi, qi}] = ni;
  }
  std::cout << "W: "<<W<<" N: "<<k<<endl; //TEMPORAL: PARA FACILITAR DEBUGGING

}

void write_ans(char** argv, double elapsed_seconds){
  ofstream outp(argv[2]);
  outp << elapsed_seconds << endl << best_L << endl;
  for (Coords bloc : best_disp){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
  }
}

// No borrado por si metemos más podas 
bool valid(){
  return L < best_L;
}

// void add_piece(Pair p, vector<int> front){
//   n[p] -=1; // Quitar la pieza de que sea libre
//   int a = p.first
  
//   for (int i=0; i<=W-a, ++i){
//     if (std::all_of(front.cbegin(), front.cend(), [](int j) { return j <=i; }))
//   }
// }

void undo_changes(vector<int> front, VectCoords disp){
  Coords pos_piece = disp.pop_back();
  Pair ul = pos_piece.first;
  Pair 
}

void exh_search(char** argv, vector<int> front, int k){
  if(k==0){ //Si ha añadido todas las piezas 
    best_L = L;
    best_disp = disp;

    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    double elapsed_seconds = elapsed.count() / 1000.0;

    //Escribir la solución
    write_ans(argv, elapsed_seconds);
  }
  else{
    if(valid()){ //poda
      for(pair<Pair, int> bloc : n){
        if (bloc.second > 0) //Si quedan piezas de la dimension
        {
          if(bloc.first.first == bloc.first.second){ // Si la pieza es cuadrada
            add_piece(bloc.first, front); // update front, disp
          }
          else{
            add_piece(bloc.first, front);
            undo_changes(front, disp);
            
            add_piece({bloc.first.second,bloc.first.first},front);
          }
          exh_search(argv, front, k-1);
          undo_changes(front, disp);
        }
      }
    }
  }
}

int main(int argc, char** argv) {

  // Formato de ejecución
  if (argc == 1) {
    cout << "Makes a sanity check of a solution" << endl;
    cout << "Usage: " << argv[0] << " INPUT_FILE OUTPUT_FILE" << endl;
    exit(0);
  }

  int k; // Contador recursiu
  assert(argc == 3);
  read_instance(argv, k);
  
  // Calcular una cota superior para realizar podas
  for(pair<Pair, int> bloc : n){
    best_L += bloc.second * min(bloc.first.first, bloc.first.second);
  }

  // Longitud a la que se ha llegado en cada columna
  vector<int> front(W, 0); 

  exh_search(argv, front, k);


}