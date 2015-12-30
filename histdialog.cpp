#include "histdialog.h"
#include "ui_histdialog.h"

HistDialog::HistDialog(TableWidget* plotData, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HistDialog)
{
  ui->setupUi(this);
  for (int i = 0; i < plotData->columnCount()-2; ++i) {
    ui->comboBox->addItem(plotData->horizontalHeaderItem(i)->text());
  }
}

HistDialog::~HistDialog()
{
  delete ui;
}

std::vector<int> HistDialog::getSelections(TableWidget* plotData, QWidget *parent) {
  HistDialog *histDlg = new HistDialog(plotData, parent);
  std::vector<int> selections;
  if ( histDlg->exec() == QDialog::Accepted ) {
    selections.push_back(histDlg->ui->comboBox->currentIndex());
    selections.push_back(histDlg->ui->checkBox->checkState()==Qt::Checked);
  }
  delete histDlg;
  return selections;
}
