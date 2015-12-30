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
//  void plot_pca(int feat_x, int feat_y, std::vector<double> h_lines = {}, std::vector<double> v_lines = {});
//  void plot_pca_setup(int feat_x, int feat_y, Gnuplot& gp);
//  void Numeric_pred_group::plot_pca_xy(int feat_x, int feat_y, Gnuplot& gp);
  void euclidean_distance(bool scale = false, std::shared_ptr<arma::mat> distances = nullptr);
  void plot_distances();
  void grab_hist(int feat_i, QVector<double>& keys, QVector<double>& values, double& width, int num_bins = 10);
  void grab_group_hist(int feat_i, QVector<QVector<double> >& keys, QVector<QVector<double> >& values, double& width, int num_bins = 10);
  void estimate_gaussian(int feat_i);
  Numeric_pred_group& operator=( const Numeric_pred_group& other );
  bool& pca_set() { return pca_set_; }

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

  void randomize_positions(arma::mat& positions);
};

#endif
