#include <iostream>
#include <vector>
#include <assert.h>
using namespace std;

int main(int argc, char** argv) {

  // Write help message.
  if (argc == 1) {
    cout << "Makes a sanity check of a solution" << endl;
    cout << "Usage: " << argv[0] << " INPUT_FILE OUTPUT_FILE" << endl;
    exit(0);
  }

  assert(argc == 3);
  cout << argv[1] << " " << argv[2];  
}