#include <iostream>
#include <vector>
#include <assert.h> 
#include <fstream> //Lectura de archivos
#include <map> //Equivalente a diciconarios de python
#include <chrono> //Cronometra el tiempo de ejecución
#include <algorithm>
using namespace std;

typedef pair<int, int> Pair; 
//Piezas ordenadas de grande a pequeño
struct AreaDescendiente {
    bool operator()(const Pair& a, const Pair& b) const {
        if (a.first*a.second != b.first*b.second) {
            return a.first*a.second > b.first*b.second; 
        }
        return a.first > b.first; // Si las areas son las mismas, se priorizan las piezas finas
    }
};
typedef map<Pair, int, AreaDescendiente>    Map; //Dimensiones y numero de repeticiones
typedef pair<Pair, Pair>     Coords; // Posición de una pieza
typedef vector<Coords>     VectCoords; //Conjunto de piezas posicionadas

// GLOBALS 
int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones -> numero de piezas

// Inicio de cronómetro
auto start = chrono::steady_clock::now();


// Dada una solución parcial del telar, busca recursivamente la mejor distribución 
// con todas las piezas añadidas y escribe la mejor solución
void exh_search(char** argv, vector<int> front, VectCoords& best_disp, VectCoords& disp, int& best_L, int L, Pair aguj, int k);

// Devuelve el tiempo transcurrido desde el inicio de la ejecución
double finish_time(){
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  return elapsed_seconds;
}

// Lee la entrada y asigna valor a las variables globales
int read_instance(char** file) {
  ifstream inp(file[1]);
  inp >> W >> N;
  int k = N;
  int ni, pi, qi;
  while (N > 0) {
    inp >> ni >> pi >> qi;
    N -= ni;
    n[{pi, qi}] = ni;
  }
  return k;
}

// Escribe el resultado en el archivo especificado en argv[2]
void write_ans(char** argv, double elapsed_seconds, VectCoords& best_disp, int best_L){
  // Escribe las soluciones encontradas por terminal y en el output file
  ofstream outp(argv[2]);
  outp << elapsed_seconds << endl << best_L << endl;
  for (Coords bloc : best_disp){
    outp << bloc.first.first << " " << bloc.first.second << " ";
    outp << bloc.second.first << " " << bloc.second.second << endl;
  }
}

// Devuelve true si la pieza es una de las dadas en el input, sino es el transpuesto
bool is_original(Pair piece){
  for (pair<Pair, int> original : n){
    if (piece == original.first) return true;
  }
  return false;
}

// Ordenación segun el segundo elemento de un Pair (la altura)
bool AscendingHeight(const Pair& a, const Pair& b) {
    return a.second < b.second;
}

// Devuelve true si en el agujero se puede añadir alguna de las piezas faltantes
bool big_hole(Pair aguj){
  for (pair<Pair, int> piece : n){
    if (piece.second > 0 && piece.first.first < aguj.first && piece.first.second < aguj.second) return true; 
  }
  return false;
}

// Devuelve true si, añadiendo las fiezas faltantes como líquidos, la solucion parcial
// no puede superar la mejor solución hasta el momento
bool cant_be_better(const vector<int>& front, int L, int best_L){
  // Calcula el area de las piezas faltantes
  int area_piezas = 0;
  for (pair<Pair, int> piece : n){
    for (int i = 0; i < piece.second; i++){
      area_piezas += piece.first.first*piece.first.second;
    }
  }
  // Calcula el area desde debajo de front hasta L
  int area_hasta_L = 0;
  for (int i = 0; i < W; i++){
    area_hasta_L += L - front[i];
  }
  return L + (area_piezas - area_hasta_L)/W >= best_L;
}

// Devuelve true si la rama se puede podar
bool poda(const vector<int>& front, int L, int best_L, Pair aguj){
  return L >= best_L || big_hole(aguj) || cant_be_better(front, L, best_L);
}

