#define _USE_MATH_DEFINES

#include "numeric_pred_group.h"
#include <ctime>
#include <cmath>
#include <random>


bool Numeric_pred_group::read_csv(const std::string& filename, bool has_header) {
  std::ifstream input(filename);
  std::string line;
  std::vector<std::string> split_line;
  int line_count = 0;
  int feature_count = 0;
  // Skip Header
  if (has_header)
    getline(input, line);
  // Grab one line to parse
  getline(input, line);
  line_count++;
  split_line = split(line, ',');
  feature_count = split_line.size() - 1; // -1 is for y_
  // Count lines
  while (getline(input, line)) {
    if (!line.empty())
      line_count++;
  }

  x_ = std::make_shared<arma::mat>(line_count, feature_count, arma::fill::zeros);
  y_ = std::make_shared<arma::ivec>(line_count, arma::fill::zeros);
  y_map_  = std::make_shared<std::map<std::string, int> >();
  // clear eof flag and return to beginning of file
  input.clear();
  input.seekg(0, std::ios::beg);
  int line_it = 0;

  while (getline(input, line)) {
    if (line.empty())
      continue;
    std::vector<std::string> split_line = split(line, ',');
    // currently assuming that last entry is y valu

    if (y_map_->find(split_line.back()) == y_map_->end()) {
      (*y_)(line_it) = y_map_->size();
      (*y_map_)[split_line.back()] = (*y_)(line_it);
    }
    else {
      (*y_)(line_it) = (*y_map_)[split_line.back()];
    }

    split_line.pop_back();
    for (int i = 0, i_max = static_cast<int>(split_line.size()); i < i_max; ++i) {
      (*x_)(line_it, i) = std::stod(split_line[i]);
    }

    line_it++;
  }

  return true;
}

std::string Numeric_pred_group::y_string(int i) {
  // No way to do find on values, have to iterate instead.
  // Should change to use bidirectional map later, perhaps
  // using boost::multiindex.
  for (auto item : *y_map_) {
    if (item.second == (*y_)(i))
      return item.first;
  }
  return "";
}

std::string Numeric_pred_group::category(int i) {
  // No way to do find on values, have to iterate instead.
  // Should change to use bidirectional map later, perhaps
  // using boost::multiindex.
  for (auto item : *y_map_) {
    if (item.second == i)
      return item.first;
  }
  return "";
}

std::vector<std::string> Numeric_pred_group::y_strings() {
  std::vector<std::string> strings;
  for (int i = 0, i_max = static_cast<int>(y_->size()); i < i_max; ++i)
    strings.push_back(y_string(i));
  return strings;
}

void Numeric_pred_group::find_uniques(int feature_index, arma::vec& unique_values, arma::uvec* include) {
  arma::vec chosen_feature = feature(feature_index);
  arma::vec included_feature;
  if (include)
    included_feature = chosen_feature.elem(*include);
  unique_values = arma::unique(chosen_feature);
}

void Numeric_pred_group::find_splits(int feature_index, arma::vec& split_points, arma::uvec* include) {
  find_uniques(feature_index, split_points, include);
  for (int i = 0, i_max = split_points.size()-1; i < i_max; ++i) {
    split_points(i) = (split_points(i) + split_points(i + 1)) / 2.0;
  }
  split_points.resize(split_points.size() - 1);
}

void Numeric_pred_group::training_split(double training_fraction) {
  arma::uvec elements(num_samples());
  elements.imbue([&]() { static int i = 0; return i++; });
  arma::arma_rng::set_seed_random();
  elements = arma::shuffle(elements);
  int train_count = training_fraction*num_samples();
  int test_count = num_samples() - train_count;
  arma::uvec train_elems, test_elems;
  train_elems = elements.head(train_count);
  test_elems = elements.head(test_count);
  arma::mat train_x,test_x;
  arma::ivec train_y, test_y;
  train_x = x_->rows(train_elems);
  test_x = x_->rows(test_elems);
  train_y = y_->elem(train_elems);
  test_y = y_->elem(test_elems);
  train_group_ = std::make_shared<Numeric_pred_group>(train_x, train_y, y_map_);
  test_group_ = std::make_shared<Numeric_pred_group>(test_x, test_y, y_map_);
}

