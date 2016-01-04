#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QtSql>

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
    bool loadSite();
    QList<BTCexchange*> m_sites;
    void generateDB(QSqlQuery *query);

public slots:

private slots:

};

#endif // MAINWINDOW_H