// Dado un telero, la anchura de una pieza y una posicion, devuelve si se puede añadir
bool can_add(const vector<int>& front, int a, int i){
  for (int j = 0; j < a; j++){
    if (i+j >= W) return false; // Si sobresale lateralmente
    if (front[i] < front[i+j]) return false; // Si solapa con otras piezas
  }
  return true;
}

// Dado un telero, una pieza y una posición, actualiza el telero para añadir la pieza
void update_teler(vector<int>& front, Pair piece, Pair& aguj, int i){
  int a = piece.first; int b = piece.second;
  int pivot = front[i]; // Posicion desde donde se añade la pieza

  //Calcula las dimensiones del agujero
  aguj.first += pivot - front[i];
  aguj.second += 1;
  front[i] = pivot + b;
  for (int j=1; j<a; ++j) {
    front[i+j]= pivot+b;
    aguj.second += 1;
    if (front[i+j] > front[i+j-1]) aguj.first = pivot - front[i+j];
  }
}

// Añade la pieza indicada en add_piece
void execute_addition(int a, int b, Pair orig_p, int i, char** argv, vector<int>& front, VectCoords& best_disp,
                VectCoords& disp, int& best_L, int k){
  n[orig_p] -=1;
  disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});

  vector<int> new_front = front; //Para no perder información al añadir una pieza
  Pair aguj = {0,0}; // Tamaño del agujero creado al añadir una pieza

  update_teler(new_front, {a,b}, aguj, i);
  exh_search(argv, new_front, best_disp, disp, best_L, *max_element(new_front.cbegin(), new_front.cend()), aguj, k-1);
  
  // Deshacer los cambios recursivos
  n[orig_p] +=1; 
  disp.pop_back();
}

// Dada una solución parcial y una posición, añade en dicha posición una de las piezas
// que faltan por añadir siempre que sea posible y llama a la función recursiva
void add_piece( char** argv, int i, vector<int>& front, VectCoords& best_disp,
                VectCoords& disp, int& best_L, int k){
  for(pair<Pair, int> piezas : n){
    if (piezas.second > 0){ // Si quedan piezas de este tamaño por añadir
      Pair p = piezas.first;

      // Se busca si la pieza fue rotada
      Pair orig_p = p; 
      if (!is_original(p)) orig_p = {p.second, p.first};

      int a = p.first; int b = p.second;
      if (can_add(front, a, i)){
        execute_addition(a, b, orig_p, i, argv, front, best_disp, disp, best_L, k);
      }

      // Misma pieza pero rotada
      if ( a != b && can_add(front, b, i)){
        execute_addition(b, a, orig_p, i, argv, front, best_disp, disp, best_L, k);
      }
    }
  }   
}

// Dada una solución parcial del telar, busca recursivamente la mejor distribución 
// con todas las piezas añadidas y escribe la mejor solución
void exh_search(char** argv, vector<int> front, VectCoords& best_disp, VectCoords& disp,
                 int& best_L, int L, Pair aguj, int k)
{
  if(!poda(front, L, best_L, aguj)){
    if(k==0){ //Si ha añadido todas las piezas 
      if(L < best_L){ 
        best_L = L;
        best_disp = disp;

        double elapsed_seconds = finish_time();

        write_ans(argv, elapsed_seconds, best_disp, best_L);
      }
    }
    else{ 
      vector<Pair> order(W); // Se buscan todas las posiciones de debajo a arriba
      for (int i = 0; i < int(W); ++i) order[i] = {i, front[i]};
      sort(order.begin(), order.end(), AscendingHeight);

      for (Pair pos : order){
        int i = pos.first;
        add_piece(argv, i, front, best_disp, disp, best_L, k);
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
  assert(argc == 3);

  int k = read_instance(argv); //k: Numero de piezas por añadir
  int best_L = INT_MAX;
  vector<int> front(W, 0); // Altura a la que se ha llegado en cada columna

  VectCoords best_disp = {}, disp = {}; //Posición de las piezas añadidas
  exh_search(argv, front, best_disp, disp, best_L, 0, {0,0}, k);
}