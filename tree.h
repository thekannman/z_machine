//Copyright (c) 2015 Zachary Kann
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

// ---
// Author: Zachary Kann

#ifndef _TREE_H_
#define _TREE_H_

#include <vector>
#include <map>
#include <armadillo>
#include <numeric>
#include "numeric_pred_group.h"

class Tree {
 public:
  Tree(std::shared_ptr<Numeric_pred_group> pred_group, double(Tree::*m)(std::map<int, int>&) = &Tree::entropy)
    : metric(m), pred_group_(pred_group), items_contained_(pred_group->num_samples()),
      depth_(1), is_root_(true) {
    items_contained_.imbue([&]() { static int i = 0; return i++; });
    total_depth_ = std::make_shared<int>(1);
  };
  Tree(Tree* root, arma::uvec& items_contained)
    : metric(root->metric), pred_group_(root->pred_group_), items_contained_(items_contained), total_depth_(root->total_depth_),
      depth_((root->depth_)+1) {
    if (depth_ > *total_depth_)
      (*total_depth_)++;
  };

  // Find best variable/value and create child nodes
  void split(int variable, double value);
  inline int item_count() { return items_contained_.size(); }
  double try_split(int variable, double value, bool print = false);
  void find_splits();
  void find_split();
  double gain();
  double (Tree::*metric)(std::map<int, int>&);
  std::shared_ptr<Tree> left() { return left_; }
  std::shared_ptr<Tree> right() { return right_; }
  double entropy(std::map<int, int>& group_counts);
  double gini(std::map<int, int>& group_counts);
  double misclassification(std::map<int, int>& group_counts);
  void set_test_set(std::shared_ptr<Numeric_pred_group> test_set);
  int total_depth() { return *total_depth_; }
  int depth() { return depth_; }
  std::shared_ptr<Numeric_pred_group> pred_group() { return pred_group_; }
  std::shared_ptr<Numeric_pred_group> test_group() { return test_group_; }
  arma::uvec& items_contained() { return items_contained_; }
  arma::uvec& test_items_contained() { return test_items_contained_; }
  int split_variable() { return split_variable_; }
  int split_value() { return split_value_; }

 private:
  int split_variable_ = -1;
  double split_value_;
  std::shared_ptr<Tree> left_;
  std::shared_ptr<Tree> right_;
  std::shared_ptr<Numeric_pred_group> pred_group_;
  std::shared_ptr<Numeric_pred_group> test_group_;
  arma::uvec items_contained_;
  arma::uvec test_items_contained_;
  int purity_check_ = false;
  std::shared_ptr<int> total_depth_;
  int depth_;
  bool is_root_ = false;

  double metric_setup(const Prediction_group& pred_group);
  double metric_setup() { return metric_setup(*pred_group_); }
  void propagate_set(std::shared_ptr<Numeric_pred_group> test_set, arma::uvec* include);
};

#endif // _TREE_H

