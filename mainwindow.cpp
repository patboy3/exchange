#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>



/*
 DATABASE HOST : charest.xyz:3390
 DATABASE LOGIN : exchange
 DATABASE PASS : M8CxS'*\)jPp7yL>
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    createConnection();
    // Crée un test
    BTCexchange *test = new CoinBase("CAD", "SBH5GeIntChyxpax","G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T");
    //test->rafraichirOrderBook();

    BTCexchange *test2 = new Quadriga("CAD","","");
    //test2->rafraichirOrderBook();
}

bool MainWindow::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("charest.xyz");
    db.setPort(3310);
    db.setDatabaseName("btcexchange");
    db.setUserName("exchange");
    db.setPassword("M8CxS\'*\\)jPp7yL>");
    if (!db.open()) {
        qDebug() << "erreur de connexion";
        return false;
    }
    else
    {
        qDebug() << "yeahhhhh";
        return false;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
