  #include "tablewidget.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>
#include <QHeaderView>
#include <QScrollBar>
#include <QString>
#include <QLabel>

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
  otherTable->pred_group_->pca_pointer() = pred_group_;
  pred_group_->is_pca_pointer() = true;
  pred_group_->pca_pointer() = pred_group_;
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
      item(i,j)->setBackground(Qt::white);
    }
  }
  setSpan(0,x.n_cols,y.size(),1);
  for (int i = 0, i_max = static_cast<int>(y.size()); i < i_max; ++i) {
    setItem(i, x.n_cols+1, new QTableWidgetItem(QString::number(y(i))));
    item(i,x.n_cols+1)->setBackground(Qt::white);
  }
}

void TableWidget::makeTree(std::shared_ptr<Tree> tree, bool test) {
  int total_width = std::pow(2,tree->total_depth()-1);
  setRowCount(tree->total_depth());
  setColumnCount(total_width);
  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::gray);
  setPalette(p);
  setShowGrid(false);
  resizeColumnsToContents();
  makeLeaves(tree, test, total_width/2, 0, 0, total_width);
}

void TableWidget::makeLeaves(std::shared_ptr<Tree> tree, bool test, int h_offset, int v_offset, int old_h_offset, int total_width) {
  makeLeaf(tree, test, h_offset, v_offset, old_h_offset);
  if (tree->left())
    makeLeaves(tree->left(), test, h_offset - total_width/std::pow(2,tree->depth()+1), v_offset + 1, h_offset, total_width);
  if (tree->right())
    makeLeaves(tree->right(), test, h_offset + total_width/std::pow(2,tree->depth()+1), v_offset + 1, h_offset, total_width);
}

void TableWidget::makeLeaf(std::shared_ptr<Tree> tree, bool test, double h_offset, double v_offset, double old_h_offset) {
  std::map<int, int> group_counts;
  std::shared_ptr<Numeric_pred_group> plot_group = test ? tree->test_group() : tree->pred_group();
  arma::uvec& items_contained = test ? tree->test_items_contained() : tree->items_contained();
  for (int i = 0, i_max = plot_group->num_categories(); i < i_max; ++i) {
    group_counts[i] = 0;
  }
  for (int i : items_contained) {
    group_counts[plot_group->y(i)]++;
  }
  std::string cell_contents;
  if (tree->split_variable() != -1)
  cell_contents += "Split on variable " + std::to_string(tree->split_variable()) + " at " + std::to_string(tree->split_value()) + ".\n";
  for (auto i : group_counts) {
    cell_contents += std::to_string(i.second) + "   ";
  }
  cell_contents.pop_back();
  cell_contents.pop_back();
  cell_contents.pop_back();
  setItem(v_offset, h_offset, new QTableWidgetItem(QString::fromStdString(cell_contents)));
  item(v_offset, h_offset)->setBackground(Qt::white);
  item(v_offset, h_offset)->setTextAlignment(Qt::AlignHCenter);
  setColumnWidth(h_offset, 124);
  if (v_offset!=0) {
    QPixmap pix;
    QSize zeroSize(100,100);
    if (h_offset > old_h_offset) {
      QLabel *lblTest = new QLabel;
      lblTest->setAlignment(Qt::AlignCenter);
      pix.load(":left_corner.png");
      lblTest->setPixmap(pix);
      setCellWidget(v_offset-1, h_offset, lblTest);
      for (int offset = h_offset - 1; offset > old_h_offset; --offset) {
        QLabel *lblTest = new QLabel;
        lblTest->setAlignment(Qt::AlignCenter);
        pix.load(":straight.png");
        lblTest->setPixmap(pix);
        setCellWidget(v_offset-1, offset, lblTest);
      }
    } else {
      QLabel *lblTest = new QLabel;
      lblTest->setAlignment(Qt::AlignCenter);
      pix.load(":right_corner.png");
      lblTest->setPixmap(pix);
      setCellWidget(v_offset-1, h_offset, lblTest);
      for (int offset = h_offset + 1; offset < old_h_offset; ++offset) {
        QLabel *lblTest = new QLabel;
        lblTest->setAlignment(Qt::AlignCenter);
        pix.load(":straight.png");
        lblTest->setPixmap(pix);
        setCellWidget(v_offset-1, offset, lblTest);
      }
    }
  }
}

