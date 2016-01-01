#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QMenuBar>
#include <QStringList>
#include <QMdiSubWindow>
#include <QScrollBar>
#include <QList>
#include <QVector>
#include <QBrush>
#include "histdialog.h"
#include "scatterdialog.h"
#include "tree.h"
#include "log_reg.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setCentralWidget(mdiArea);
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
          this, SLOT(updateMenus()));
  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget*)),
          this, SLOT(setActiveSubWindow(QWidget*)));

  createActions();
  createMenus();
  updateMenus();
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);

    plotMenu = menuBar()->addMenu(tr("&Plot"));
    plotMenu->addAction(plotHistAction);
    plotMenu->addAction(plotScatterAction);

    statMenu = menuBar()->addMenu(tr("&Statistics"));
    statMenu->addAction(pcaAction);
    statMenu->addAction(treeAction);
    statMenu->addAction(logRegAction);
}

void MainWindow::createActions() {
    openAction = new QAction(tr("&Open..."), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing csv file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    plotHistAction = new QAction(tr("&Histogram..."), this);
    plotHistAction->setStatusTip(tr("Plot a histogram"));
    connect(plotHistAction, SIGNAL(triggered()), this, SLOT(plotHist()));

    plotScatterAction = new QAction(tr("&Scatter..."), this);
    plotScatterAction->setStatusTip(tr("Plot a scatter plot"));
    connect(plotScatterAction, SIGNAL(triggered()), this, SLOT(plotScatter()));

    pcaAction = new QAction(tr("&PCA..."), this);
    pcaAction->setStatusTip(tr("Perform a principal component analysis"));
    connect(pcaAction, SIGNAL(triggered()), this, SLOT(pca()));

    treeAction = new QAction(tr("&Decision tree..."), this);
    treeAction->setStatusTip(tr("Make a decision tree from the data"));
    connect(treeAction, SIGNAL(triggered()), this, SLOT(tree()));

    logRegAction = new QAction(tr("&Logistic regression..."), this);
    logRegAction->setStatusTip(tr("Perform logistic regression on the data"));
    connect(logRegAction, SIGNAL(triggered()), this, SLOT(logReg()));
}

void MainWindow::open() {
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open csv file"), ".",
                                                  tr("csv files (*.csv)"));
  if (!fileName.isEmpty()) {
      QMdiSubWindow *existing = findMdiChild(fileName);
      if (existing) {
          mdiArea->setActiveSubWindow(existing);
          return;
      }

      if (openFile(fileName))
          statusBar()->showMessage(tr("File loaded"), 2000);
  }
}

bool MainWindow::openFile(const QString &fileName) {
  TableWidget *child = createMdiChild();
  const bool succeeded = child->loadFile(fileName);
  if (succeeded)
    child->show();
  else
    child->close();
  return succeeded;
}

