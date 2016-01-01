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

#ifndef _NUMERIC_PRED_GROUP_H_
#define _NUMERIC_PRED_GROUP_H_

#include "prediction_group.h"
#include <map>
#include <armadillo>
#include "z_string.h"
#include "point.h"
#include "line.h"
#include <QVector>
#include <memory>

class Numeric_pred_group : public Prediction_group {
 public:
  Numeric_pred_group() {}
  Numeric_pred_group(const std::string& filename) {
    read_csv(filename);
  }
  Numeric_pred_group(arma::mat& x) : x_(&x) {}
  Numeric_pred_group(arma::mat& x, arma::ivec& y, std::shared_ptr<std::map<std::string, int> > y_map)
    : x_(&x), y_(&y), y_map_(y_map) {
  }
  virtual inline int num_samples() const { return x().n_rows; }
  inline int num_features() const { return x().n_cols; }
  inline int num_categories() const { return y_map_->size(); }
  virtual inline int y(int i) const { return (*y_)(i); }
  inline arma::vec feature(int i) const { return x().col(i); }
  inline arma::rowvec sample(int i) const { return x().row(i); }
  arma::mat& x() const { return is_pca_pointer_ ? *pca_x_ : *x_; }
  arma::ivec& y() const { return *y_; }
  std::shared_ptr<arma::ivec> y_ptr() const { return y_; }
  std::string y_string(int i);
  std::string category(int i);
  std::vector<std::string> y_strings();
  void find_uniques(int feature_index, arma::vec& unique_values, arma::uvec* include = nullptr);
  void find_splits(int feature_index, arma::vec& split_points, arma::uvec* include = nullptr);
  void training_split(double training_fraction = 0.6);
  std::shared_ptr<Numeric_pred_group> grab_training(double training_fraction = 0.6);
  std::shared_ptr<Numeric_pred_group> grab_testing(double training_fraction = 0.6);
  virtual bool read_csv(const std::string& filename, bool has_header = false);
  void pca();
  void euclidean_distance(bool scale = false, std::shared_ptr<arma::mat> distances = nullptr);
  void plot_distances();
  void grab_hist(int feat_i, QVector<double>& keys, QVector<double>& values, double& width, int num_bins = 10);
  void grab_group_hist(int feat_i, QVector<QVector<double> >& keys, QVector<QVector<double> >& values, double& width, int num_bins = 10);
  void estimate_gaussian(int feat_i);
  Numeric_pred_group& operator=( const Numeric_pred_group& other );
  bool& pca_set() { return pca_set_; }
  std::shared_ptr<Numeric_pred_group>& pca_pointer() { return pca_pointer_; }

  bool& is_pca_pointer() { return is_pca_pointer_; }
 private:
  std::shared_ptr<arma::mat> x_;
  std::shared_ptr<arma::ivec> y_;
  std::shared_ptr<std::map<std::string, int> > y_map_;
  std::shared_ptr<Numeric_pred_group> train_group_;
  std::shared_ptr<Numeric_pred_group> test_group_;
  arma::vec pca_var;
  bool pca_set_ = false;
  arma::mat pca_coeff_;
  std::shared_ptr<arma::mat> pca_x_;
  arma::vec pca_var_;
  std::shared_ptr<arma::mat> distances_;
  bool is_pca_pointer_ = false;
  std::shared_ptr<Numeric_pred_group> pca_pointer_;

  void randomize_positions(arma::mat& positions);
};

#endif // _NUMERIC_PRED_GROUP_H_
