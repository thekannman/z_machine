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

#ifndef _TABLEWIDGET_H_
#define _TABLEWIDGET_H_

#include <QObject>
#include <QTableWidget>
#include <armadillo>

#include "numeric_pred_group.h"
#include "tree.h"

class TableWidget : public QTableWidget {
  Q_OBJECT

 public:
  TableWidget(QWidget *parent = 0);

  //void newFile();
  bool loadFile(const QString &fileName);
  void makePcaTable(TableWidget* otherTable);
  bool initTable(const QString &fileName);
  //bool save();
  //bool saveAs();
  //bool saveFile(const QString &fileName);
  QString userFriendlyCurrentFile();
  QString currentFile() { return curFile; }
  void fillTable(std::shared_ptr<Numeric_pred_group> pred_group);
  void makeTree(std::shared_ptr<Tree> tree, bool test);
  void makeLeaves(std::shared_ptr<Tree> tree, bool test, int h_offset, int v_offset, int old_h_offset, int total_width);
  void makeLeaf(std::shared_ptr<Tree> tree, bool test, double h_offset, double v_offset, double old_h_offset);
  void confusionMatrix(std::shared_ptr<Numeric_pred_group> pred_group, std::shared_ptr<arma::ivec> y_pred);
  void classificationReport(std::shared_ptr<Numeric_pred_group> pred_group, std::shared_ptr<arma::ivec> y_pred);
  int suggest_width();
  int suggest_height();
  std::shared_ptr<Numeric_pred_group> pred_group_;

 signals:

 public slots:

 private:
  //bool maybeSave();
  void setCurrentFile(const QString &fileName);
  QString strippedName(const QString &fullFileName);

  QString curFile;
  bool isUntitled;
};

#endif // _TABLEWIDGET_H
