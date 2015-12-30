#ifndef SCATTERDIALOG_H
#define SCATTERDIALOG_H

#include <QDialog>
#include "tablewidget.h"

namespace Ui {
class ScatterDialog;
}

class ScatterDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ScatterDialog(TableWidget* plotData, QWidget *parent = 0);
  ~ScatterDialog();

  static std::vector<int> getSelections(TableWidget* plotData, QWidget *parent = 0);

private:
  Ui::ScatterDialog *ui;
};

#endif // SCATTERDIALOG_H
