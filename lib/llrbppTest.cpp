/* 
 *  Copyright (c) 2012 Daisuke Okanohara
  * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#include <gtest/gtest.h>
#include <string>
#include <queue>
#include <map>
#include "llrbpp.hpp"

using namespace std;

TEST(llrbpp, trivial){
  llrbpp::LLRBPP<string, int> fid;
  EXPECT_EQ(0, fid.Num());
  fid.Insert("eee", 5);
  fid.Insert("aaa", 3);
  fid.Insert("bbb", 4);
  fid.Insert("ccc", 2);

  EXPECT_EQ(4, fid.Num());
  EXPECT_EQ(5, fid.Search("eee"));
  EXPECT_THROW(fid.Search("ddd"), llrbpp::NotFound);

  fid.Delete("eee");
  EXPECT_THROW(fid.Search("eee"), llrbpp::NotFound);

  fid.Clear();
  EXPECT_EQ(0, fid.Num());
  EXPECT_THROW(fid.Search("eee"), llrbpp::NotFound);
  fid.Clear();
  EXPECT_EQ(0, fid.Num());
}

TEST(llrbpp, small){
  llrbpp::LLRBPP<int, int> fid;
  map<int, int> m;
  int N = 10;
  queue<int> delete_keys;
  for (int i = 0; i < N; ++i){
    int key = rand() % 10 ;
    int val = rand() % 100;
    fid.Insert(key, val);
    m[key] = val;
    if (rand() % 2 == 0){
      delete_keys.push(key);
    }
    if (rand() % 4 == 0 && !delete_keys.empty()){
      int delete_key = delete_keys.front();
      delete_keys.pop();
      fid.Delete(delete_key);
      m.erase(delete_key);
    }
  }
  ASSERT_EQ(m.size(), fid.Num());
  try {
    for (map<int, int>::const_iterator it = m.begin(); it != m.end(); ++it){
      EXPECT_EQ(it->second, fid.Search(it->first));
    }
  } catch (...){
    FAIL();
  }
}  



