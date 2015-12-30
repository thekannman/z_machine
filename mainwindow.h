
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

  QMenu *fileMenu;
  QMenu *plotMenu;
  QMenu *statMenu;
  QAction *openAction;
  QAction *plotHistAction;
  QAction *plotScatterAction;
  QAction *pcaAction;
  QMdiArea *mdiArea;
  QSignalMapper *windowMapper;

  QString curFile;

};

#endif // MAINWINDOW_H

