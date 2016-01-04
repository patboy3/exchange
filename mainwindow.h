#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>


#include "btcexchange.h"
#include "coinbase.h"
#include "quadriga.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool createConnection();
    QList<BTCexchange*> site;

public slots:

private slots:

};

#endif // MAINWINDOW_H
