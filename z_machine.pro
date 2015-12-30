TEMPLATE = app

CONFIG += c++11

QT += widgets printsupport

SOURCES += main.cpp \
    mainwindow.cpp \
    numeric_pred_group.cpp \
    line.cpp \
    log_reg.cpp \
    point.cpp \
    prediction_group.cpp \
    z_string.cpp \
    tree.cpp \
    tablewidget.cpp \
    qcustomplot.cpp \
    customplot.cpp \
    histdialog.cpp \
    scatterdialog.cpp

RESOURCES += qml.qrc

HEADERS += mainwindow.h \
    numeric_pred_group.h \
    line.h \
    log_reg.h \
    point.h \
    prediction_group.h \
    z_string.h \
    tree.h \
    tablewidget.h \
    qcustomplot.h \
    customplot.h \
    histdialog.h \
    scatterdialog.h

INCLUDEPATH += C:\\armadillo-6.200.5\\include

LIBS += -L"C:\\Users\\zak\\Documents\\Visual Studio 2015\\Libraries" \
        -llapack_win32_MTd \
        -lblas_win32_MTd

FORMS += \
    histdialog.ui \
    scatterdialog.ui