void TableWidget::confusionMatrix(std::shared_ptr<Numeric_pred_group> pred_group, std::shared_ptr<arma::ivec> y_pred) {
  std::shared_ptr<arma::ivec> y_hat = pred_group->y_ptr();
  arma::mat conf_mat(pred_group->num_categories(),pred_group->num_categories(), arma::fill::zeros);
  QFont bold_font;
  bold_font.setBold(true);
  for (int i = 0; i < y_hat->size(); ++i) {
    conf_mat((*y_hat)(i), (*y_pred)(i)) += 1;
  }

  setRowCount(pred_group->num_categories()+2);
  setColumnCount(pred_group->num_categories()+2);
  for (int i = 0, i_max = pred_group->num_categories(); i < i_max; ++i) {
    setItem(i+2, 1, new QTableWidgetItem(QString::fromStdString(pred_group->category(i))));
    item(i+2, 1)->setFont(bold_font);
    item(i+2, 1)->setTextAlignment(Qt::AlignCenter);
    setItem(1, i+2, new QTableWidgetItem(QString::fromStdString(pred_group->category(i))));
    item(1, i+2)->setFont(bold_font);
    item(1, i+2)->setTextAlignment(Qt::AlignCenter);
    for (int j = 0, j_max = pred_group->num_categories(); j < j_max; ++j) {
      setItem(i+2, j+2, new QTableWidgetItem(QString::number(conf_mat(i,j))));
      item(i+2, j+2)->setTextAlignment(Qt::AlignCenter);
    }
  }

  setSpan(0,1,1,pred_group->num_categories()+1);
  setSpan(1,0,pred_group->num_categories()+1,1);
  setItem(1,0, new QTableWidgetItem(QString("Actual")));
  item(1, 0)->setFont(bold_font);
  item(1, 0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
  resizeColumnsToContents();
  setItem(0,1, new QTableWidgetItem(QString("Predicted")));
  item(0, 1)->setFont(bold_font);
  item(0, 1)->setTextAlignment(Qt::AlignCenter);

  setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

void TableWidget::classificationReport(std::shared_ptr<Numeric_pred_group> pred_group, std::shared_ptr<arma::ivec> y_pred) {
  std::shared_ptr<arma::ivec> y_hat = pred_group->y_ptr();
  arma::mat conf_mat(pred_group->num_categories(),pred_group->num_categories(), arma::fill::zeros);
  QFont bold_font;
  bold_font.setBold(true);
  for (int i = 0; i < y_hat->size(); ++i) {
    conf_mat((*y_hat)(i), (*y_pred)(i)) += 1;
  }
  arma::rowvec col_sums = sum(conf_mat);
  arma::vec row_sums = sum(conf_mat,1);
  setRowCount(pred_group->num_categories()+2);
  setColumnCount(5);
  setItem(0,0, new QTableWidgetItem(QString("Class")));
  item(0, 0)->setFont(bold_font);
  item(0, 0)->setTextAlignment(Qt::AlignCenter);
  setItem(0,1, new QTableWidgetItem(QString("Precision")));
  item(0, 1)->setFont(bold_font);
  item(0, 1)->setTextAlignment(Qt::AlignCenter);
  item(0, 1)->setToolTip("TP / (TP + FP)");
  setItem(0,2, new QTableWidgetItem(QString("Recall")));
  item(0, 2)->setFont(bold_font);
  item(0, 2)->setTextAlignment(Qt::AlignCenter);
  item(0, 2)->setToolTip("TP / (TP + FN)");
  setItem(0,3, new QTableWidgetItem(QString("F1-score")));
  item(0, 3)->setFont(bold_font);
  item(0, 3)->setTextAlignment(Qt::AlignCenter);
  item(0, 3)->setToolTip("2 * (precision * recall) / (precision + recall)");
  setItem(0,4, new QTableWidgetItem(QString("Support")));
  item(0, 4)->setFont(bold_font);
  item(0, 4)->setTextAlignment(Qt::AlignCenter);
   item(0, 4)->setToolTip("Count( y_true = y_i)");

  double precision_avg = 0.0, recall_avg = 0.0, f1_score_avg = 0.0;
  int support_sum = 0;
  for (int i = 0, i_max = pred_group->num_categories(); i < i_max; ++i) {
    setItem(i+1,0, new QTableWidgetItem(QString::fromStdString(pred_group->category(i))));
    item(i+1, 0)->setFont(bold_font);
    item(i+1, 0)->setTextAlignment(Qt::AlignCenter);
    double precision = conf_mat(i,i)/col_sums(i);
    setItem(i+1, 1, new QTableWidgetItem(QString::number(precision, 'g', 2)));
    item(i+1, 1)->setTextAlignment(Qt::AlignCenter);
    double recall = conf_mat(i,i)/row_sums(i);
    setItem(i+1, 2, new QTableWidgetItem(QString::number(recall, 'g', 2)));
    item(i+1, 2)->setTextAlignment(Qt::AlignCenter);
    double f1_score = precision+recall==0 ? 0 : 2.0*precision*recall/(precision+recall);
    setItem(i+1, 3, new QTableWidgetItem(QString::number(f1_score, 'g', 2)));
    item(i+1, 3)->setTextAlignment(Qt::AlignCenter);
    int support = row_sums(i);
    setItem(i+1, 4, new QTableWidgetItem(QString::number(support)));
    item(i+1, 4)->setTextAlignment(Qt::AlignCenter);
    precision_avg += precision*support;
    recall_avg += recall*support;
    f1_score_avg += f1_score*support;
    support_sum += support;
  }
  precision_avg /= support_sum;
  recall_avg /= support_sum;
  f1_score_avg /= support_sum;
  setItem(pred_group->num_categories()+1,0, new QTableWidgetItem(QString("Avg/total")));
  item(pred_group->num_categories()+1, 0)->setFont(bold_font);
  item(pred_group->num_categories()+1, 0)->setTextAlignment(Qt::AlignCenter);

  setItem(pred_group->num_categories()+1, 1, new QTableWidgetItem(QString::number(precision_avg, 'g', 2)));
  item(pred_group->num_categories()+1, 1)->setFont(bold_font);
  item(pred_group->num_categories()+1, 1)->setTextAlignment(Qt::AlignCenter);
  setItem(pred_group->num_categories()+1, 2, new QTableWidgetItem(QString::number(recall_avg, 'g', 2)));
  item(pred_group->num_categories()+1, 2)->setFont(bold_font);
  item(pred_group->num_categories()+1, 2)->setTextAlignment(Qt::AlignCenter);
  setItem(pred_group->num_categories()+1, 3, new QTableWidgetItem(QString::number(f1_score_avg, 'g', 2)));
  item(pred_group->num_categories()+1, 3)->setFont(bold_font);
  item(pred_group->num_categories()+1, 3)->setTextAlignment(Qt::AlignCenter);
  setItem(pred_group->num_categories()+1, 4, new QTableWidgetItem(QString::number(support_sum)));
  item(pred_group->num_categories()+1, 4)->setFont(bold_font);
  item(pred_group->num_categories()+1, 4)->setTextAlignment(Qt::AlignCenter);

  parentWidget()->resize(suggest_width(), suggest_height());
  setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

int TableWidget::suggest_width() {
  int suggested_width = 100.0;
  for (int i = 0; i < columnCount(); ++i) {
    suggested_width += columnWidth(i);
  }
  return std::min(suggested_width, parentWidget()->parentWidget()->width());
}

int TableWidget::suggest_height() {
  int suggested_height = 100.0;
  for (int i = 0; i < rowCount(); ++i) {
    suggested_height += rowHeight(i);
  }
  return std::min(suggested_height, parentWidget()->parentWidget()->height());
}
