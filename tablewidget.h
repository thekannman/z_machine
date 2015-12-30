#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QObject>
#include <QTableWidget>
#include <armadillo>

#include "numeric_pred_group.h"

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

#endif // TABLEWIDGET_H
