#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>

//

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    //add les sites ds m_sites
    loadSite();
}

void MainWindow::loadSite()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName( "./exchange.db");

    if (!db.open()) {
        //si connect pas... add les site poru faire seulement un test direct

        qDebug() << "erreur de connexion :" << db.lastError();
        BTCexchange *test = new CoinBase("CAD", "SBH5GeIntChyxpax","G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T");
        //test->rafraichirOrderBook();

        BTCexchange *test2 = new Quadriga("CAD","","",1);
        //test2->rafraichirOrderBook();


    }
    else
    {
        //Si sa connect... rentre les site ds une list de site
        qDebug() << "yeahhhhh";

        //detect si ya des tables
        bool tabledetection(false);
        QSqlQuery query;
        query.exec("select * from SQLite_master;");

        while (query.next() && !tabledetection)
        {
            //detect si ya déja des table ds la db.. (si la db est pas neuve)
            tabledetection = true;
        }

        if (!tabledetection) //si db neuve.. créé les tables !
            generateDB(&query);

        query.exec("SELECT sites.sitename, currency.currency, apiKey, secretKey, sites.ident FROM exchange left join currency on currency.ID = exchange.ID_Currency left outer join sites on sites.ID = exchange.ID_Sitename;");

        while (query.next()) {
            if (query.value(0).toString() == "quadriga")
                m_sites.append(new Quadriga(query.value(1).toString(),query.value(2).toString(),query.value(3).toString(),query.value(4).toInt()));
            else if (query.value(0).toString() == "coinbase")
                m_sites.append(new CoinBase(query.value(1).toString(),query.value(2).toString(),query.value(3).toString()));
        }

        foreach (BTCexchange* solo, m_sites)
        {
            //solo->viewOpenOrder();
            //solo->rafraichirOrderBook();
            solo->loadBalance();
            //solo->sellOrder(0.005);
            //solo->buyOrder(0.005,572);
            //solo->cancelOrder("i1xgbuxud9qikqod8rbt065gp6i7of07gxnrskm6ucoe3csc3354abkzh276h8dy");
            //solo->lookOrder("vsvzw7e67s3kio5izcffecg3em3x1gax86x5dcck4f5n4a54nlj1xz5bm89oojlx");
        }
    }
}

void MainWindow::generateDB(QSqlQuery *query)
{
    //créé les tables
    query->exec("CREATE TABLE `currency` ( `ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,`currency`	CHAR(50) NOT NULL);");
    query->exec("CREATE TABLE exchange (ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, ID_Currency INT(11) NOT NULL, ID_Sitename INT(11) NOT NULL, apiKey VARCHAR(255) DEFAULT NULL, secretKey VARCHAR(255) DEFAULT NULL, CONSTRAINT FK_exchange_currency_ID FOREIGN KEY (ID_Currency) REFERENCES currency(ID) ON DELETE RESTRICT ON UPDATE RESTRICT, CONSTRAINT FK_exchange_sites_ID FOREIGN KEY (ID_Sitename) REFERENCES sites(ID) ON DELETE RESTRICT ON UPDATE RESTRICT);");
    query->exec("CREATE TABLE `sites` ( `ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `sitename`	TEXT NOT NULL, `ident`	INTEGER);");

    //add 3 currency de bases
    query->exec("INSERT INTO sites (sitename) VALUES ('quadriga'), ('coinbase');");
    query->exec("INSERT INTO currency (currency) VALUES ('CAD'), ('USD'), ('EURO');");
    query->exec("INSERT INTO exchange (ID_Currency, ID_Sitename) VALUES ( (SELECT ID from currency WHERE currency='CAD'),     (SELECT ID from sites WHERE sitename='coinbase') ), ( (SELECT ID from currency WHERE currency='CAD'),     (SELECT ID from sites WHERE sitename='quadriga') );");
}

MainWindow::~MainWindow()
{
    delete ui;
    foreach (BTCexchange* solo, m_sites)
    {
        delete solo;
    }
}
