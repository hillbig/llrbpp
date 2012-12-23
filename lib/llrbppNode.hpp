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

#ifndef LLRBPP_NODE_HPP_
#define LLRBPP_NODE_HPP_

#include <stdio.h> // NULL

namespace llrbpp{

const static bool kRED = true;
const static bool kBLACK = false;

template <class K, class V>
struct Node{
  Node(K key, V val) : 
    key(key), val(val), left(NULL), right(NULL), color(kRED) {}

  ~Node(){
    delete left;
    delete right;
  }

  K key;
  V val;
  Node* left;
  Node* right;
  bool color;
};

}

#endif // LLRBPP_NODE_HPP_
