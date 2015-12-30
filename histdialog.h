#ifndef HISTDIALOG_H
#define HISTDIALOG_H

#include <QDialog>
#include "tablewidget.h"

namespace Ui {
class HistDialog;
}

class HistDialog : public QDialog
{
  Q_OBJECT

public:
  explicit HistDialog(TableWidget* plotData, QWidget *parent = 0);

  ~HistDialog();

  static std::vector<int> getSelections(TableWidget* plotData, QWidget *parent = 0);

private:
  Ui::HistDialog *ui;
};

#endif // HISTDIALOG_H
