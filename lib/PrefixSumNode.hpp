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

#include <limits.h>

namespace prefixsum {

const static bool kRED = true;
const static bool kBLACK = false;
const static int64_t kNULL = -INT_MAX;

struct PrefixSumNode{
  PrefixSumNode(uint64_t weight, int64_t sum) : 
    weight(weight), sum(sum), left_ind(kNULL), right_ind(kNULL), color(kRED) {}
  ~PrefixSumNode(){
  }

  uint64_t weight;
  int64_t sum;
  int64_t left_ind;
  int64_t right_ind;
  bool color;
};

} // namespace prefixsum

#endif // PREFIXSUM_PREFIXSUM_NODE_HPP_
