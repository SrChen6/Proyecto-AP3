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
int L=0; //Longitud ans parcial
int best_L=1; 
VectCoords disp = {}; //disposicion de ans parcial/total
VectCoords best_disp = {};

// Declaración de funciones
void exh_search(char** argv, vector<int>& front, int k);

// Inicio de cronómetro
auto start = chrono::steady_clock::now();

void write_n(){
  for(pair<Pair, int> bloc : n){
    cout << bloc.first.first << " " << bloc.first.second << " ; " << bloc.second << endl;
  }
}

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
  write_n();
  std::cout << "W: "<<W<<" N: "<<k<<endl; //TEMPORAL: PARA FACILITAR DEBUGGING
}

void write_ans(char** argv, double elapsed_seconds){
  // Escribe las soluciones encontradas por terminal
  ofstream outp(argv[2]);
  outp << elapsed_seconds << endl << best_L << endl;
  cout << elapsed_seconds << endl << best_L << endl;
  for (Coords bloc : best_disp){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
    cout << bloc.first.first << " " << bloc.first.second << " ";
    cout << bloc.second.first << " " << bloc.second.second << endl;
  }
  cout <<endl;
}

// No borrado por si metemos más podas 
bool valid(){
  return L < best_L;
}

bool is_original(Pair piece){
  for (pair<Pair, int> original : n){
    if (piece == original.first) return true;
  }
  return false;
}

void undo_change(vector<int>& front){
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

void add_piece(Pair p, vector<int>& front, char** argv, int k){
    Pair orig_p = p; //Si la pieza esta rotada, consultar las dimensiones originales
    if (!is_original(p)) orig_p = {p.second, p.first};
    if(n[orig_p] > 0){
    int a = p.first;
    int b = p.second;
    bool may_add_here = true;
    
    for (int i=0; i<=W-a; ++i){ //Mirar cada casilla posible
      may_add_here = true;
      int j = 0;
      while (j <a && may_add_here){ // Si se puede añadir aquí
        may_add_here = may_add_here && (front[i] <= front[i+j]);
        ++j;
      }

      if (may_add_here){ //Añadir la pieza
        n[orig_p] -=1; // Quitar la pieza de que sea libre
        disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});

        for (int j=0; j<a; ++j) front[i+j]+=b;
//Problema: la actualización a front debría ser front[i+j] = front[i]+b;
//Sin embargo, si se hace esto luego al quitar la pieza habremos perdido la información 
//sobre qué posicion era la que había antes de añadir la pieza

        L = *max_element(front.cbegin(), front.cend());
        exh_search(argv, front, k-1);
        undo_change(front);
      }
    }
  }
}

void exh_search(char** argv, vector<int>& front, int k){
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
          add_piece(bloc, front, argv, k); // update front, disp
        }
        else{
          add_piece(bloc, front, argv, k);
          //Caso rotado
          add_piece({bloc.second, bloc.first},front, argv, k);
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

  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  cout << "Ha tardat en trobar totes les combinacions: " << elapsed_seconds<< endl;
  write_n();
}