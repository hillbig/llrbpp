#include <iostream>
#include <cmath>
#include <queue>
#include <stdlib.h>
#include "../lib/llrbpp.hpp"
#include "../lib/PrefixSum.hpp"

#include <sys/time.h>
double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double)tv.tv_usec*1e-6;
}


using namespace std;

int main(int argc, char* argv[]){
  llrbpp::LLRBPP<int, int> fid;
  int N = 1000000;
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
  
  cout << fid.Num() << " " << log(fid.Num()) / log(2.f) << " " << (float) fid.DepthSum() / fid.Num() << " " << fid.DepthMax() << std::endl;

  double begin_time = gettimeofday_sec();
  prefixsum::PrefixSum ps;
  for (int i = 0; i < N; ++i){
    int ind = rand() % (i+1);
    int val = rand();
    ps.Insert(ind, val);
  }
  cout << ps.Num() << " " << log(ps.Num()) / log(2.f) << " " << (float) ps.DepthSum() / ps.Num() << " " << ps.DepthMax() << std::endl;
  cout << gettimeofday_sec() - begin_time << endl;
  
  return 0;
}
