
#include <iostream>
#include <vector>
#include <assert.h> 
#include <fstream> //Lectura de archivos
#include <map> //Equivalente a diciconarios de python
#include <chrono> //Cronometra el tiempo de ejecución
#include <algorithm>
#include <cstdlib>
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
typedef vector<Pair>      Elem; // Elemento del grupo de permutaciones
// first : indice de posición (permutacion) ;  second: si gira o no
typedef vector<Elem>    Popula; // Población de soluciones 

// GLOBALS 
int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones + numero de piezas
int best_L=999999; 
int L = best_L ; //Longitud ans parcial
VectCoords disp = {}; //Disposicion de ans parcial/total
vector< Pair > n_orig = {}; // Configuración identidad


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

void write_ans(char** argv){
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;

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
    return a.second < b.second; // Compare based on the second element
}

// <<< /!\ <<< /!\ <<< /!\ <<< /!\ Realmente importante ???
Elem operate(Elem A, Elem B){
  //Hacer actuar A sobre B
  int N = int(A.size());
  assert( N == int(B.size()) ) ;
  
  Elem out(N, {0,0});

  for (int i=0; i<N; ++i){
    out[i].first = B[A[i].first].first ; 
    out[i].second = (A[i].second + B[A[i].first].second)%2 ;
  }
  return out;
}

vector<Pair> act(Elem A){
  vector<Pair> out(int(A.size()));
  for (int i=0; i<int(A.size()); ++i){
    out[i] = n_orig[A[i].first];
    if (A[i].second && out[i].second <= W) out[i] = {out[i].second, out[i].first};
  }
  return out;
}

bool is_in(int a, vector<int> v){
  for (int e : v) if(a==e) return true;
  return false;
}

int fitness(char** argv, Elem action){
  // Basada en la implementación del greedy, 
  // dada una ordenación de las piezas las coloca
  // para obtener una L.
  vector<int> front(W, 0);
  vector<Pair> n_list = act(action) ;
  disp = {};

  // cout << "testing ";
  // for( Pair p: action) cout << p.first << p.second << " ";
  // cout << endl;

  for(Pair p: n_list){
      bool been_put = false;
      int delta = 0; // incremento desde el front a colocar la pieza

      int a = p.first; int b = p.second;

      vector<Pair> order(front.size());
      for (int i = 0; i < int(front.size()); ++i) order[i] = {i, front[i]};
      // Ordenar de más bajo a más alto
      sort(order.begin(), order.end(), compareBySecond);  

      while (!been_put) {

        for (Pair pos : order){ // Buscar de debajo a arriba
          int i = pos.first;
          bool may_add_here = true;
          int j = 0;
          while (j <a && may_add_here){ // Si se puede añadir aquí
            may_add_here = may_add_here && (front[i]+delta >= front[i+j]) && i <= W-a;
            ++j;
          }

          if (!been_put && may_add_here){ //Añadir la pieza
            disp.push_back({{i, front[i]+delta},{i+a-1, front[i]+b-1+delta}});
            vector<int> new_front = front;
            for (int j=0; j<a; ++j) new_front[i+j]= front[i]+b+delta;
            front = new_front;
            been_put = true;
          }
        }
        ++delta;
        // write_ans(argv);
      }
    // cout << a << b << " " << been_put <<endl ;
  }
  
  L = *max_element(front.cbegin(), front.cend());
  if (L < best_L){  best_L = L;  write_ans(argv);
  }

  return L;
}

Popula selectParents(char** argv, Popula P, int numSelected ){
  vector<Pair> order(P.size());
  vector<Elem> out = {};
  int n = int(P.size());
  int idx = 0;

  for (int i = 0; i < n; ++i) order[i] = {i, fitness(argv, P[i])};
  sort(order.begin(), order.end(), compareBySecond);

  while (int(out.size()) < min(numSelected, n) ){
    if ( rand()%(2*idx+3) == 0) out.push_back( P[order[idx%n].first] );
    ++idx;
  }
  return out;
}

