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
            return a.first > b.first; // Compare first elements
        }
        return a.second < b.second; // If first elements are equal, compare second elements
    }
};
typedef map<Pair, int, CompareByFirst>    Map; //Piezas ordenadas de ancho a fino
typedef pair<Pair, Pair>     Coords; // Posición de una pieza en la solución
typedef vector<Coords>     VectCoords; //Conjunto de piezas posicionadas

// GLOBALS 
int W, N; //Anchura del telar y numero de comandas
Map n; //Dimensiones -> numero de piezas
vector<int> areas = {};

// Inicio de cronómetro
auto start = chrono::steady_clock::now();

// Declaración de funciones
void exh_search(char** argv, vector<int> front, VectCoords& best_disp, VectCoords& disp, int& best_L, int L, Pair f, int k);

double finish_time(){
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  return elapsed_seconds;
}

// Lee la entrada y asigna valor a las variables básicas
void read_instance(char** file, int& k) {
  ifstream inp(file[1]);
  inp >> W >> N;
  k = N; // Comptador del nivell recursiu
  int ni, pi, qi;
  while (N > 0) {
    inp >> ni >> pi >> qi;
    // cout <<"entrada "<< ni << " " << pi<< " " << qi<<endl;
    N -= ni;
    n[{pi, qi}] = ni;
    // cout << n[{pi, qi}]<< " " <<ni<<endl;
  }
  // std::cout << "W: "<<W<<" N: "<<k<<endl;
  // for (const auto& p : n){ cout <<n[{p.first.first, p.first.second}]<<" "<< p.first.first << " " <<p.first.second<<endl;}

}

// Escribe el resultado sobre un archivo
void write_ans(char** argv, double elapsed_seconds, VectCoords& best_disp, int best_L){
  // Escribe las soluciones encontradas por terminal y en el output file
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

// Retorna true si la pieza es una de las dadas en el inp, sino es el transpuesto
bool is_original(Pair piece){
  for (pair<Pair, int> original : n){
    if (piece == original.first) return true;
  }
  return false;
}

// Ordenación segun el segundo elemento de un Pair (la altura)
bool compareBySecond(const Pair& a, const Pair& b) {
    return a.second < b.second; // Compare based on the second element
}

// Retorna true si el numero de agujeros es mayor que 2 veces el area de una pieza
bool forat_gran(Pair f){
  for (pair<Pair, int> piece : n){
    if (piece.second > 0 && piece.first.first < f.first && piece.first.second < f.second) return true; 
  }
  return false;
}

// Devuelve true si la rama se puede podar
bool poda(int L, int best_L, Pair f){
  // Problema: descarta solucines optimas
  bool poda1 = (L >= best_L);
  // bool poda2 = (f > min_f);
  bool poda3 = forat_gran(f);
  // cout << "f: " << f<<endl;
  // cout << "forat_gran: " << poda3<<endl;
  return poda1 || poda3 ;//|| poda2;// ;
  // return L > best_L || f > min_f || forat_gran(f);
}

// Dado un telero, la anchura de una pieza y una posicion, devuelve si se puede añadir
bool may_add_here(const vector<int>& front, int a, int i){
  for (int j = 0; j < a; j++){
    if (i+j >= W) return false;
    if (front[i] < front[i+j]) return false;
  }
  return true;
}

// Dado un telero, una pieza y una posición, actualiza el telero para añadir la pieza
void update_teler(vector<int>& front, Pair piece, Pair& f, int i){
  int a = piece.first; int b = piece.second;
  int pivot = front[i];
  //Calcula las dimensiones del agujero
  f.first += pivot - front[i];
  f.second += 1;
  front[i] = pivot + b;
  for (int j=1; j<a; ++j) {
    front[i+j]= pivot+b;
    f.second += 1;
    // cout<< i+j<<endl;
    if (front[i+j] > front[i+j-1]) f.first = pivot - front[i+j];
  }
}


void add_piece( char** argv, int i, vector<int>& front, VectCoords& best_disp,
                VectCoords& disp, int& best_L, int L, Pair f, int k){
  for(pair<Pair, int> piezas : n){
    if (piezas.second > 0){
      Pair p = piezas.first;

      Pair orig_p = p; 
      if (!is_original(p)) orig_p = {p.second, p.first};

      int a = p.first; int b = p.second;
      if (may_add_here(front, a, i)){ //Añadir la pieza
        // cout << "fixed " << i <<endl;
        n[orig_p] -=1;
        disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});

        vector<int> new_front = front;
        
        Pair new_f = {0,0};
        update_teler(new_front, p, new_f, i);
        exh_search(argv, new_front, best_disp, disp, best_L, *max_element(new_front.cbegin(), new_front.cend()), new_f, k-1);
        
        // Deshacer los cambios recursivos
        n[orig_p] +=1; 
        disp.pop_back();
      }

      // Caso reversed
      a = p.second; b = p.first;
      if ( a != b && may_add_here(front, a, i)){ //Añadir la pieza
        // cout << "fixed " << i <<endl;
        n[orig_p] -=1;
        disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});

        vector<int> new_front = front;
        Pair new_f = f;
        update_teler(new_front, {a,b}, new_f, i);
        
        exh_search(argv, new_front, best_disp, disp, best_L, *max_element(new_front.cbegin(), new_front.cend()), new_f, k-1);
        
        // Deshacer los cambios recursivos
        n[orig_p] +=1; 
        disp.pop_back();
      }
    }
  }   
}

// f: numero de forats
void exh_search(char** argv, vector<int> front, VectCoords& best_disp, VectCoords& disp,
                 int& best_L, int L, Pair f, int k)
{
  // cout << "f: " << f<<endl;
  // for (int col : front) cout << col <<" ";
  // cout<<endl;
  if(k==0){ //Si ha añadido todas las piezas 
    if(L < best_L){ 
      best_L = L;
      best_disp = disp;

      // Mira el tiempo
      double elapsed_seconds = finish_time();

      write_ans(argv, elapsed_seconds, best_disp, best_L);
    }
  }

  else{
    if(!poda(L, best_L, f)){
    if (L == best_L){
    double elapsed_seconds = finish_time();
    cout << "ha trobat un equivalent al segon "<<elapsed_seconds<<endl;
  }
      vector<Pair> order(front.size());
      for (int i = 0; i < int(front.size()); ++i) order[i] = {i, front[i]};
      sort(order.begin(), order.end(), compareBySecond);

      for (Pair pos : order){ //Buscar de arriba a abajo
        int i = pos.first;
        add_piece(argv, i, front, best_disp, disp, best_L, L, f, k);
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

  int k; // Numero de piezas por anadir
  assert(argc == 3);
  read_instance(argv, k);

  // Calcular una cota superior para realizar podas
  int best_L = 999999; // Por alguna razón cuando se usa la cota superior a veces no encuentra resultado
  // for(pair<Pair, int> bloc : n){
  //   best_L += bloc.second * min(bloc.first.first, bloc.first.second);
  // }
  cout << "best L: "<<best_L<<endl;

  // Altura a la que se ha llegado en cada columna
  vector<int> front(W, 0); 

  VectCoords best_disp ={}, disp = {};
  exh_search(argv, front, best_disp, disp, best_L, 0, {0,0}, k);

  // Finalización de la busqueda
  double elapsed_seconds = finish_time();
  cout << "Ha tardat en trobar totes les combinacions: " << elapsed_seconds<< endl;
}