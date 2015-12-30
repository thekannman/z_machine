#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include <QObject>
#include "qcustomplot.h"

class CustomPlot : public QCustomPlot {
  Q_OBJECT
 public:
  CustomPlot(QWidget *parent = 0);

 signals:

 public slots:
};

#endif // CUSTOMPLOT_H