Popula recombine(Popula P){
  int L = int(P[0].size());
  vector<Elem> out = {};
  int r1, r2, rb, mi, ma, idx;
  Elem O1(L), O2(L);

  for (int i = 0; i < int(P.size()); ++i){
    for (int j = i+1; j < int(P.size()); ++j){
      
      r1 = rand() % L ; r2 = rand() % L ; rb = rand() % L ; 
      mi = min(r1,r2) ; ma = max(r1,r2);
      vector<int> frag1(ma-mi), frag2(ma-mi);
      vector<int> comp1 = {}, comp2 = {};
      vector<int> bitvec1(L), bitvec2(L);

      // cout << i <<j << "index" << r1<<r2<<rb<<"rand"<<endl;

      for (int k = mi; k < ma; ++k) {
        frag1[k-mi] = P[i][k].first;
        frag2[k-mi] = P[j][k].first;
      }
      for (int k = 0; k < L; ++k) {
        if ( !is_in(P[i][k].first, frag2) ) comp1.push_back(P[i][k].first);
        if ( !is_in(P[j][k].first, frag1) ) comp2.push_back(P[j][k].first);
      }
      for (int k = 0; k < L; ++k) {
        if (k < rb){
          bitvec1[k] = P[i][k].second;
          bitvec2[k] = P[j][k].second;
        }
        else{
          bitvec2[k] = P[i][k].second;
          bitvec1[k] = P[j][k].second;
        }
      }

      idx = 0;
      for (int k = 0; k < L; ++k){
        if (mi<=k && k<ma) {
          O1[k] = {frag1[k-mi],bitvec1[k]};
          O2[k] = {frag2[k-mi],bitvec2[k]};
        }
        else{
          O1[k] = {comp2[idx],bitvec1[k]};
          O2[k] = {comp1[idx],bitvec2[k]};
          ++idx;
        }
      }
      // for (int m=0; m<L; ++m) cout << O1[m].first << O1[m].second << "a" << O2[m].first << O2[m].second <<" ";
      // cout << endl;
      out.push_back(O1); out.push_back(O2);
    }
  }
  return out;
}

Popula mutate(Popula P, int prob){
  Popula out(int(P.size()));
  int L = int(P[0].size());
  int r1, r2;
  Pair hold;

  for (int i=0; i<int(P.size()); ++i){
    Elem e(L);
    for (int k=0; k<L; ++k){
      if (rand()%prob == 0) e[k] = {P[i][k].first, (1+P[i][k].second)%2};
      else e[k] = P[i][k];
    }
    for (int j=0; j<50; ++j){
      if (rand()%prob == 0){
        r1 = rand()%L; r2 = rand()%L;
        while (r1 == r2) r2 = rand()%L;
        hold = e[r1]; 
        e[r1] = e[r2]; 
        e[r2] = hold; 
      }
    }

    out[i] = e;
  }
  return out;
}

void metah(char** argv){
  for(pair<Pair, int> blocs : n) {
    for (int repes = 0; repes < blocs.second; repes++) n_orig.push_back(blocs.first);
  }
  sort(n_orig.begin(), n_orig.end(), compareBySecond); 

  Elem Ident(int(n_orig.size()));
  for (int i=0; i<int(n_orig.size()); ++i){
    Ident[i] = {i, 0};
  }

  Elem Rever = Ident;
  reverse(Rever.begin(), Rever.end());

  Popula Pop = { Ident, Rever };
  Pop.push_back(mutate({Ident}, 2)[0]);
  Pop.push_back(mutate({Rever}, 2)[0]);


  while (1){
    Pop = recombine(Pop);
    Pop = mutate(Pop, 20);
    Pop = selectParents(argv, Pop, 25);
  }

  // Ident = {{0,0}, {1,0}, {2,1}, {3,1}, {4,0}, {5,1}, {6,1}, {7,1}, {8,1}, {9,1},
  //  {10,0}, {11,0}, {12,0}, {13,0}, {14,0}, {15,0}, {16,0}, {17,1}, {18,0}, {19,1},
  //  {20,0}, {21,1}, {22,0}, {23,1}, {24,0}, {25,0}, {26,0}, {27,0}, {28,0}};

  // cout << fitness(argv, Ident) <<endl;

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

  // Finalización de la busqueda
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  cout << "Ha tardat en trobar totes les combinacions: " << elapsed_seconds<< endl;
}