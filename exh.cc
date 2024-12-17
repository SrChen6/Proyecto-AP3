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

// Declaración de funciones
void exh_search(char** argv, vector<int> front, VectCoords& best_disp, VectCoords disp, int& best_L, int L, int k);

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

// void undo_change(vector<int>& front, VectCoords& disp){
//   Coords pos_piece = disp.back();
//   disp.pop_back();
//   Pair ul = pos_piece.first;
//   Pair dr = pos_piece.second;
//   Pair piece = {dr.first-ul.first+1, dr.second-ul.second+1};
//   for(int j = ul.first ; j <= dr.first; j++){
//     front[j] -= piece.second;
//   }
//   if (is_original(piece)){ //Por si la pieza fue rotada
//     n[piece] += 1;
//   }
//   else{
//     Pair orig_piece = {piece.second, piece.first};
//     n[orig_piece] += 1;
//   }
// }

void add_piece( char** argv, Pair p, vector<int>& front, VectCoords& best_disp,
                VectCoords& disp, int& best_L, int L, int k){
  //Si la pieza esta rotada, consultar las dimensiones originales en n
  Pair orig_p = p; 
  if (!is_original(p)) orig_p = {p.second, p.first};
    int a = p.first;
    int b = p.second;
    bool may_add_here = true;

    // En vez de buscar de izquierda a derecha, buscar de arriba a abajo
    vector<Pair> order(front.size());
    for (int i = 0; i < int(front.size()); ++i) order[i] = {i, front[i]};
    // Ordenar de más bajo a más alto
    sort(order.begin(), order.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  return a.second < b.second; // Compare by value
              });
    // for (int i=0; i<=W-a; ++i){ //Mirar cada casilla posible
    for (Pair pos : order){ //Mirar cada casilla posible
      int i = pos.first;
      may_add_here = true;
      int j = 0;
      while (j <a && may_add_here){ // Si se puede añadir aquí
        may_add_here = may_add_here && (front[i] >= front[i+j]) && i <= W-a;
        ++j;
      }

      if (may_add_here && front[i]+b < best_L){ //Añadir la pieza
        n[orig_p] -=1;
        disp.push_back({{i, front[i]},{i+a-1, front[i]+b-1}});
        vector<int> new_front = front;
        for (int j=0; j<a; ++j) new_front[i+j]= front[i]+b;
        
        // cout << "esta por llamar un nuevo nivel recursivo"<<endl;
        exh_search(argv, new_front, best_disp, disp, best_L, *max_element(new_front.cbegin(), new_front.cend()), k-1);
        
        // Deshacer los cambios recursivos
        n[orig_p] +=1; 
        disp.pop_back();
      }
    }
}

void exh_search(char** argv, vector<int> front, VectCoords& best_disp, VectCoords disp,
                 int& best_L, int L, int k)
{
  // cout << "front"<<endl;
  // for (int col : front) cout << col <<" ";
  // cout<<endl;
  if(k==0){ //Si ha añadido todas las piezas 
    // cout <<"Ha puesto todas las piezas"<<endl;
    if(L < best_L){
      // cout << "Ha encontrado una solución mejor" << endl;
      best_L = L;
      best_disp = disp;

      // Mira el tiempo
      auto end = chrono::steady_clock::now();
      auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
      double elapsed_seconds = elapsed.count() / 1000.0;

      write_ans(argv, elapsed_seconds, best_disp, best_L);
    }
  }
  else{
    if(L < best_L){ //poda
      for(pair<Pair, int> blocs : n){
        if(blocs.second > 0){
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
}

int main(int argc, char** argv) {
  ios_base::sync_with_stdio(false);

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
  for(pair<Pair, int> bloc : n){
    best_L += bloc.second * min(bloc.first.first, bloc.first.second);
  }
  cout << "best L: "<<best_L<<endl;

  // Altura a la que se ha llegado en cada columna
  vector<int> front(W, 0); 

  VectCoords best_disp ={}, disp = {};
  exh_search(argv, front, best_disp, disp, best_L, 0, k);

  // Finalización de la busqueda
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
  double elapsed_seconds = elapsed.count() / 1000.0;
  cout << "Ha tardat en trobar totes les combinacions: " << elapsed_seconds<< endl;
}