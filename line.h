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

#ifndef _LINE_H_
#define _LINE_H_

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

#endif // _LINE_H_
