#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>
//

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    createConnection();
}

bool MainWindow::createConnection()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName( "./exchange.db");

    /**
     * si db pas créé... faudrait la créé
     *
      CREATE TABLE exchange (
      ID INT PRIMARY KEY      NOT NULL,
      ID_Currency INT(11) NOT NULL,
      Nom VARCHAR(255) NOT NULL,
      apiKey VARCHAR(255) NOT NULL,
      secretKey VARCHAR(255) NOT NULL,
      CONSTRAINT FK_exchange_currency_ID FOREIGN KEY (ID_Currency)
        REFERENCES currency(ID) ON DELETE RESTRICT ON UPDATE RESTRICT
    );

    CREATE TABLE currency(
       ID INT PRIMARY KEY      NOT NULL,
       currency           CHAR(50) NOT NULL
    );



     *
     */

    if (!db.open()) {
        //si connect pas... add les site poru faire seulement un test direct

        qDebug() << "erreur de connexion :" << db.lastError();
        BTCexchange *test = new CoinBase("CAD", "SBH5GeIntChyxpax","G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T");
        //test->rafraichirOrderBook();

        BTCexchange *test2 = new Quadriga("CAD","","");
        //test2->rafraichirOrderBook();

        return false;
    }
    else
    {
        //Si sa connect... rentre les site ds une list de site
        qDebug() << "yeahhhhh";

        QSqlQuery query;
        query.exec("SELECT Nom, currency.currency, apiKey, secretKey FROM exchange left join currency on currency.ID = exchange.ID_Currency;");

        while (query.next()) {
            if (query.value(0).toString() == "quadriga")
                m_sites.append(new Quadriga(query.value(1).toString(),query.value(2).toString(),query.value(3).toString()));
            else if (query.value(0).toString() == "coinbase")
                m_sites.append(new CoinBase(query.value(1).toString(),query.value(2).toString(),query.value(3).toString()));
        }

        foreach (BTCexchange* solo, m_sites)
        {
            solo->rafraichirOrderBook();
        }

        return false;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    foreach (BTCexchange* solo, m_sites)
    {
        delete solo;
    }
}
