
#include <iostream>
#include <vector>
#include <assert.h> 
#include <fstream> //Lectura de archivos
#include <map> //Equivalente a diciconarios de python
#include <chrono> //Cronometra el tiempo de ejecución
#include <algorithm>
#include <cstdlib>
using namespace std;



typedef pair<int, int> Pair; //Tuplas
struct CompareByFirst {
    bool operator()(const Pair& a, const Pair& b) const {
        if (a.first != b.first) {
            return a.first > b.first; // Comparar por primer elemento
        }
        return a.second < b.second; // Si el primero es igual, ordenar por el segundo
    }
};

typedef map<Pair, int, CompareByFirst>    Map; //Diccionarios
typedef pair<Pair, Pair>     Coords; // Posición de una pieza en la solución
typedef vector<Coords>     VectCoords; //Conjunto de piezas posicionadas
typedef vector<Pair>      Elem; // Elemento del grupo de permutaciones
// Se implementa un Algoritmo Genético donde cada individuo se representa
//    mediante una acción de permutar y girar las piezas a colocar.
// first : indice de posición (permutacion) ;  second: si gira o no (0-1)
typedef vector<Elem>    Popula; // Población de soluciones 


// GLOBALES 
int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones + numero de piezas
int best_L=999999; // Mejor longitud encontrada hasta el momento
int L = best_L ; //Longitud solución parcial
VectCoords disp = {}; //Disposicion de ans parcial/total
vector< Pair > n_orig = {}; // Configuración identidad


// Inicio de cronómetro
auto start = chrono::steady_clock::now();


// Lee la entrada y asigna valor a las variables globales
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


// Devuelve el tiempo transcurrido desde el inicio de la ejecución
double finish_time(){
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  return elapsed_seconds;
}


// Escribe el resultado en el archivo especificado en argv[2]
void write_ans(char** argv){
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;

  ofstream outp(argv[2]);
  outp << finish_time() << endl << L << endl;
  for (Coords bloc : disp){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
  }
}


bool compareBySecond(const pair<int, int>& a, const pair<int, int>& b) {
    return a.second < b.second; // Comparar según el segundo elemento
}


// Hacer actuar el elemento sobre la lista la configuración inicial
//  para obtener la ordenación correspondiente
vector<Pair> act(Elem A){
  vector<Pair> out(int(A.size()));
  for (int i=0; i<int(A.size()); ++i){
    out[i] = n_orig[A[i].first]; // first representa la permutación
    if (A[i].second && out[i].second <= W) out[i] = {out[i].second, out[i].first};
    // second indica si se rota la pieza de tal indice
  }
  return out;
}


// Metodo para comprobar si integer pertenece a un vector.
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

  for(Pair p: n_list){
      bool been_put = false;
      int delta = 0; // incremento desde el front a colocar la pieza

      // Dimensiones de la pieza
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
        ++delta; // En caso de no poder colocar la pieza, se incrementa la altura.
      }
  }
  
  L = *max_element(front.cbegin(), front.cend());
  if (L < best_L) {best_L = L;  write_ans(argv);}

  return L;
}


// Seleccionar individuos según fitness y posteriormente seleccionar progenitores
Popula selection(char** argv, Popula P, int numIndv, int numParent ){
  vector<Pair> order(P.size());
  vector<Elem> out = {};
  vector<Elem> indiv = {};
  int idx = 0;

  // Ordenar individuos 
  for (int i = 0; i < int(P.size()); ++i) order[i] = {i, fitness(argv, P[i])};
  sort(order.begin(), order.end(), compareBySecond);

  // Seleccionar aquellos con mejor fitness
  for (int i = 0; i<min(int(P.size()), numIndv); ++i) indiv.push_back(P[order[i].first]);

  // Mediante una progresión descendiente como una harmonica, se seleccionan 
  //  probabilisticamente los progenitores 
  while ( int(out.size()) < min(numParent, int(indiv.size())) ){
    if ( rand()%(2*idx+1) == 0) out.push_back( indiv[idx%int(indiv.size())] );
    ++idx;
  }

  return out;
}


// Mediante un algoritmo de cross-over extender la población
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
      // Los fragmentos de las permutaciones que no se alteraran 
      vector<int> comp1 = {}, comp2 = {};
      // Los complementos de estos fragmentos, que con la reordenación 
      //  resultante tras eliminar los números pertenecientes a los fragmentos.
      vector<int> bitvec1(L), bitvec2(L);

      for (int k = mi; k < ma; ++k) {
        frag1[k-mi] = P[i][k].first;
        frag2[k-mi] = P[j][k].first;
      }
      for (int k = 0; k < L; ++k) {
        if ( !is_in(P[i][k].first, frag2) ) comp1.push_back(P[i][k].first);
        if ( !is_in(P[j][k].first, frag1) ) comp2.push_back(P[j][k].first);
      }
      // Para el array de bits se cruzan en un punto
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

      // Se reconstruye los individuos combinados
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

      out.push_back(O1); out.push_back(O2);
    }
  }
  return out;
}


// Mutar las soluciones intercambiando elementos de la permutación
//  e inviertiendo bits
Popula mutate(Popula P, int prob){
  // int prob representa el inverso de la probabilidad de mutación
  //  i.e. 1 de cada "prob" individuos muta
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
        while (r1 == r2) r2 = rand()%L; // Asegurarse de que sean diferentes
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
  // Generar la primera configuración, ordenando las piezas de grande a pequeña
  for(pair<Pair, int> blocs : n) {
    for (int repes = 0; repes < blocs.second; repes++) n_orig.push_back(blocs.first);
  }
  sort(n_orig.begin(), n_orig.end(), compareBySecond); 

  // Generar el elemento identidad del grupo
  Elem Ident(int(n_orig.size()));
  for (int i=0; i<int(n_orig.size()); ++i){
    Ident[i] = {i, 0};
  }

  Elem Rever = Ident;
  reverse(Rever.begin(), Rever.end());

  // Inicializar la población con la Identidad (cual es la configuración 
  //  utilizada en el greedy), y su Reverso (que será una mala solución pero 
  //  inducirá variabilidad genética), y sus mutaciones con alta probabilidad.
  Popula Pop = { Ident, Rever };
  Pop.push_back(mutate({Ident}, 2)[0]);
  Pop.push_back(mutate({Rever}, 2)[0]);

  while (1){
    Pop = recombine(Pop);
    Pop = mutate(Pop, 50);
    Pop = selection(argv, Pop, 100, 20);
  }
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

}