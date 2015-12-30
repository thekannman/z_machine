#include "log_reg.h"

void Log_reg::train(Numeric_pred_group& train_group, double tol) {
  if (train_group.num_categories() == 2)
    train_binary(train_group, tol);
  else
    train_multiclass(train_group, tol);
}


void Log_reg::train_binary(Numeric_pred_group& train_group, double tol) {
  const int MAX_ITER = 1e8;
  train_group_ = std::make_shared<Numeric_pred_group>(train_group);
  int N = train_group.y().size();
  int P = train_group.x().n_cols;
  // P_1: Number of coefficients (variables + intercept)
  int P_1 = P + 1;
  beta_.set_size(P_1);
  beta_.zeros();  
  arma::vec beta_old;
  arma::mat x(N, P_1, arma::fill::ones);
  x.submat(0, 1, N-1, P) = train_group.x();
  auto y = train_group.y();
  
  arma::mat x_t = x.t();
  arma::mat bx;
  arma::vec p(N);
  // 0.5 only because beta = 0
  p.imbue([&]() { return 0.5; });
  arma::mat x_tilda;
  // 0.25 only because beta = 0
  x_tilda = 0.25*x; 
  beta_ += (x_t*x_tilda).i()*x_t*(y - p);
  for (int i = 0; i < MAX_ITER; ++i) {
    beta_old = beta_;
    for (int j = 0; j < N; ++j) {
      bx = beta_.t()*x_t.col(j);
      double exp_bx = std::exp(arma::as_scalar(bx));
      p(j) = exp_bx / (1.0 + exp_bx);
      x_tilda.row(j) = p(j)*(1.0 - p(j))*x.row(j);
    }
    beta_ += (x_t*x_tilda).i()*x_t*(y - p);
    beta_old = beta_ - beta_old;
    double iter_diff = arma::norm(beta_old);
    if (i % 1 == 0)
      std::cout << "Iteration " << i << " complete. Difference at " << iter_diff << std::endl;
    if (iter_diff < tol)
      break;
  }
  beta_.print();
  arma::vec test = (beta_.t()*x_t).t();
  arma::uvec predictions = (test >= 0);
  arma::uvec correct = (predictions == y);
  std::cout << arma::sum(correct)/static_cast<double>(correct.size()) << std::endl;
  getchar();
}

void Log_reg::train_multiclass(Numeric_pred_group& train_group, double tol) {
  std::ofstream test_file("test.txt");
  const int MAX_ITER = 1e8;
  double alpha = 1.0;
  train_group_ = std::make_shared<Numeric_pred_group>(train_group);
  int N = train_group.y().size();
  int K = train_group.num_categories();
  int P = train_group.x().n_cols;
  // P_1: Number of coefficients (variables + intercept)
  int P_1 = P + 1;
  beta_.set_size(P_1*(K-1));
  //beta_ << 848.4648 << -50.4198 << -16.82837 << -376.29272 << 30.77966 << -0.3187808
  //  << 115.4110 << -93.03559 << 358.9488 << 100.5827 << -24.09276 << 442.3062
  //  << -26.29891 << -0.5581035
  //  << -149.9645 << 61.7616 << -14.00633 << -93.30316 << 28.10216 << -4.2882308
  //  << 267.9405 << -256.98401 << -389.8206 << 151.4916 << 37.77146 << -220.3293
  //  << -166.22775 << -0.4108165;
  beta_.zeros();
  arma::vec beta_old;
  arma::mat x(N, P_1, arma::fill::ones);
  x.submat(0, 1, N - 1, P) = train_group.x();
  arma::ivec y(N*(K - 1));
  for (int i = 1; i < K; ++i) {
    y.subvec((i - 1)*N, i*N - 1) = arma::conv_to<arma::ivec>::from(train_group.y() == i);
  }
  arma::mat bx;
  arma::vec p(N*(K-1));
  arma::vec p_sum(N);
  arma::mat x_tilda(N*(K-1),P_1*(K-1),arma::fill::zeros);
  for (int i = 1; i < K; ++i) {
    x_tilda.submat((i - 1)*N, (i - 1)*P_1, i*N - 1, i*P_1 - 1) = x;
  }
  arma::mat x_tilda_t = x_tilda.t();
  arma::mat x_t = x.t();
  arma::mat W(N*(K - 1), N*(K - 1), arma::fill::zeros);
  arma::rowvec beta_sub_t;
  double iter_diff_old = std::numeric_limits<double>::max();
  for (int i = 0; i < MAX_ITER; ++i) {
    p_sum.ones();
    beta_old = beta_;
    for (int k = 0; k < K - 1; ++k) {
      int kN = k*N;
      beta_sub_t = beta_.subvec(k*P_1, (k + 1)*P_1 - 1).t();
      for (int j = 0; j < N; ++j) {
        bx = beta_sub_t*x_t.col(j);
        double exp_bx = std::exp(arma::as_scalar(bx));
        p(kN+j) = exp_bx;
        p_sum(j) += exp_bx;
      }
    }
    for (int k = 0; k < K - 1; ++k) {
      for (int j = 0; j < N; ++j) {
        p(k*N + j) /= p_sum(j);
      }
    }
    for (int k = 0; k < K - 1; ++k) {
      int kN = k*N;
      for (int j = k; j < K - 1; ++j) {
        int jN = j*N;
        if (j == k) {
          for (int i_diag = 0; i_diag < N; ++i_diag) {
            W(kN + i_diag, jN + i_diag) = p(kN + i_diag)*(1 - p(jN + i_diag));
          }
        } else {
          for (int i_diag = 0; i_diag < N; ++i_diag) {
            W(kN + i_diag, jN + i_diag) = W(jN + i_diag, kN + i_diag) = -p(kN + i_diag) * p(jN + i_diag);
          }
        }
      }
    }
    //reshape(y - p, N, K - 1).print();
    //getchar();
    beta_ += (x_tilda_t*W*x_tilda).i()*x_tilda_t*alpha*(y - p);
    beta_old = (beta_ - beta_old) / beta_old;
    
    double iter_diff = arma::norm(beta_old);
    if (i % 1 == 0  ) {
      std::cout << "Iteration " << i << " complete. Difference at " << iter_diff << std::endl;
    }
    if (iter_diff < tol)
      break;
    if (iter_diff > iter_diff_old) {
      alpha /= 2.0;
      std::cout << "Adjusting alpha to " << alpha << std::endl;
    }
    iter_diff_old = iter_diff;
  }
  arma::uvec p_elems(K-1);
  arma::vec p_sub;
  arma::uvec predictions(N);
  for (int i =  0; i < K - 1; ++i) {
    p_elems(i) = N*i;
  }
  for (int i = 0; i < N; ++i) {
    p_sub = p.elem(p_elems);
    double p_max;
    p_max = p.elem(p_elems).max(predictions(i));
    if (1.0 - arma::sum(p_sub) > p_max)
      predictions(i) = 0;
    else
      predictions(i) += 1;
    p_elems += 1;
    //std::cout << 1.0 - arma::sum(p_sub) << std::endl;
    //p_sub.print();
    //std::cout << std::endl;
  }
  //arma::reshape(beta_, P_1, K - 1).print();
  predictions.print();
  std::cout << std::endl;
  arma::uvec correct = (predictions == train_group.y());
  std::cout << arma::sum(correct) / static_cast<double>(correct.size()) << std::endl;
  getchar();
}
