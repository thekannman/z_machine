#include "tree.h"

void Tree::split(int variable, double value) {
  arma::vec feature = (*pred_group_).feature(variable);
  arma::uvec value_left = (feature < value);
  arma::uvec value_right = (feature >= value);
  arma::uvec going_left(feature.size(), arma::fill::zeros);
  arma::uvec going_right(feature.size(), arma::fill::zeros);
  int left_count = 0, right_count = 0;
  for (int i : items_contained_) {
    if (value_left(i))
      going_left(left_count++) = i;
    else
      going_right(right_count++) = i;
  }
  going_left.resize(left_count);
  going_right.resize(right_count);
  left_ = std::make_shared<Tree>(this, going_left);
  right_ = std::make_shared<Tree>(this, going_right);
}

double Tree::metric_setup(const Prediction_group& pred_group) {
  std::map<int, int> group_counts;
  for (int i : items_contained_) {
    group_counts[pred_group.y(i)]++;
  }
  if (group_counts.size() == 1) {
    purity_check_ = true;
    return 0.0;
  }
  return (this->*metric)(group_counts);
}

double Tree::entropy(std::map<int, int>& group_counts) {
  double total_entropy = 0.0;
  int count = item_count();
  for (auto i : group_counts) {
    double p = static_cast<double>(i.second) / count;
    total_entropy += -p*log2(p);
  }
  return total_entropy;
}

double Tree::gini(std::map<int, int>& group_counts) {
  double total_gini = 0.0;
  int count = item_count();
  for (auto i : group_counts) {
    double p = static_cast<double>(i.second) / count;
    total_gini += p*(1-p);
  }
  return total_gini;
}

double Tree::misclassification(std::map<int, int>& group_counts) {
  double max_p = 0.0;
  int count = item_count();
  for (auto i : group_counts) {
    double p = static_cast<double>(i.second) / count;
    if (p > max_p)
      max_p = p;
  }
  return 1 - max_p;
}

double Tree::try_split(int variable, double value, bool print) {
  split(variable, value);
  double gained = gain();
  std::vector<double> h_lines, v_lines(1,value);
  if (print) {
    std::cout << "The gain for this trial split would be: " << gain() << std::endl;
//    (*pred_group_).plot(variable, 1, h_lines, v_lines);
  }
  left_.reset();
  right_.reset();
  return gained;
}

double Tree::gain() {
  double total_count = static_cast<double>(item_count());
  double gained = metric_setup();
  gained -= (left_->item_count() / total_count) * left_->metric_setup();
  gained -= (right_->item_count() / total_count) * right_->metric_setup();

  return gained;
}

void Tree::find_splits() {
  find_split();
  if (left_)
    left_->find_splits();
  if (right_)
    right_->find_splits();
}

void Tree::find_split() {
  arma::vec split_points;
  double best_gain = 1.0e-10, best_value = -1;
  int best_feature = -1;
  for (int i = 0, i_max = pred_group_->num_features(); i < i_max; ++i) {
    pred_group_->find_splits(i, split_points, &items_contained_);
    for (auto value : split_points) {
      double trial_gain = try_split(i, value);
      if (trial_gain > best_gain) {
        best_gain = trial_gain;
        best_feature = i;
        best_value = value;
      }
    }
  }
  if (best_feature != -1) {
    std::cout << "Splitting on feature " << best_feature << " at value " << best_value << " for a gain of " << best_gain << std::endl;
    split(best_feature, best_value);
    split_variable_ = best_feature;
    split_value_ = best_value;
  }
  else {
    std::cout << "Failed to find splitting point" << std::endl;
  }
}
void Tree::set_test_set(std::shared_ptr<Numeric_pred_group> test_set) {
  test_items_contained_.set_size(test_set->num_samples());
  test_items_contained_.imbue([&]() { static int i = 0; return i++; });
  propagate_set(test_set, &test_items_contained_);
}

void Tree::propagate_set(std::shared_ptr<Numeric_pred_group> test_set, arma::uvec* include) {
  test_group_ = test_set;
  test_items_contained_ = *include;
  if (split_variable_ == -1)
    return;
  arma::vec feature = (*test_set).feature(split_variable_);
  arma::uvec value_left = (feature < split_value_);
  arma::uvec value_right = (feature >= split_value_);
  arma::uvec going_left(feature.size(), arma::fill::zeros);
  arma::uvec going_right(feature.size(), arma::fill::zeros);
  int left_count = 0, right_count = 0;
  for (int i : (*include)) {
    if (value_left(i))
      going_left(left_count++) = i;
    else
      going_right(right_count++) = i;
  }
  going_left.resize(left_count);
  going_right.resize(right_count);
  if (left_)
    left_->propagate_set(test_set, &going_left);
  if (right_)
    right_->propagate_set(test_set, &going_right);
}
