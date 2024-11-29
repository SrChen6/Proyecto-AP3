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
int best_L; //Inicialitzat a main()
VectCoords disp = {}; //disposicion de ans parcial/total
VectCoords best_disp = {};

// Declaración de funciones
void exh_search(char** argv, vector<int>& front, int k);

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

void undo_change(vector<int>& front, VectCoords& disp){
  Coords pos_piece = disp.back();
  cout << "pieza quitada " << pos_piece.first.first << pos_piece.first.second<<endl;
  disp.pop_back();

  Pair ul = pos_piece.first;
  Pair dr = pos_piece.second;
  Pair piece = {dr.first-ul.first, dr.second-ul.second};
  n[piece] += 1;
}

void add_piece(Pair p, vector<int>& front, char** argv, int k){
  n[p] -=1; // Quitar la pieza de que sea libre
  int a = p.first;
  bool may_add_here;
  
  for (int i=0; i<=W-a; ++i){ //Mirar cada casilla posible
    may_add_here = true;
    int j = 0;
    while (j <a && may_add_here){
      may_add_here = may_add_here && (front[i] <= front[i+j]);
      ++j;
    }

    if (may_add_here){ //Añadir la pieza
      for (int j=0; j<a; ++j) front[i+j]+=p.second ;
      L = *max_element(front.cbegin(), front.cend());
      disp.push_back({{i, front[i]},{i+p.first, front[i]+p.second}});
      cout << "added piece " << p.first << p.second << endl;
      exh_search(argv, front, k-1);
      undo_change(front, disp);
    }
  }
}

void exh_search(char** argv, vector<int>& front, int k){
  if(k==0){ //Si ha añadido todas las piezas 
    best_L = L;
    best_disp = disp;
    cout << "best_L" << L <<endl;
    cout << "L " << L << endl;

    // Mira el tiempo
    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    double elapsed_seconds = elapsed.count() / 1000.0;

    //Escribir la solución
    write_ans(argv, elapsed_seconds);
  }
  else{
    if(valid()){ //poda
      cout << "k index" << k<< endl;
      for(pair<Pair, int> bloc : n){
        if (bloc.second > 0) //Si quedan piezas de la dimension
        {
          cout << "entra a caso rec" << endl;
          if(bloc.first.first == bloc.first.second){ // Si la pieza es cuadrada
            add_piece(bloc.first, front, argv, k); // update front, disp
          }
          else{
            add_piece(bloc.first, front, argv, k);
            //Caso rotado
            add_piece({bloc.first.second, bloc.first.first},front, argv, k);
          }
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
  cout << "best L " <<best_L <<endl;

  // Longitud a la que se ha llegado en cada columna
  vector<int> front(W, 0); 

  exh_search(argv, front, k);


}