#pragma once
#include "numeric_pred_group.h"

class Log_reg
{
 public:
  void train(Numeric_pred_group& train_group, double tol = 1.0e-4);
  void train_binary(Numeric_pred_group& train_group, double tol = 1.0e-3);
  void train_multiclass(Numeric_pred_group& train_group, double tol = 1.0e-3);

 private:
  arma::vec beta_;
  std::shared_ptr<Numeric_pred_group> train_group_;
};

