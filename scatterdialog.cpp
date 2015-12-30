#include "scatterdialog.h"
#include "ui_scatterdialog.h"

ScatterDialog::ScatterDialog(TableWidget* plotData, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ScatterDialog)
{
  ui->setupUi(this);
  for (int i = 0; i < plotData->columnCount()-2; ++i) {
    ui->xComboBox->addItem(plotData->horizontalHeaderItem(i)->text());
    ui->yComboBox->addItem(plotData->horizontalHeaderItem(i)->text());
  }
}

ScatterDialog::~ScatterDialog()
{
  delete ui;
}

std::vector<int> ScatterDialog::getSelections(TableWidget* plotData, QWidget *parent) {
  ScatterDialog *scatterDlg = new ScatterDialog(plotData, parent);
  std::vector<int> selections;
  if ( scatterDlg->exec() == QDialog::Accepted ) {
    selections.push_back(scatterDlg->ui->xComboBox->currentIndex());
    selections.push_back(scatterDlg->ui->yComboBox->currentIndex());
    selections.push_back(scatterDlg->ui->checkBox->checkState()==Qt::Checked);
  }
  delete scatterDlg;
  return selections;
}