std::shared_ptr<Numeric_pred_group> Numeric_pred_group::grab_training(double training_fraction) {
  if (!train_group_)
    training_split(training_fraction);
  return train_group_;
}

std::shared_ptr<Numeric_pred_group> Numeric_pred_group::grab_testing(double training_fraction) {
  if (!test_group_)
    training_split(training_fraction);
  return test_group_;
}

void Numeric_pred_group::pca() {
  auto stddev_mat = arma::stddev(*x_);
  x_->each_row() /= stddev_mat;
  pca_x_ = std::make_shared<arma::mat>();
  arma::vec tsquared;
  arma::princomp(pca_coeff_, *pca_x_, pca_var_, tsquared, *x_);
  pca_set_ = true;
}

void Numeric_pred_group::euclidean_distance(bool scale, std::shared_ptr<arma::mat> distances) {
  arma::mat new_x(*x_);
  distances_ = std::make_shared<arma::mat>(new_x.n_rows, new_x.n_rows, arma::fill::zeros);
  arma::rowvec dist_vec;
  if (scale) {
    auto stddev_mat = arma::stddev(new_x);
    new_x.each_row() /= stddev_mat;
  }
  for (int i = 0, i_max = new_x.n_rows; i < i_max; ++i) {
    for (int j = i+1, j_max = new_x.n_rows; j < j_max; ++j) {
      dist_vec = new_x.row(i) - new_x.row(j);
      dist_vec = dist_vec % dist_vec;
      (*distances_)(i,j) = (*distances_)(j,i) = sqrt(arma::sum(dist_vec));
    }
  }
  if (distances != nullptr)
    distances = distances_;
}

void Numeric_pred_group::plot_distances() {
  const int DIMS = 2;
  double step_size = 1.0e-2;
  const int MAX_STEPS = 1e5;
  const double POT_TOL = 1.0e-3;
  const double DIFF_TOL = 1.0e-5;
  const double DIST_MIN = 1.0e-7;
  const int MAX_JUMPS = 100;
  int jump_count = 0;
  if (distances_ == nullptr)
    euclidean_distance(true);
  arma::mat positions(num_samples(), DIMS);
  randomize_positions(positions);
  arma::mat best_positions = positions;
  double best_potential = std::numeric_limits<double>::max();
  arma::mat forces(num_samples(), DIMS);
  double old_potential = std::numeric_limits<double>::max();
  for (int step = 0; step < MAX_STEPS; ++step) {
    // Essentially an MD problem here
    // I'm thinking something like U = (dist - ideal_dist)^2 / 2.0
    // Thus F = -(dist - ideal_dist)
    double total_potential = 0.0;
    forces.zeros();
    for (int i = 0; i < num_samples(); ++i) {
      for (int j = i + 1; j < num_samples(); ++j) {
        arma::rowvec dx = positions.row(i) - positions.row(j);
        double distance = sqrt(arma::sum(dx%dx));
        double distance_to_ideal = distance - (*distances_)(i, j);
        total_potential += distance_to_ideal*distance_to_ideal; // Not divided by two since two contributions
        if (distance >= DIST_MIN) { // Avoids distance being numerically zero resulting in infinite force
          forces.row(i) -= distance_to_ideal*dx / distance;
          forces.row(j) += distance_to_ideal*dx / distance;
        }
        double max_force = forces.max();
        double min_force = forces.min();
      }
      positions.row(i) += step_size*forces.row(i);
    } 
    if (step % 100 == 0)
      std::cout << "Iteration " << step << " complete. U_tot/N^2 = " << total_potential/num_samples()/num_samples() << " and potential change is " << abs((total_potential - old_potential) / old_potential)*100 << "%" << std::endl;
    if (total_potential < best_potential) {
      best_potential = total_potential;
      best_positions = positions;
    }
    if (total_potential / num_samples()/ num_samples() < POT_TOL)
      break;
    if (abs((total_potential - old_potential) / old_potential) < DIFF_TOL) {
      if (jump_count++ < MAX_JUMPS) {
        randomize_positions(positions);
        std::cout << "Initiating randomization " << jump_count << " of " << MAX_JUMPS << std::endl;
      } else {
        break;
      }
    }
    old_potential = total_potential;
  }
  std::cout << "Minimum potential was U_tot/N^2 = " << best_potential / num_samples() / num_samples() << std::endl;
  // TODO (Zak): set this up with qt plotting. Might need to save best_positions as class member to make that happen.
  //plot(0, 1, best_positions);
}

