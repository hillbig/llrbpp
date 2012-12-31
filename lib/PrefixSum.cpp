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

#include <stdexcept>
#include <cassert>
#include "PrefixSum.hpp"

namespace prefixsum{

PrefixSum::PrefixSum() : val_sum_(0), root_ind_(0LL){
}

PrefixSum::~PrefixSum(){
}

void PrefixSum::Clear(){
  nodes_.clear();
  leaves_.clear();
  val_sum_ = 0;
  root_ind_ = 0;
}

void PrefixSum::Insert(uint64_t ind, int64_t val){
  if (ind > leaves_.size()){
    throw std::out_of_range("PrefixSum::InsertInternal out of range");
  }
  if (leaves_.size() == 0){
    // ind == 0
    leaves_.push_back(PrefixSumLeaf(-1, val));
    root_ind_ = -1; // = leaves_[0]
  } else {
    root_ind_ = InsertInternal(root_ind_, ind, val);
    nodes_[root_ind_].color = kBLACK;
  }
  val_sum_ += val;
}

void PrefixSum::Add(uint64_t ind, int64_t val){
  if (ind >= Num()){
    throw std::out_of_range("PrefixSum::Add out of range");  
  }
  val_sum_ += val;
  if (Num() == 1){
    leaves_[0].val += val;
    return;
  }
  int64_t node_ind = root_ind_;
  for (;;){
    PrefixSumNode& node = nodes_[node_ind];
    node.sum += val;
    uint64_t left_weight = GetLeftWeight(node_ind);
    if (ind < left_weight){
      if (node.left_ind < 0){
        leaves_[ToLeafInd(node.left_ind)].val += val;
        return;
      }
      node_ind = node.left_ind;
    } else {
      ind -= left_weight;
      if (node.right_ind < 0){
        leaves_[ToLeafInd(node.right_ind)].val += val;
        return;
      }
      node_ind = node.right_ind;
    }
  }
}

void PrefixSum::Set(uint64_t ind, int64_t val){
  if (ind >= Num()){
    throw std::out_of_range("PrefixSum::Set out of range");  
  }
  int64_t cur_val = Get(ind);
  Add(ind, val - cur_val);
}

int64_t PrefixSum::Get(uint64_t ind) const{
  if (ind >= Num()){
    throw std::out_of_range("PrefixSum::Add out of range");  
  }
  if (Num() == 1){
    return leaves_[0].val;
  }
  int64_t node_ind = root_ind_;
  for (;;){
    const PrefixSumNode& node = nodes_[node_ind];
    uint64_t left_weight = GetLeftWeight(node_ind);
    if (ind < left_weight){
      if (node.left_ind < 0){
        return leaves_[ToLeafInd(node.left_ind)].val;
      }
      node_ind = node.left_ind;
    } else {
      if (node.right_ind < 0){
        return leaves_[ToLeafInd(node.right_ind)].val;
      }
      ind -= left_weight;
      node_ind = node.right_ind;
    }
  }
}

int64_t PrefixSum::GetPrefixSum(uint64_t ind) const{
  if (ind > Num()){
    throw std::out_of_range("PrefixSum::Add out of range");  
  }
  if (ind == Num()){
    return val_sum_;
  }
  if (Num() == 1){
    return 0; // ind == 0
  }
  int64_t node_ind = root_ind_;
  int64_t sum = 0;
  while (node_ind >= 0){
    const PrefixSumNode& node = nodes_[node_ind];
    uint64_t left_weight = GetLeftWeight(node_ind);
    if (ind < left_weight){
      node_ind = node.left_ind;
    } else {
      sum += GetLeftVal(node_ind);
      ind -= left_weight;
      node_ind = node.right_ind;
    }
  }
  return sum;
}

uint64_t PrefixSum::FindInPositiveValues(int64_t val) const{
  if (val >= val_sum_){
    return Num();
  }
  if (Num() == 1){
    if (val < val_sum_) return 0;
    else return 1;
  }
  int64_t node_ind = root_ind_;
  int64_t ind = 0;
  while (node_ind >= 0){
    const PrefixSumNode& node = nodes_[node_ind];
    int64_t left_val = GetLeftVal(node_ind);
    if (val < left_val){
      node_ind = node.left_ind;
    } else {
      val -= left_val;
      ind += GetLeftWeight(node_ind);
      node_ind = node.right_ind;
    }
  }
  return ind;
}

int64_t PrefixSum::InsertInternal(int64_t node_ind, uint64_t ind, int64_t val){
  if (node_ind < 0){
    assert(ind < 2);
    int64_t pre_leave_ind = node_ind;
    PrefixSumLeaf& pre_leaf = leaves_[ToLeafInd(pre_leave_ind)];
    int64_t leaf_val = pre_leaf.val;
    PrefixSumNode new_node(2, leaf_val + val);
    int64_t new_node_ind = nodes_.size();
    pre_leaf.parent = new_node_ind;
    leaves_.push_back(PrefixSumLeaf(new_node_ind, val));
    int64_t new_leave_ind = -(static_cast<int64_t>(leaves_.size()));
    new_node.left_ind = (ind == 0) ? new_leave_ind : pre_leave_ind;
    new_node.right_ind = (ind == 0) ? pre_leave_ind : new_leave_ind;
    nodes_.push_back(new_node);
    return new_node_ind;
  }
  
  PrefixSumNode& node = nodes_[node_ind];
  node.weight += 1;
  node.sum += val;
  if (IsRED(node.left_ind) && IsRED(node.right_ind)){
    FlipColor(node_ind);
  }

  uint64_t left_weight = GetLeftWeight(node_ind);
  const PrefixSumNode& cur_node = nodes_[node_ind];
  if (ind < left_weight){
    int ret = InsertInternal(cur_node.left_ind, ind, val);
    nodes_[node_ind].left_ind = ret;
  } else {
    int64_t ret = InsertInternal(cur_node.right_ind, ind - left_weight, val);
    nodes_[node_ind].right_ind = ret;
  }
 
  if (IsRED(nodes_[node_ind].right_ind)){
    node_ind = RotateLeft(node_ind);
  }

  const PrefixSumNode& new_node = nodes_[node_ind];
  if (IsRED(new_node.left_ind)){
    if (IsRED(nodes_[new_node.left_ind].left_ind)){
      node_ind = RotateRight(node_ind);
    }
  }
  
  return node_ind;
}

bool PrefixSum::IsRED(int64_t node_ind) const{
  if (node_ind < 0) return false;
  return nodes_[node_ind].color == kRED;
}

void PrefixSum::FlipColor(int64_t node_ind) {
  PrefixSumNode& node = nodes_[node_ind];
  node.color = !node.color;
  if (node.left_ind >= 0){
    nodes_[node.left_ind].color = !nodes_[node.left_ind].color;
  }
  if (node.right_ind >= 0){
    nodes_[node.right_ind].color = !nodes_[node.right_ind].color;
  }
}

int64_t PrefixSum::RotateLeft(int64_t h_ind){
  PrefixSumNode& h = nodes_[h_ind];
  int64_t x_ind = h.right_ind;
  PrefixSumNode& x = nodes_[x_ind];
  x.sum = h.sum;
  x.weight = h.weight;
  h.sum = GetLeftVal(h_ind) + GetLeftVal(x_ind);
  h.weight = GetLeftWeight(h_ind) + GetLeftWeight(x_ind);
  //h.right_val = x.left_val;

  h.right_ind = x.left_ind;
  if (h.right_ind < 0) {
    leaves_[ToLeafInd(h.right_ind)].parent = h_ind;
  }

  x.left_ind = h_ind;
  if (x.left_ind < 0) {
    leaves_[ToLeafInd(x.left_ind)].parent = x_ind;
  }

  x.color = nodes_[x.left_ind].color;
  nodes_[x.left_ind].color = kRED;

  assert(x_ind >= 0);
  return x_ind;
}

int64_t PrefixSum::RotateRight(int64_t h_ind){
  PrefixSumNode& h = nodes_[h_ind];
  int64_t x_ind = h.left_ind;
  PrefixSumNode& x = nodes_[x_ind];
  x.sum = h.sum;
  x.weight = h.weight;
  h.sum = GetRightVal(h_ind) + GetRightVal(x_ind);
  h.weight = GetRightWeight(h_ind) + GetRightWeight(x_ind);
  //h.left_val = x.right_val;

  h.left_ind = x.right_ind;
  if (h.left_ind < 0) {
    leaves_[ToLeafInd(h.left_ind)].parent = h_ind;
  }
  x.right_ind = h_ind;
  if (x.right_ind < 0) {
    leaves_[ToLeafInd(x.right_ind)].parent = x_ind;
  }
  
  x.color = nodes_[x.right_ind].color;
  nodes_[x.right_ind].color = kRED;

  assert(x_ind >= 0);
  return x_ind;
}

/*
void PrefixSum::Delete(int64_t ind){
  if (ind >= num_){
    throw std::out_of_range("PrefixSum::InsertInternal out of range");
  }
  root_ind_ = DeleteInternal(root_ind_, ind);
  if (root_ind_ >= 0){
    nodes_[root_ind_].color = kBLACK;
  }
  //sum_ += val;
  --num_;
}
*/

/*
int64_t PrefixSum::DeleteInternal(int64_t node_ind, int64_t ind){
  if (node_ind < 0){
    int64_t delete_leaf_ind = ToLeafInd(node_ind);
    if (delete_leaf_ind != leaves_.size()-1){
      std::swap(leaves_[delete_leaf_ind], leaves_[leaves_.size()-1]);
      
    }
    return -777;
  }
  int64_t left_weight = GetLeftWeight(node_ind);
  int64_t left_ind = nodes_[node_ind].left_ind;
  if (ind < left_weight){
    if (!IsRED(left_ind) && left_ind >= 0 && !IsRED(nodes_[left_ind].left_ind)){
      node_ind = MoveREDLeft(node_ind);
    }
    int ret = DeleteInternal(nodes_[node_ind].left_ind, ind);
    nodes_[node_ind].left_ind = ret;
  } else {
    if (IsRED(left_ind)){
      node_ind = RotateRight(node_ind);
    }
    int64_t right_ind = nodes_[node_ind].right_ind;
    if (!IsRED(right_ind) && right_ind >= 0 && !IsRED(nodes_[right_ind].left_ind)){
      node_ind = MoveREDRight(node_ind);
    }
    int ret = DeleteInternal(nodes_[node_ind].right_ind, ind - left_weight); 
    nodes_[node_ind].right_ind = ret;
  }
  return FixUp(node_ind);
}

int64_t PrefixSum::MoveREDLeft(int64_t node_ind){
  FlipColor(node_ind);
  int64_t right_ind = nodes_[node_ind].right_ind;
  if (right_ind >= 0 && IsRED(nodes_[right_ind].left_ind)){
    nodes_[node_ind].right_ind = RotateRight(right_ind);
    node_ind = RotateLeft(node_ind);
    FlipColor(node_ind);
  }
  return node_ind;
}

int64_t PrefixSum::MoveREDRight(int64_t node_ind){
  FlipColor(node_ind);
  int64_t left_ind = nodes_[node_ind].left_ind;
  if (left_ind >= 0 && IsRED(nodes_[left_ind].left_ind)){
    node_ind = RotateRight(node_ind);
    FlipColor(node_ind);
  }
  return node_ind;
}

int64_t PrefixSum::FixUp(int64_t node_ind){
  if (node_ind < 0) return node_ind;
  if (IsRED(nodes_[node_ind].right_ind)){
    node_ind = RotateLeft(node_ind);
  }
  int64_t left_ind = nodes_[node_ind].left_ind;
  if (IsRED(left_ind) && IsRED(nodes_[left_ind].left_ind)){
    node_ind = RotateRight(node_ind);
  }
  if (IsRED(nodes_[node_ind].left_ind) && IsRED(nodes_[node_ind].right_ind)){
    FlipColor(node_ind);
  }
  return node_ind;
}
*/

} // namespace prefixsum
