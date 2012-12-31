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
#include <iostream>
#include <cassert>
#include "PrefixSumNode.hpp"
#include "PrefixSumLeaf.hpp"

namespace prefixsum{

/**
 * Dynamic Succinct Prefix Sum Data Structure
 * Store integer arrrays vs[0...num_-1] supporting
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
  void Insert(uint64_t ind, int64_t val);

  //void Delete(int64_t ind);

  /**
   * Increment current value vs[ind] <- max(vs[ind] + val, 0)
   */
  void Add(uint64_t ind, int64_t val);

  /**
   * Set vs[ind] <- val
   */
  void Set(uint64_t ind, int64_t val);

  /**
   * Return vs[ind]
   */
  int64_t Get(uint64_t ind) const;

  /**
   * Return vs[0] + vs[1] + ... + vs[ind-1]
   */
  int64_t GetPrefixSum(uint64_t ind) const;

  /**
   * Return ind s.t. GetPrefixSum(ind) <= ind < GetPrefixSum(ind+1) if values are all positive
   */
  uint64_t FindInPositiveValues(int64_t val) const;

  /**
   * Return the number of leaves
   */
  size_t Num() const {
    return leaves_.size();
  }

  /**
   * Return the sum of vals
   */
  int64_t ValSum() const{
    return val_sum_;
  }

  int DepthSum() const{
    return DepthSumInternal(root_ind_, 0);
  }

  int DepthMax() const{
    return DepthMaxInternal(root_ind_, 0);
  }

  void Print() const{
    PrintInternal(root_ind_, 0);
  }

  void CheckParent() const{
    CheckParentInternal(root_ind_, -1);
  }

private:
  void CheckParentInternal(int64_t node_ind, int64_t parent_ind) const{
    if (node_ind < 0){
      if (leaves_[ToLeafInd(node_ind)].parent != parent_ind){
        std::cerr << "parent = " << parent_ind << " leaf_ind=" << ToLeafInd(node_ind) << " node.parend=" << leaves_[ToLeafInd(node_ind)].parent << std::endl;
        assert(false);
      }
    } else {
      CheckParentInternal(nodes_[node_ind].left_ind, node_ind);
      CheckParentInternal(nodes_[node_ind].right_ind, node_ind);
    }
  }
  
  void PrintInternal(int64_t ind, int64_t depth) const{
    for (int i = 0; i < depth; ++i){
      std::cout << " ";
    }
    if (ind < 0) {
      std::cout << leaves_[ToLeafInd(ind)].parent << "#" << ToLeafInd(ind) << ":" << leaves_[ToLeafInd(ind)].val << std::endl;
    } else {
      std::cout << ind << ":" << nodes_[ind].weight << ":" << nodes_[ind].sum << std::endl;
      PrintInternal(nodes_[ind].left_ind, depth+1);
      PrintInternal(nodes_[ind].right_ind, depth+1);
    }
  }

  static int64_t ToLeafInd(int64_t ind){
    return - ind - 1;
  }

  int64_t InsertInternal(int64_t node_ind, uint64_t ind, int64_t val);
  //int64_t DeleteInternal(int64_t node_ind, int64_t ind);
  //int64_t MoveREDLeft(int64_t node_ind);
  //int64_t MoveREDRight(int64_t node_ind);
  //int64_t FixUp(int64_t node_ind);

  // Left-Leaning Red-Black Tree
  bool IsRED(int64_t node_ind) const;
  void FlipColor(int64_t node_ind);
  int64_t RotateLeft(int64_t node_ind);
  int64_t RotateRight(int64_t node_ind);

  uint64_t GetLeftWeight(int64_t node_ind) const{
    if (node_ind < 0) return 0;
    int64_t left_ind = nodes_[node_ind].left_ind;
    if (left_ind < 0) return 1;
    return nodes_[left_ind].weight;
  }

  int64_t GetLeftVal(int64_t node_ind) const{
    if (node_ind < 0) return 0;
    int64_t left_ind = nodes_[node_ind].left_ind;
    if (left_ind < 0) return leaves_[-left_ind-1].val;
    return nodes_[left_ind].sum;
  }

  uint64_t GetRightWeight(int64_t node_ind) const{
    if (node_ind < 0) return 0;
    int64_t right_ind = nodes_[node_ind].right_ind;
    if (right_ind < 0) return 1;
    return nodes_[right_ind].weight;
  }

  int64_t GetRightVal(int64_t node_ind) const{
    if (node_ind < 0) return 0;
    int64_t right_ind = nodes_[node_ind].right_ind;
    if (right_ind < 0) return leaves_[-right_ind-1].val;
    return nodes_[right_ind].sum;
  }

  int DepthSumInternal(const int64_t ind, int depth) const{
    if (ind < 0) return depth;
    return 
      DepthSumInternal(nodes_[ind].left_ind, depth+1)
      + DepthSumInternal(nodes_[ind].right_ind, depth+1);
  }

  static int MyMax(int x, int y) {
    return (x > y) ? x : y;
  }

  int DepthMaxInternal(const int64_t ind, int depth) const{
    if (ind < 0) return depth;
    return MyMax(DepthMaxInternal(nodes_[ind].left_ind, depth+1),
                 DepthMaxInternal(nodes_[ind].right_ind, depth+1));
  }

  std::vector<PrefixSumNode> nodes_;
  std::vector<PrefixSumLeaf> leaves_;
  int64_t val_sum_;
  int64_t root_ind_;
};


} // namespace prefixsum

#endif // PREFIX_SUM_PREFIX_SUM_HPP_