void Numeric_pred_group::grab_hist(int feat_i, QVector<double>& keys, QVector<double>& values, double& width, int num_bins) {
  double x_min = floor(feature(feat_i).min());
  double x_max = ceil(feature(feat_i).max());
  width = (x_max - x_min) / num_bins;

  arma::vec A;
  A = x().col(feat_i);

  double increment = 1.0 / (width*A.size());

  keys.resize(num_bins);
  values.resize(num_bins);
  keys.fill(0.0);
  values.fill(0.0);

  for (int i = 0; i < keys.size(); ++i) {
    keys[i] = x_min + width*(i+0.5);
  }

  int which_bin;
  for (auto val : A) {
    which_bin = static_cast<int>((val-x_min)/width);
    values[which_bin] += increment;
  }

}

void Numeric_pred_group::grab_group_hist(int feat_i, QVector<QVector<double> >& keys, QVector<QVector<double> >& values, double& width, int num_bins) {
  double x_min = floor(feature(feat_i).min());
  double x_max = ceil(feature(feat_i).max());
  width = (x_max - x_min) / num_bins;

  arma::vec A;
  A = x().col(feat_i);

  double increment = 1.0 / (width*A.size());

  keys.resize(num_categories());
  values.resize(num_categories());


  for (int i = 0; i < keys.size(); ++i) {
    keys[i].resize(num_bins);
    values[i].resize(num_bins);
    values[i].fill(0.0);
    for (int j = 0; j < keys[i].size(); ++j) {
      keys[i][j] = x_min + width*(j+0.5);
    }
  }

  int which_bin;
  for (int i = 0; i < A.size(); ++i) {
    which_bin = static_cast<int>((A[i]-x_min)/width);
    values[(*y_)(i)][which_bin] += increment;
  }

}


void Numeric_pred_group::randomize_positions(arma::mat& positions) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  double max_dist = distances_->max();
  static std::uniform_real_distribution<double> dist_gen(-max_dist / 2.0, max_dist / 2.0);
  static std::uniform_real_distribution<double> angle_gen(0.0, 2.0*M_PI);
  for (int i = 0; i < num_samples(); ++i) {
    double distance = dist_gen(mt);
    double angle = angle_gen(mt);
    positions(i, 0) = distance*cos(angle);
    positions(i, 1) = distance*sin(angle);
  }
}

void Numeric_pred_group::estimate_gaussian(int feat_i) {
  double average = arma::mean(x_->col(feat_i));
  double variance = arma::var(x_->col(feat_i));
  std::vector<std::string> functions;
  std::string function;
  function = "Gauss(x, " + std::to_string(average) + ", "
    + std::to_string(sqrt(variance)) + ")";
  functions.push_back(function);
  std::vector<std::string> extra_inputs;
  extra_inputs.push_back(
    "Gauss(x,mu,sigma) = 1./(sigma*sqrt(2*pi)) * exp( -(x-mu)**2 / (2*sigma**2) )");
  //plot_hist(feat_i, 10, functions, extra_inputs);

}

// Copy everything other than is_pca_pointer_
Numeric_pred_group& Numeric_pred_group::operator=( const Numeric_pred_group& other ) {
  x_ = other.x_;
  y_ = other.y_;
  y_map_ = other.y_map_;
  train_group_ = other.train_group_;
  test_group_ = other.test_group_;
  pca_var = other.pca_var;
  pca_set_ = other.pca_set_;
  pca_coeff_ =other.pca_coeff_;
  pca_x_ = other.pca_x_;
  pca_var_ = other.pca_var_;
  distances_ = other.distances_;
}
