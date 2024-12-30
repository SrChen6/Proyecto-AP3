
#include <iostream>
#include <vector>
#include <assert.h> 
#include <fstream> //Lectura de archivos
#include <map> //Equivalente a diciconarios de python
#include <chrono> //Cronometra el tiempo de ejecución
#include <algorithm>
using namespace std;

// Idea: Reordenar el vector de piezas según grande a pequeño y generar dos vectores : una con la mitad grande 
// y otra con la mitad pequeña. 

typedef pair<int, int> Pair; //Tuplas
struct CompareByFirst {
    bool operator()(const Pair& a, const Pair& b) const {
        if (a.first != b.first) {
            return a.first > b.first; // Compare first elements
        }
        return a.second < b.second; // If first elements are equal, compare second elements
    }
};
typedef map<Pair, int, CompareByFirst>    Map; //Diccionarios
typedef pair<Pair, Pair>     Coords; // Posición de una pieza en la solución
typedef vector<Coords>     VectCoords; //Conjunto de piezas posicionadas

// GLOBALS 
int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones + numero de piezas
int L=999999; //Longitud ans parcial
VectCoords disp = {}; //disposicion de ans parcial/total

// Optimization globals
vector< Pair > n_vect = {} ; // Lista con piezas (incluyendo repetidas) ordenada
vector< Pair > n_res; // Piezas pequeñas no colocadas en la primera pasada
vector< Pair > blocs_to_put = {} ; 


// Inicio de cronómetro
auto start = chrono::steady_clock::now();

void read_instance(char** file) {
  ifstream inp(file[1]);
  inp >> W >> N;
  int ni, pi, qi;
  while (N != 0) {
    inp >> ni >> pi >> qi;
    N -= ni;
    n[{pi, qi}] = ni;
  }
  std::cout << "W: "<<W<<" N: "<<endl; //TEMPORAL: PARA FACILITAR DEBUGGING
  for (const auto& p : n){ cout <<n[{p.first.first, p.first.second}]<<" "<< p.first.first << " " <<p.first.second<<endl;}
}

void write_ans(char** argv, double elapsed_seconds){
  ofstream outp(argv[2]);
  outp << elapsed_seconds << endl << L << endl;
  cout << elapsed_seconds << endl << "L :" << L << endl;
  for (Coords bloc : disp){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
    cout << bloc.first.first << " " << bloc.first.second << " ";
    cout << bloc.second.first << " " << bloc.second.second << endl;
  }
  cout <<endl;
}

// void add_piece(Pair p, vector<int> front){
//   n[p] -=1; // Quitar la pieza de que sea libre
//   int a = p.first
  
//   for (int i=0; i<=W-a, ++i){
//     if (std::all_of(front.cbegin(), front.cend(), [](int j) { return j <=i; }))
//   }
// }

bool compareBySecond(const pair<int, int>& a, const pair<int, int>& b) {
    return a.second < b.second; // Compare based on the second element
}

int phi_L(vector<Pair> n_list){
  // Basada en la implementación del greedy, 
  // dada una ordenación de las piezas las coloca
  // para obtener una L.
  vector<int> front(W, 0);
  for(Pair p: n_list){
      bool been_put = false;

      int a = p.first; int b = p.second;

      vector<Pair> order(front.size());
      for (int i = 0; i < int(front.size()); ++i) order[i] = {i, front[i]};
      // Ordenar de más bajo a más alto
      sort(order.begin(), order.end(), compareBySecond);  

      for (Pair pos : order){ //Buscar de debajo a arriba
        int i = pos.first;
        bool may_add_here = true;
        int j = 0;
        while (j <a && may_add_here){ // Si se puede añadir aquí
          may_add_here = may_add_here && (front[i] >= front[i+j]) && i <= W-a;
          ++j;
        }

        if (!been_put && may_add_here){ //Añadir la pieza
          disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});
          vector<int> new_front = front;
          // cout << a<<" "<<b<<endl;
          for (int j=0; j<a; ++j) new_front[i+j]= front[i]+b;
          front = new_front;
          been_put = true;
        }
      }
    
  }
  cout << int(disp.size()) << endl;
  L = *max_element(front.cbegin(), front.cend());
  return L;
}

void metah(char** argv){
  vector< Pair > n_orig = {};
  for(pair<Pair, int> blocs : n) {
    for (int repes = 0; repes < blocs.second; repes++) n_orig.push_back(blocs.first);
  }

  reverse(n_orig.begin(), n_orig.end());
  cout << "L orig: " << int(n_orig.size()) << "-"<< phi_L(n_orig) << endl;

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
  
  metah(argv);

  // Mira el tiempo
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;

  write_ans(argv, elapsed_seconds);

  // Finalización de la busqueda
  // auto end = chrono::steady_clock::now();
  // auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  // double elapsed_seconds = elapsed.count() / 1000.0;
  cout << "Ha tardat en trobar totes les combinacions: " << elapsed_seconds<< endl;
}