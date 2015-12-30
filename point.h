#pragma once
class Point
{
 public:
  Point() : x_(0), y_(0) {};
  Point(double x, double y) : x_(x), y_(y) {};
  double x() { return x_; }
  double y() { return y_; }

 private:
   double x_;
   double y_;
};

