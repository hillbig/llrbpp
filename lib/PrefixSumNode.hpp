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

#ifndef PREFIXSUM_PREFIXSUM_NODE_HPP_
#define PREFIXSUM_PREFIXSUM_NODE_HPP_

#include <stdio.h> // NULL

namespace prefixsum {

const static bool kRED = true;
const static bool kBLACK = false;

struct PrefixSumNode{
  PrefixSumNode(int64_t weight, int64_t sum) : 
    weight(weight), sum(sum), left(NULL), right(NULL), color(kRED) {}
  ~PrefixSumNode(){
    delete left;
    delete right;
  }

  int64_t GetLeftWeight() const{
    if (left == NULL) return 1;
    return left->weight;
  }

  int64_t GetLeftVal() const{
    if (left == NULL) return left_val;
    return left->sum;
  }

  int64_t GetRightWeight() const{
    if (right == NULL) return 1;
    return right->weight;
  }

  int64_t GetRightVal() const{
    if (right == NULL) return right_val;
    return right->sum;
  }


  int64_t weight;
  int64_t sum;
  PrefixSumNode* left;
  PrefixSumNode* right;
  int64_t left_val;
  int64_t right_val;
  bool color;
};

} // namespace prefixsum

#endif // PREFIXSUM_PREFIXSUM_NODE_HPP_
