#ifndef _PREDICTION_GROUP_H_
#define _PREDICTION_GROUP_H_

#include <string>

class Prediction_group {
 public:
  virtual int num_samples() const = 0;
  virtual int y(int i) const = 0;
  virtual bool read_csv(const std::string& filename, bool has_header = false) = 0;
};

#endif
