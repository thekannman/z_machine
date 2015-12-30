#pragma once
#include "point.h"
//#include "gnuplot-iostream.h"

class Line {
 public:
  Line(Point start, Point end) : start_(start), end_(end) {};
  Line(double x_min, double y_min, double x_max, double y_max)
    : start_(x_min, y_min), end_(x_max, y_max) {};
//  void draw(Gnuplot& gp) {
//    gp << "set arrow from " << start_.x() << "," << start_.y() << " to " << end_.x() << "," << end_.y() << " nohead\n";
//  }

 private:
   Point start_;
   Point end_;
};

