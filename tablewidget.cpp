#include "tablewidget.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>
#include <QHeaderView>
#include <QScrollBar>

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent) {
  setAttribute(Qt::WA_DeleteOnClose);
  isUntitled = true;
}

bool TableWidget::loadFile(const QString &fileName) {
  pred_group_ = std::make_shared<Numeric_pred_group>();

  if (!pred_group_->read_csv(fileName.toStdString())) {
    return false;
  }

  initTable(fileName);

  return true;
}

void TableWidget::makePcaTable(TableWidget* otherTable) {
  pred_group_ = std::make_shared<Numeric_pred_group>();
  if (!otherTable->pred_group_->pca_set())
    otherTable->pred_group_->pca();
  *pred_group_ = *(otherTable->pred_group_);
  pred_group_->is_pca_pointer() = true;
  initTable(otherTable->curFile);
}

bool TableWidget::initTable(const QString &fileName) {
  fillTable(pred_group_);
  QStringList headerList;
  for (int i = 1; i < columnCount()-1; ++i) {
    headerList << ("x" + std::to_string(i)).c_str();
  }
  headerList << "" << "y";
  setHorizontalHeaderLabels(headerList);
  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::gray);
  setPalette(p);
  double new_width = std::min(columnWidth(0)*(columnCount()+1), parentWidget()->parentWidget()->width());
  parentWidget()->resize(new_width, height());
  // TODO (Zak): This was an attempt to get the exact size, might come back to it later
  //parentWidget()->resize(columnWidth(0)*(pred_group_->num_features()+2)+verticalHeader()->width()+frameWidth()*2+verticalScrollBar()->sizeHint().width()+4, height());
  setCurrentFile(fileName);

  return true;
}

QString TableWidget::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void TableWidget::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString TableWidget::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void TableWidget::fillTable(std::shared_ptr<Numeric_pred_group> pred_group) {
  arma::mat x = pred_group->x();
  arma::ivec y = pred_group->y();
  setRowCount(pred_group->num_samples());
  setColumnCount(pred_group->num_features()+2);
  for (int i = 0, i_max = static_cast<int>(x.n_rows); i < i_max; ++i) {
    for (int j = 0, j_max = static_cast<int>(x.n_cols); j < j_max; ++j) {
      setItem(i, j, new QTableWidgetItem(QString::number(x(i,j))));
      item(i,j)->setBackgroundColor(Qt::white);
    }
  }
  setSpan(0,x.n_cols,y.size(),1);
  for (int i = 0, i_max = static_cast<int>(y.size()); i < i_max; ++i) {
    setItem(i, x.n_cols+1, new QTableWidgetItem(QString::number(y(i))));
    item(i,x.n_cols+1)->setBackgroundColor(Qt::white);
  }
}
