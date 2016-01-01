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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMdiArea>
#include <QSignalMapper>
#include "tablewidget.h"
#include "customplot.h"

#include <cstring>

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget *parent = 0);

 signals:

 public slots:

 private slots:
  void open();
  TableWidget *createMdiChild();
  void updateMenus();
  void setActiveSubWindow(QWidget *window);
  void plotHist();
  void plotScatter();
  void pca();
  void tree();
  void logReg();

 private:
  void createActions();
  void createMenus();
  bool okToContinue();
  bool openFile(const QString &fileName);
  bool loadFile(const QString &fileName);
  void setCurrentFile(const QString &fileName);
  QString strippedName(const QString &fullFileName);
  TableWidget *activeTable();
  CustomPlot *activePlot();
  QMdiSubWindow *findMdiChild(const QString &fileName);
  QMdiSubWindow *findMdiChild(const std::shared_ptr<Numeric_pred_group> target_group);

  QMenu *fileMenu;
  QMenu *plotMenu;
  QMenu *statMenu;

  QAction *openAction;
  QAction *plotHistAction;
  QAction *plotScatterAction;
  QAction *pcaAction;
  QAction *treeAction;
  QAction *logRegAction;

  QMdiArea *mdiArea;
  QSignalMapper *windowMapper;

  QString curFile;

};

#endif // _MAINWINDOW_H_

