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

#ifndef _LOG_REG_H_
#define _LOG_REG_H_

#include <QStatusBar>
#include "numeric_pred_group.h"

class Log_reg
{
 public:
  Log_reg(std::shared_ptr<Numeric_pred_group> train_group, QStatusBar* statusbar, double tol = 1.0e-4);
  void train(std::shared_ptr<Numeric_pred_group> train_group, QStatusBar* statusbar, double tol = 1.0e-4);
  void train_binary(std::shared_ptr<Numeric_pred_group>, QStatusBar* statusbar, double tol = 1.0e-4);
  void train_multiclass(std::shared_ptr<Numeric_pred_group>, QStatusBar* statusbar, double tol = 1.0e-4);
  std::shared_ptr<arma::ivec> y_pred() { return y_pred_; }

 private:
  arma::vec beta_;
  std::shared_ptr<arma::ivec> y_pred_;
  std::shared_ptr<Numeric_pred_group> train_group_;
};

#endif // _LOG_REG_H_
