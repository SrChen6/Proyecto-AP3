
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
  // std::cout << "W: "<<W<<" N: "<<endl; //TEMPORAL: PARA FACILITAR DEBUGGING
  // for (const auto& p : n){ cout <<n[{p.first.first, p.first.second}]<<" "<< p.first.first << " " <<p.first.second<<endl;}
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

bool compareBySecond(const pair<int, int>& a, const pair<int, int>& b) {
  if (a.second != b.second) return a.second < b.second; // Compare based on the second element
  return a.first < b.first;
}

void createVectorOfMap(){
  for(pair<Pair, int> blocs : n) {
    for (int repes = 0; repes < blocs.second; repes++) n_vect.push_back(blocs.first);
  }

  int len = int(n_vect.size());
  
  for (int i = 0; i < len/2; ++i) {
    blocs_to_put.push_back( n_vect[i] );
    blocs_to_put.push_back( n_vect[len-1-i] );
  }
  if (len % 2) blocs_to_put.push_back( n_vect[len/2] );
}

bool smallHere( Pair p , vector<int> front){
  int max_front = *max_element(front.cbegin(), front.cend());
  int min_front = *min_element(front.cbegin(), front.cend());
  return max_front >= min_front + min(p.first, p.second);
}

bool may_add_here(const vector<int>& front, int a, int i){
  for (int j = 0; j < a; j++){
    if (i+j >= W) return false;
    if (front[i] < front[i+j]) return false;
  }
  return true;
}

void greedy(char** argv){
  vector<int> front(W, 0); 
  int idx = 0;
  // Primera pasada colocando piezas grandes y pequeñas alternadamente
  for(Pair p : blocs_to_put){

    if (!(idx%2) || ( idx%2 && smallHere(p, front))) {
      bool been_put = false;

      int a = p.first; int b = p.second;

      vector<Pair> order(front.size());
      for (int i = 0; i < int(front.size()); ++i) order[i] = {i, front[i]};
      // Ordenar de más bajo a más alto
      sort(order.begin(), order.end(), compareBySecond);  

      for (Pair pos : order){ //Buscar de debajo a arriba
        int i = pos.first;

        if (!been_put && may_add_here(front, a, i)){ //Añadir la pieza
          disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});
          int pivot = front[i];
          for (int j=0; j<a; ++j) front[i+j] = pivot + b;
          been_put = true;
        }

      }
      
    }
    // Si se decide no añadir, se guarda para ponerla despues
    else n_res.push_back({p.first, p.second}); 

    ++idx;
  }

  // Añadimos las restantes

  for (Pair p: n_res){

    bool been_put = false;

    int a = p.first; int b = p.second;

    vector<Pair> order(front.size());
    for (int i = 0; i < int(front.size()); ++i) order[i] = {i, front[i]};
    // Ordenar de más bajo a más alto
    sort(order.begin(), order.end(), compareBySecond); 

    for (Pair pos : order){ //Buscar de debajo a arriba
      int i = pos.first;

      if (!been_put && may_add_here(front, a, i)){ //Añadir la pieza
        disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});
        int pivot = front[i];
        for (int j=0; j<a; ++j) front[i+j] = pivot + b;
        been_put = true;
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
  
  // Longitud a la que se ha llegado en cada columna
  createVectorOfMap();
  greedy(argv);

  // Mira el tiempo
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;

  write_ans(argv, elapsed_seconds);

  cout << "Ha tardat en executar el greedy: " << elapsed_seconds<< endl;
}