bool MainWindow::okToContinue()
{
  /*
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Spreadsheet"),
                        tr("The document has been modified.\n"
                           "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::No
                        | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
  */
  return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = tr("Untitled");
    if (!curFile.isEmpty()) {
        shownName = strippedName(curFile);
        //recentFiles.removeAll(curFile);
        //recentFiles.prepend(curFile);
        //updateRecentFileActions();
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("Spreadsheet")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


TableWidget *MainWindow::createMdiChild()
{
    TableWidget *child = new TableWidget;
    mdiArea->addSubWindow(child);

    return child;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        TableWidget *mdiChild = qobject_cast<TableWidget *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const std::shared_ptr<Numeric_pred_group> target_group)
{
    if (target_group == nullptr)
      return nullptr;
    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        TableWidget *mdiChild = qobject_cast<TableWidget *>(window->widget());
        if (mdiChild->pred_group_ == target_group)
            return window;
    }
    return nullptr;
}

void MainWindow::updateMenus() {
  bool childIsTable = (activeTable() != 0);
  plotMenu->setEnabled(childIsTable);
  statMenu->setEnabled(childIsTable);
  //saveAct->setEnabled(hasMdiChild);
  //saveAsAct->setEnabled(hasMdiChild);
  //closeAct->setEnabled( hasMdiChild);
  //closeAllAct->setEnabled(hasMdiChild);
  //tileAct->setEnabled(hasMdiChild);
  //cascadeAct->setEnabled(hasMdiChild);
  //nextAct->setEnabled(hasMdiChild);
  //previousAct->setEnabled(hasMdiChild);
  //separatorAct->setVisible(hasMdiChild);
}

TableWidget *MainWindow::activeTable() {
  if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    return qobject_cast<TableWidget *>(activeSubWindow->widget());
  return 0;
}

CustomPlot *MainWindow::activePlot() {
  if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    return qobject_cast<CustomPlot *>(activeSubWindow->widget());
  return 0;
}

void MainWindow::setActiveSubWindow(QWidget *window) {
  if (!window)
    return;
  mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::plotHist() {
  TableWidget* activeTbl = activeTable();
  std::vector<int> selections = HistDialog::getSelections(activeTbl, this);
  if (selections.empty())
    return;
  CustomPlot *customPlot = new CustomPlot;
  double width;
  if (!selections[1]) {
    QCPBars *hist = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(hist);
    QVector<double> keys, values;

    activeTbl->pred_group_->grab_hist(selections[0], keys, values, width);
    customPlot->xAxis->setRange(keys.first()-width, keys.last()+width);
    customPlot->yAxis->setRange(0.0, 1.0);
    hist->setData(keys, values);
    hist->setWidth(width);
  } else {
    std::vector<QCPBars*> hists;
    hists.resize(activeTbl->pred_group_->num_categories());
    for (auto& hist : hists) {
      hist = new QCPBars(customPlot->xAxis, customPlot->yAxis);
      customPlot->addPlottable(hist);
    }
    std::vector<QColor> colors;
    colors.push_back(QColor(255,0,0,127));
    colors.push_back(QColor(0,255,0,127));
    colors.push_back(QColor(0,0,255,127));
    colors.push_back(QColor(255,255,0,127));
    colors.push_back(QColor(255,0,255,127));
    colors.push_back(QColor(0,255,255,127));
    colors.push_back(QColor(0,0,0,127));
    QVector<QVector<double> > keys, values;
    activeTbl->pred_group_->grab_group_hist(selections[0], keys, values, width);
    customPlot->xAxis->setRange(keys.first().first()-width, keys.first().last()+width);
    customPlot->yAxis->setRange(0.0, 1.0);
    for (int i = 0, i_max = static_cast<int>(hists.size()); i < i_max; ++i) {
      hists[i]->setData(keys[i], values[i]);
      if (i!=0) {
        hists[i]->moveAbove(hists[i-1]);
      }
      hists[i]->setBrush(colors[i]);
      hists[i]->setPen(QColor(0,0,0));
      hists[i]->setWidth(width);
    }
  }
  mdiArea->addSubWindow(customPlot);
  customPlot->xAxis->setLabel(activeTbl->horizontalHeaderItem(selections[0])->text());
  customPlot->parentWidget()->resize(500.0, 400.0);
  customPlot->show();
}

void MainWindow::plotScatter() {
  TableWidget* activeTbl = activeTable();
  std::vector<int> selections = ScatterDialog::getSelections(activeTbl, this);
  if (selections.empty())
    return;
  CustomPlot *customPlot = new CustomPlot;
  auto x = activeTbl->pred_group_->feature(selections[0]);
  auto y = activeTbl->pred_group_->feature(selections[1]);
  double x_min = arma::min(x);
  double x_max = arma::max(x);
  double y_min = arma::min(y);
  double y_max = arma::max(y);
  double graph_x_min = x_min - (x_max-x_min)*0.1;
  double graph_x_max = x_max + (x_max-x_min)*0.1;
  double graph_y_min = y_min - (y_max-y_min)*0.1;
  double graph_y_max = y_max + (y_max-y_min)*0.1;
  customPlot->xAxis->setRange(graph_x_min, graph_x_max);
  customPlot->yAxis->setRange(graph_y_min, graph_y_max);
  if (!selections[2]) {
    QCPGraph *scatter = new QCPGraph(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(scatter);
    scatter->setData(QVector<double>::fromStdVector(arma::conv_to<std::vector<double> >::from(x)),
                     QVector<double>::fromStdVector(arma::conv_to<std::vector<double> >::from(y)));
    scatter->setLineStyle(QCPGraph::lsNone);
    scatter->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
  } else {
    std::vector<QCPGraph*> scatter(activeTbl->pred_group_->num_categories());
    for (auto& i : scatter) {
      i = new QCPGraph(customPlot->xAxis, customPlot->yAxis);
      customPlot->addPlottable(i);
    }

    arma::vec x_sub, y_sub;
    arma::uvec samples(activeTbl->pred_group_->num_samples());
    int count = 0;
    std::vector<QColor> colors;
    colors.push_back(QColor(255,0,0,127));
    colors.push_back(QColor(0,255,0,127));
    colors.push_back(QColor(0,0,255,127));
    colors.push_back(QColor(255,255,0,127));
    colors.push_back(QColor(255,0,255,127));
    colors.push_back(QColor(0,255,255,127));
    colors.push_back(QColor(0,0,0,127));
    for (int i_cat = 0; i_cat < activeTbl->pred_group_->num_categories(); ++i_cat) {
      for (int i = 0, i_max = static_cast<int>(activeTbl->pred_group_->num_samples()); i < i_max; ++i) {
        std::cout << i_cat << " " << y(i) << std::endl;
        if (activeTbl->pred_group_->y(i) == i_cat) {
          samples(count) = i;
          ++count;
        }
      }
      samples.resize(count);
      x_sub = x.elem(samples);
      y_sub = y.elem(samples);
      samples = arma::zeros<arma::uvec>(activeTbl->pred_group_->num_samples());
      count = 0;
      scatter[i_cat]->setData(QVector<double>::fromStdVector(arma::conv_to<std::vector<double> >::from(x_sub)),
                          QVector<double>::fromStdVector(arma::conv_to<std::vector<double> >::from(y_sub)));
      scatter[i_cat]->setLineStyle(QCPGraph::lsNone);
      scatter[i_cat]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
      scatter[i_cat]->setPen(colors[i_cat]);
    }
  }
  mdiArea->addSubWindow(customPlot);
  customPlot->xAxis->setLabel(activeTbl->horizontalHeaderItem(selections[0])->text());
  customPlot->yAxis->setLabel(activeTbl->horizontalHeaderItem(selections[1])->text());
  customPlot->parentWidget()->resize(500.0, 400.0);
  customPlot->show();
}

void MainWindow::pca() {
  TableWidget* activeTbl = activeTable();
  QMdiSubWindow *existing = findMdiChild(activeTbl->pred_group_->pca_pointer());
  if (existing) {
    existing->show();
    return;
  }
  TableWidget *child = createMdiChild();
  child->makePcaTable(activeTbl);
  child->show();
}

void MainWindow::tree() {
  TableWidget* activeTbl = activeTable();
  TableWidget *child = createMdiChild();
  std::shared_ptr<Tree> decisionTree = std::make_shared<Tree>(activeTbl->pred_group_);
  decisionTree->find_splits();
  child->makeTree(decisionTree, false);
  child->show();
}

void MainWindow::logReg() {
  TableWidget* activeTbl = activeTable();
  TableWidget *child = createMdiChild();
  std::shared_ptr<Log_reg> log_reg = std::make_shared<Log_reg>(activeTbl->pred_group_, statusBar());
  child->classificationReport(activeTbl->pred_group_,log_reg->y_pred());
  child->show();
}
