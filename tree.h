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
    getchar();
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
//  void plot_tree();
//  void plot_leaves(Gnuplot& gp, double box_width, double h_offset = 0.0, double y_offset = 0.0, double old_h_offset = 0.0);
//  void plot_leaf(Gnuplot& gp, double box_width, double h_offset, double y_offset, double old_h_offset);
//  void plot_groups(bool plot_test = false);
//  void plot_group_leaves(Gnuplot& gp, bool test, double box_width, double h_offset = 0.0, double y_offset = 0.0, double old_h_offset = 0.0);
//  double plot_group_leaf(Gnuplot& gp, bool test, double box_width, double h_offset, double y_offset, double old_h_offset);
  double entropy(std::map<int, int>& group_counts);
  double gini(std::map<int, int>& group_counts);
  double misclassification(std::map<int, int>& group_counts);
  void set_test_set(std::shared_ptr<Numeric_pred_group> test_set);

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

#endif
