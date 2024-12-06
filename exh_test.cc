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
// int L=0; //Longitud ans parcial
// int best_L=1; 
// VectCoords disp = {}; //disposicion de ans parcial/total
// VectCoords best_disp = {};

// Declaración de funciones
void exh_search(char** argv, vector<int>& front, VectCoords best_disp, VectCoords disp, int best_L, int L, int k);

// Inicio de cronómetro
auto start = chrono::steady_clock::now();

void read_instance(char** file, int& k) {
}

void write_ans(char** argv, double elapsed_seconds){
}

// Retorna true si la pieza es una de las dadas en el inp, sino es el transpuesto
bool is_original(Pair piece){
}

void undo_change(vector<int>& front, VectCoords& disp){
  Coords pos_piece = disp.back();
  disp.pop_back();

  Pair ul = pos_piece.first;
  Pair dr = pos_piece.second;
  Pair piece = {dr.first-ul.first+1, dr.second-ul.second+1};

  for(int j = ul.first ; j <= dr.first; j++){
    front[j] -= piece.second;
  }

  if (is_original(piece)){ //Por si la pieza fue rotada
    n[piece] += 1;
  }
  else{
    Pair orig_piece = {piece.second, piece.first};
    n[orig_piece] += 1;
  }
}

void add_piece( char** argv, Pair p, vector<int> front, VectCoords& best_disp,
                VectCoords& disp, int best_L, int L, int k){
    Pair orig_p = p; //Si la pieza esta rotada, consultar las dimensiones originales en n
    if (!is_original(p)) orig_p = {p.second, p.first};
    if(n[orig_p] > 0){
    int a = p.first;
    int b = p.second;
    bool may_add_here = true;
    
    for (int i=0; i<=W-a; ++i){ //Mirar cada casilla posible
      may_add_here = true;
      int j = 0;
      while (j <a && may_add_here){ // Si se puede añadir aquí
        may_add_here = may_add_here && (front[i] >= front[i+j]);
        ++j;
      }

      if (may_add_here){ //Añadir la pieza
        n[orig_p] -=1; // Quitar la pieza de que sea libre
        disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});

        for (int j=0; j<a; ++j) front[i+j]= front[i]+b;

        L = *max_element(front.cbegin(), front.cend());
        exh_search(argv, front, best_disp, disp, best_L, L, k-1);
        undo_change(front, disp);
      }
    }
  }
}

void exh_search(char** argv, vector<int>& front, VectCoords best_disp, VectCoords disp,
                 int best_L, int L, int k){
  if(k==0){ //Si ha añadido todas las piezas 
    if(L < best_L){
      best_L = L;
      best_disp = disp;

      // Mira el tiempo
      auto end = chrono::steady_clock::now();
      auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
      double elapsed_seconds = elapsed.count() / 1000.0;

      write_ans(argv, elapsed_seconds);
    }
  }
  else{
    if(L < best_L){ //poda
      for(pair<Pair, int> blocs : n){
        Pair bloc = blocs.first;
        if(bloc.first == bloc.second){ // Si la pieza es cuadrada
          add_piece(argv, bloc, front, best_disp, disp, best_L, L, k); // update front, disp
        }
        else{
          add_piece(argv, bloc, front, best_disp, disp, best_L, L, k);
          //Caso rotado
          add_piece(argv, {bloc.second, bloc.first}, front, best_disp, disp, best_L, L, k);
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  ios_base::sync_with_stdio(false);

  // Formato de ejecución
  if (argc == 1) {
    cout << "Makes a sanity check of a solution" << endl;
    cout << "Usage: " << argv[0] << " INPUT_FILE OUTPUT_FILE" << endl;
    exit(0);
  }

  int k; // Contador recursivo
  assert(argc == 3);
  read_instance(argv, k);
  
  // Calcular una cota superior para realizar podas
  int best_L = 0;
  for(pair<Pair, int> bloc : n){
    best_L += bloc.second * min(bloc.first.first, bloc.first.second);
  }

  // Longitud a la que se ha llegado en cada columna
  vector<int> front(W, 0); 
  VectCoords best_disp ={}, disp = {};
  exh_search(argv, front, best_disp, disp, best_L, 0, k);

  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  cout << "Ha tardat en trobar totes les combinacions: " << elapsed_seconds<< endl;
}