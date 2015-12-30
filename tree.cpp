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
/*
void Tree::plot_tree() {
  Gnuplot gp("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\"");
  double x_min = -7.0 - 8.5*(*total_depth_)*(1-std::pow(2.0, (*total_depth_)))/(1-2.0);
  double x_max = 7.0 + 8.5*(*total_depth_)*(1 - std::pow(2.0, (*total_depth_))) / (1 - 2.0);
  double y_min = -2.0 - 3.0*((*total_depth_) - 1);
  double box_width = 1.5*x_max / (*total_depth_);
  gp << "set key off\n" << "set xrange[" << x_min << ":" << x_max << "]\n"
     << "set yrange[" << y_min << ":2]\n"
     << "unset xtics\n" << "unset ytics\n" << "unset border\n";

  plot_leaves(gp, box_width);

  gp << "plot 1/0\n";
  gp.flush();
  getchar();
}

void Tree::plot_leaves(Gnuplot& gp, double box_width, double h_offset, double v_offset, double old_h_offset) {
  plot_leaf(gp, box_width, h_offset, v_offset, old_h_offset);
  if (left_)
    left_->plot_leaves(gp, box_width, h_offset - 8.5*(*total_depth_)*std::pow(2.0, (*total_depth_) - depth_), v_offset - 3.0, h_offset);
  if (right_)
    right_->plot_leaves(gp, box_width, h_offset + 8.5*(*total_depth_)*std::pow(2.0, (*total_depth_) - depth_), v_offset - 3.0, h_offset);
}

void Tree::plot_leaf(Gnuplot& gp, double box_width, double h_offset, double v_offset, double old_h_offset) {
  std::map<int, int> group_counts;
  for (int i = 0, i_max = pred_group_->num_categories(); i < i_max; ++i) {
    group_counts[i] = 0;
  }
  for (int i : items_contained_) {
    group_counts[pred_group_->y(i)]++;
  }
  gp << "set object rect from " << h_offset - box_width/2.0 << "," << v_offset - 1.0 << " to " << h_offset + box_width / 2.0 << "," << v_offset + 1.0 << "\n";
  if (split_variable_ != -1)
    gp << "set label 'Variable " << split_variable_ << " at value "
       << std::setprecision(2) << std::fixed << split_value_ << "' at " << h_offset << "," << v_offset + 0.45 << " center font 'Sans, 10'\n";
  gp << "set label '  ";
  for (auto i : group_counts) {
    gp << i.second << "  ";
  }
  gp << "' at " << h_offset << "," << v_offset - 0.45 << " center font 'Sans, 10'\n";
  Line line(old_h_offset, v_offset + 2.0, h_offset, v_offset + 1.0);
  if (!is_root_)
    line.draw(gp);
}

void Tree::plot_groups(bool test) {
  std::shared_ptr<Numeric_pred_group> plot_group = test ? pred_group_ : test_group_;
  Gnuplot gp("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\"");
  double x_min = -7.0 - 8.5*(*total_depth_)*(1 - std::pow(2.0, (*total_depth_))) / (1 - 2.0);
  double x_max = 7.0 + 8.5*(*total_depth_)*(1 - std::pow(2.0, (*total_depth_))) / (1 - 2.0);
  double y_min = -2.0 - 3.0*(*total_depth_);
  double box_width = 0.0;
  gp << "set key off\n" << "set xrange[" << x_min << ":" << x_max << "]\n"
    << "set yrange[" << y_min << ":2]\n"
    << "unset xtics\n" << "unset ytics\n" << "unset border\n";

  plot_group_leaves(gp, test, box_width);

  gp << "plot 1/0\n";
  gp.flush();
  getchar();
}

void Tree::plot_group_leaves(Gnuplot& gp, bool test, double box_width, double h_offset, double v_offset, double old_h_offset) {
  v_offset = plot_group_leaf(gp, test, box_width, h_offset, v_offset, old_h_offset);
  if (left_)
    left_->plot_group_leaves(gp, test, box_width, h_offset - 8.5*(*total_depth_)*std::pow(2.0, (*total_depth_) - depth_), v_offset - 3.0, h_offset);
  if (right_)
    right_->plot_group_leaves(gp, test, box_width, h_offset + 8.5*(*total_depth_)*std::pow(2.0, (*total_depth_) - depth_), v_offset - 3.0, h_offset);
}

double Tree::plot_group_leaf(Gnuplot& gp, bool test, double box_width, double h_offset, double v_offset, double old_h_offset) {
  std::map<int, int> group_counts;
  std::shared_ptr<Numeric_pred_group> plot_group = test ? test_group_ : pred_group_;
  arma::uvec& items_contained = test ? test_items_contained_ : items_contained_;
  for (int i = 0, i_max = plot_group->num_categories(); i < i_max; ++i) {
    group_counts[i] = 0;
  }
  for (int i : items_contained) {
    group_counts[plot_group->y(i)]++;
  }
  if (group_counts.size() % 2 == 0)
    v_offset += 0.8 * (group_counts.size() / 2.0 + 0.5);
  else
    v_offset += 0.8 * (group_counts.size() / 2.0);
  double max_v_offset = v_offset;
  for (auto i : group_counts) {
    gp << "set label '"
      << i.second << ""
      << "' at " << h_offset << "," << v_offset << " center font 'Sans, 10'\n";
      v_offset -= 0.8;
  }
  Line line(old_h_offset, max_v_offset + 2.0, h_offset, max_v_offset + 0.4);
  if (!is_root_)
    line.draw(gp);
  return v_offset;
}
*/
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
