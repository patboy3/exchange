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

        BTCexchange *test = new CoinBase("CAD", "SBH5GeIntChyxpax","G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T");
        test->rafraichirOrderBook();

        BTCexchange *test2 = new Quadriga("CAD","","");
        test2->rafraichirOrderBook();

        return false;
    }
    else
    {
        qDebug() << "yeahhhhh";

        QSqlQuery query;
        query.exec("SELECT Nom, currency.currency, apiKey, secretKey FROM exchange left join currency on currency.ID = exchange.ID_Currency");

        while (query.next()) {
            /*QString name = query.value(0).toString();
            qDebug() << "name:" << query.value(0).toString() << " - " << query.value(1).toString();*/


            if (query.value(0).toString() == "quadriga")
                site.append(new Quadriga(query.value(1).toString(),query.value(2).toString(),query.value(3).toString()));
            else if (query.value(0).toString() == "coinbase")
                site.append(new CoinBase(query.value(1).toString(),query.value(2).toString(),query.value(3).toString()));

        }

        foreach (BTCexchange* solo, site)
        {
            solo->rafraichirOrderBook();
        }

        return false;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
