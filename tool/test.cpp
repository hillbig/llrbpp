#include <iostream>
#include <cmath>
#include <queue>
#include <stdlib.h>
#include "../lib/llrbpp.hpp"

using namespace std;

int main(int argc, char* argv[]){
  llrbpp::LLRBPP<int, int> fid;
  int N = 100000;
  queue<int> delete_keys;
  for (int i = 0; i < N; ++i){
    int key = rand();
    int val = rand();
    fid.Insert(key, val);
    if (rand() % 2 == 0){
      delete_keys.push(key);
    }
    if (rand() % 4 == 0 && !delete_keys.empty()){
      int delete_key = delete_keys.front();
      delete_keys.pop();
      fid.Delete(delete_key);
    }
  }
  
  cout << log(fid.Num()) / log(2.f) << " " << (float) fid.DepthSum() / fid.Num() << " " << fid.DepthMax() << std::endl;
  
  return 0;
}
