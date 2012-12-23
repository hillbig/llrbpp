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

#ifndef LLRBPP_LLRBPP_HPP_
#define LLRBPP_LLRBPP_HPP_

#include <stdint.h>
#include <iostream>
#include <exception>
#include "llrbppNode.hpp"

namespace llrbpp{

class NotFound : std::exception {
};

template <class Key, class Val, class Comp = std::less<Key> >
class LLRBPP{
public:
  LLRBPP() : root_(NULL), num_(0){
  }

  ~LLRBPP(){
    delete root_;
  }

  void Insert(Key key, Val val){
    root_ = InsertInternal(root_, key, val);
    root_->color = kBLACK;
  }

  void Delete(Key key){
    root_ = DeleteInternal(root_, key);
    if (root_ != NULL){
      root_->color = kBLACK;
    }
    --num_;
  }

  void Clear(){
    delete root_;
    root_ = NULL;
    num_ = 0;
  }

  Val Search(Key key) const{
    Node<Key, Val>* node = root_;
    while (node != NULL){
      if (key == node->key){
        return node->val;
      } else if (Comp()(key, node->key)){
        node = node->left;
      } else {
        node = node->right;
      }
    }
    throw NotFound();
  }

  int DepthSum() const{
    return DepthSumInternal(root_, 0);
  }

  int DepthMax() const{
    return DepthMaxInternal(root_, 0);
  }

  uint64_t Num() const {
    return num_;
  }

private:
  bool IsRED(Node<Key, Val>* h) const {
    // IsRED is true iff h is not NULL and h is RED
    if (h == NULL) return false;
    return h->color == kRED;
  }

  void FlipColor(Node<Key, Val>* x){
    // assert(x->left != NULL) && assert(x->right)
    x->color = !x->color;
    if (x->left != NULL){
      x->left->color = !x->left->color;
    }
    if (x->right != NULL){
      x->right->color = !x->right->color;
    }
  }

  Node<Key, Val>* RotateLeft(Node<Key, Val>* h){
    Node<Key, Val>* x = h->right;
    h->right = x->left;
    x->left = h;
    x->color = x->left->color;
    x->left->color = kRED;
    return x;
  }

  Node<Key, Val>* RotateRight(Node<Key, Val>* h){
    Node<Key, Val>* x = h->left;
    h->left = x->right;
    x->right = h;
    x->color = x->right->color;
    x->right->color = kRED;
    return x;
  }

  Node<Key, Val>* InsertInternal(Node<Key, Val>* h, Key key, Val val){
    if (h == NULL){
      ++num_;
      return new Node<Key, Val>(key, val);
    }
    
    if (IsRED(h->left) && IsRED(h->right)){
      FlipColor(h);
    }

    if (key == h->key){
      h->val = val;
    } else if (Comp()(key, h->key)){
      h->left = InsertInternal(h->left, key, val);
    } else {
      h->right = InsertInternal(h->right, key, val);
    }
    
    if (IsRED(h->right)){
      h = RotateLeft(h);
    }

    if (IsRED(h->left) && IsRED(h->left->left)){
      h = RotateRight(h);
    }

    return h;
  }

  Node<Key, Val>* MoveREDLeft(Node<Key, Val>* h){
    FlipColor(h);
    if (h->right != NULL && IsRED(h->right->left)){
      h->right = RotateRight(h->right);
      h = RotateLeft(h);
      FlipColor(h);
    }
    return h;
  }

  Node<Key, Val>* MoveREDRight(Node<Key, Val>* h){
    FlipColor(h);
    if (h->left != NULL && IsRED(h->left->left)){
      h = RotateRight(h);
      FlipColor(h);
    }
    return h;
  }

  Node<Key, Val>* FixUp(Node<Key, Val>* h){
    if (IsRED(h->right)){
      h = RotateLeft(h);
    }
    if (IsRED(h->left) && IsRED(h->left->left)){
      h = RotateRight(h);
    }
    if (IsRED(h->left) && IsRED(h->right)){
      FlipColor(h);
    }
    return h;
  }

  Node<Key, Val>* DeleteMin(Node<Key, Val>* h){
    if (h->left == NULL){
      return NULL;
    }
    if (!IsRED(h->left) && !IsRED(h->left->left)){
      h = MoveREDLeft(h);
    }
    Node<Key, Val>* new_left = DeleteMin(h->left);
    if (new_left == NULL){
      --num_;
      delete h->left;
    }
    h->left = new_left;
    return FixUp(h);
  }

  Node<Key, Val>* GetMin(Node<Key, Val>* h){
    while (h->left != NULL){
      h = h->left;
    }
    return h;
  }

  Node<Key, Val>* DeleteInternal(Node<Key, Val>* h, Key key){
    if (h == NULL) return NULL;
    if (Comp()(key, h->key)){
      if (!IsRED(h->left) && 
          h->left != NULL &&
          !IsRED(h->left->left)){
        h = MoveREDLeft(h);
      }
      h->left = DeleteInternal(h->left, key);
    } else {
      if (IsRED(h->left)){
        h = RotateRight(h);
      }
      if ((key == h->key) && (h->right == NULL)){
        return NULL;
      }
      if (!IsRED(h->right) && 
          h->right != NULL && 
          !IsRED(h->right->left)){
        h = MoveREDRight(h);
      }
      if (key == h->key){
        Node<Key, Val>* min_node = GetMin(h->right);
        h->key = min_node->key;
        h->val = min_node->val;
        h->right = DeleteMin(h->right);
      } else {
        h->right = DeleteInternal(h->right, key);
      }
    }
    return FixUp(h);
  }

  int DepthSumInternal(const Node<Key, Val>* h, int depth) const{
    if (h == NULL) return 0;
    return depth 
      + DepthSumInternal(h->left, depth+1)
      + DepthSumInternal(h->right, depth+1);
  }

  static int MyMax(int x, int y) {
    return (x > y) ? x : y;
  }

  int DepthMaxInternal(const Node<Key, Val>* h, int depth) const{
    if (h == NULL) return depth;
    return MyMax(DepthMaxInternal(h->left, depth+1),
                 DepthMaxInternal(h->right, depth+1));
  }

  Node<Key, Val>* root_;
  uint64_t num_;
};

} // namespace llrbpp

#endif // LLRBPP_LLRBPP_HPP_
