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

#ifndef PREFIX_SUM_PREFIX_SUM_HPP_
#define PREFIX_SUM_PREFIX_SUM_HPP_

#include <vector>
#include <stdint.h>
#include "PrefixSumNode.hpp"

namespace prefixsum{

/**
 * Dynamic Succinct Prefix Sum Data Structure
 * Store integer arrrays vs[0...num_-1] support
 *   prefixsum(k)    : return \sum_{i=0}^{k} vs[i]
 *   find(i)         : return i s.t. prefixum(k) <= i < prefixsum(k+1)
 *   insert(i, x)    : vs <- vs[0...i-1] x vs[i ... num_-1]
 *   set(i, x)       : vs[i] <- x
 */
class PrefixSum{
public:
  /**
   * Constructor
   */ 
  PrefixSum();
  
  /**
   * Destructor
   */ 
  ~PrefixSum();

  /**
   * Clear the internal state
   */
  void Clear();

  /**
   * Insert val between vs[ind-1] and vs[ind]
   */
  void Insert(int64_t ind, int64_t val);

  /**
   * Increment current value vs[ind] <- max(vs[ind] + val, 0)
   */
  void Add(int64_t ind, int64_t val);

  /**
   * Set vs[ind] <- val
   */
  void Set(int64_t ind, int64_t val);

  /**
   * Return vs[ind]
   */
  int64_t Get(int64_t ind) const;

  /**
   * Return vs[0] + vs[1] + ... + vs[ind-1]
   */
  int64_t GetPrefixSum(int64_t ind) const;

  /**
   * Return ind s.t. GetPrefixSum(ind) <= ind < GetPrefixSum(ind+1) if values are all positive
   */
  int64_t FindInPositiveValues(int64_t val) const;

  /**
   * Return the number of interger nums
   */
  int64_t Num() const{
    return num_;
  }

  /**
   * Return the sum of integers
   */
  int64_t Sum() const {
    return sum_;
  }

  int DepthSum() const{
    return DepthSumInternal(root_, 0);
  }

  int DepthMax() const{
    return DepthMaxInternal(root_, 0);
  }

private:
  PrefixSumNode* InsertInternal(PrefixSumNode* h, int64_t ind, int64_t val, int64_t leaf_val);

  // Left-Leaning Red-Black Tree
  bool IsRED(PrefixSumNode* h) const;

  void FlipColor(PrefixSumNode* h);
  PrefixSumNode* RotateLeft(PrefixSumNode* h);
  PrefixSumNode* RotateRight(PrefixSumNode* h);

  int DepthSumInternal(const PrefixSumNode* h, int depth) const{
    if (h == NULL) return depth;
    return 
      DepthSumInternal(h->left, depth+1)
      + DepthSumInternal(h->right, depth+1);
  }

  static int MyMax(int x, int y) {
    return (x > y) ? x : y;
  }

  int DepthMaxInternal(const PrefixSumNode* h, int depth) const{
    if (h == NULL) return depth;
    return MyMax(DepthMaxInternal(h->left, depth+1),
                 DepthMaxInternal(h->right, depth+1));
  }



  PrefixSumNode* root_;
  int64_t num_;
  int64_t sum_;
};


} // namespace prefixsum

#endif // PREFIX_SUM_PREFIX_SUM_HPP_
