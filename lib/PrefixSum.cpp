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

PrefixSum::PrefixSum() : root_(NULL), num_(0), sum_(0){
}

PrefixSum::~PrefixSum(){
}

void PrefixSum::Clear(){
  delete root_;
  num_ = 0;
  sum_= 0;
}

void PrefixSum::Insert(int64_t ind, int64_t val){
  if (ind > num_){
    throw std::out_of_range("PrefixSum::InsertInternal out of range");
  }
  if (num_ == 0){
    sum_ = val;
    ++num_;
    return;
  } 
  root_ = InsertInternal(root_, ind, val, sum_);
  root_->color = kBLACK;
  sum_ += val;
  ++num_;
}

void PrefixSum::Add(int64_t ind, int64_t val){
  if (ind >= num_){
    throw std::out_of_range("PrefixSum::Add out of range");  
  }
  sum_ += val;
  if (num_ == 1){
    return;
  }
  PrefixSumNode* node = root_;
  for (;;){
    node->sum += val;
    int64_t left_weight = node->GetLeftWeight();
    if (ind < left_weight){
      if (node->left == NULL){
        node->left_val += val;
        return;
      }
      node = node->left;
    } else {
      ind -= left_weight;
      if (node->right == NULL){
        node->right_val += val;
        return;
      }
      node = node->right;
    }
  }
}

void PrefixSum::Set(int64_t ind, int64_t val){
  if (ind >= num_){
    throw std::out_of_range("PrefixSum::Set out of range");  
  }
  int64_t cur_val = Get(ind);
  Add(ind, val - cur_val);
}

int64_t PrefixSum::Get(int64_t ind) const{
  if (ind >= num_){
    throw std::out_of_range("PrefixSum::Add out of range");  
  }
  if (num_ == 1){
    return sum_;
  }
  PrefixSumNode* node = root_;
  for (;;){
    int64_t left_weight = node->GetLeftWeight();
    if (ind < left_weight){
      if (node->left == NULL){
        return node->left_val;
      }
      node = node->left;
    } else {
      if (node->right == NULL){
        return node->right_val;
      }
      ind -= left_weight;
      node = node->right;
    }
  }
}

int64_t PrefixSum::GetPrefixSum(int64_t ind) const{
  if (ind > num_){
    throw std::out_of_range("PrefixSum::Add out of range");  
  }
  if (ind == num_) {
    return sum_;
  }
  if (num_ == 1){
    if (ind == 0) return 0;
    else return sum_;
  }
  PrefixSumNode* node = root_;
  int64_t sum = 0;
  while (node != NULL){
    int64_t left_weight = node->GetLeftWeight();
    if (ind < left_weight){
      node = node->left;
    } else {
      sum += node->GetLeftVal();
      ind -= left_weight;
      node = node->right;
    }
  }
  return sum;
}

int64_t PrefixSum::FindInPositiveValues(int64_t val) const{
  if (val >= sum_){
    return num_;
  }
  if (num_ == 1){
    if (val < sum_) return 0;
    else return 1;
  }
  PrefixSumNode* node = root_;
  int64_t ind = 0;
  while (node != NULL){
    int64_t left_val = node->GetLeftVal();
    if (val < left_val){
      node = node->left;
    } else {
      val -= left_val;
      ind += node->GetLeftWeight();
      node = node->right;
    }
  }
  return ind;
}

PrefixSumNode* PrefixSum::InsertInternal(PrefixSumNode* h, int64_t ind, int64_t val, int64_t leaf_val){
  if (h == NULL){
    assert(ind < 2);
    PrefixSumNode* new_node = new PrefixSumNode(2, leaf_val + val);
    new_node->left_val = (ind == 0) ? val : leaf_val;
    new_node->right_val = (ind == 0) ? leaf_val : val;
    return new_node;
  }

  h->weight += 1;
  h->sum += val;
  if (IsRED(h->left) && IsRED(h->right)){
    FlipColor(h);
  }

  int64_t left_weight = h->GetLeftWeight();
  if (ind < left_weight){
    h->left = InsertInternal(h->left, ind, val, h->left_val);
  } else {
    h->right = InsertInternal(h->right, ind - left_weight, val, h->right_val);
  }
  
  if (IsRED(h->right)){
    h = RotateLeft(h);
  }
  
  if (IsRED(h->left) && IsRED(h->left->left)){
    h = RotateRight(h);
  }
  
  return h;
}

bool PrefixSum::IsRED(PrefixSumNode* h) const{
  if (h == NULL) return false;
  return h->color == kRED;
}

void PrefixSum::FlipColor(PrefixSumNode* x){
  x->color = !x->color;
  if (x->left != NULL){
    x->left->color = !x->left->color;
  }
  if (x->right != NULL){
    x->right->color = !x->right->color;
  }
}

PrefixSumNode* PrefixSum::RotateLeft(PrefixSumNode* h){
  PrefixSumNode* x = h->right;
  x->sum = h->sum;
  x->weight = h->weight;
  h->sum = h->GetLeftVal() + x->GetLeftVal();
  h->weight = h->GetLeftWeight() + x->GetLeftWeight();
  h->right_val = x->left_val;

  h->right = x->left;
  x->left = h;
  x->color = x->left->color;
  x->left->color = kRED;


  return x;
}

PrefixSumNode* PrefixSum::RotateRight(PrefixSumNode* h){
  PrefixSumNode* x = h->left;
  x->sum = h->sum;
  x->weight = h->weight;
  h->sum = h->GetRightVal() + x->GetRightVal();
  h->weight = h->GetRightWeight() + x->GetRightWeight();
  h->left_val = x->right_val;

  h->left = x->right;
  x->right = h;
  x->color = x->right->color;
  x->right->color = kRED;


  return x;
}

} // namespace prefixsum
