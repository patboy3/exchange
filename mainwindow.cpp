#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>


//



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);


    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName( "./exchange.db");
    m_db.open();


    //add les sites ds m_sites
    loadSite(); //load les site ds la db et rempli les balances

    m_trade = new Trade(&m_sites, m_query);

    //Pouvoir updater les textbox a partir de Trade
    connect(m_trade, SIGNAL(updateNeg()), this, SLOT(updateNeg()));
    connect(m_trade, SIGNAL(updatePos()), this, SLOT(updatePos()));
    connect(m_trade, SIGNAL(updateFunds()), this, SLOT(updateFunds()));

    //start Trade
    //Check les probabilitées
    //m_trade->start();
}


void MainWindow::loadSite()
{


    //Si sa connect... rentre les site ds une list de site

    //detect si ya des tables
    bool tabledetection = false;
    m_query = new QSqlQuery;

   // m_query->exec("INSERT INTO trade (ID_Buy, ID_Sell, Profitability) VALUES (1, 2, -1.4870);");

    m_query->exec("select * from SQLite_master;");



    while (m_query->next() && !tabledetection)
    {
        //detect si ya déja des table ds la db.. (si la db est pas neuve)
        tabledetection = true;
    }

    if (!tabledetection) //si db neuve.. créé les tables !
        generateDB(m_query);

    m_query->exec("SELECT sites.sitename, currency.currency, sites.apiKey, sites.secretKey, sites.ident, sites.passphrase FROM exchange left join currency on currency.ID = exchange.ID_Currency left outer join sites on sites.ID = exchange.ID_Sitename;");

    while (m_query->next()) {
        if (m_query->value(0).toString() == "quadriga")
        {
            //m_sites.append(new Quadriga(m_query->value(1).toString(),m_query->value(2).toString(),m_query->value(3).toString(),m_query->value(4).toInt(), m_query));
        }
        else if (m_query->value(0).toString() == "coinbase")
        {
            //m_sites.append(new CoinBase(m_query->value(1).toString(),m_query->value(2).toString(),m_query->value(3).toString(), m_query->value(5).toString(), m_query));
        }
        else if (m_query->value(0).toString() == "kraken")
        {
            m_sites.append(new Kraken(m_query->value(1).toString(),m_query->value(2).toString(),m_query->value(3).toString(), m_query->value(5).toString(), m_query));
        }
        else if (m_query->value(0).toString() == "poloniex")
        {
            //m_sites.append(new Poloniex(m_query->value(1).toString(),m_query->value(2).toString(),m_query->value(3).toString(), m_query->value(5).toString(), m_query));
        }
    }


    foreach (BTCexchange* solo, m_sites)
    {
        if (*solo->get_apiKey() != "")
        {
            //solo->viewOpenOrder();
            //solo->rafraichirOrderBook();
            //solo->loadBalance();
            solo->sellOrder(0.24,0.02);
            //solo->buyOrder(0.24,0.010);
            //solo->cancelOrder("ODULRG-AJOLB-ZU2XRN");
            //solo->lookOrder("120033296664");
            //solo->cancelOrder();
        }
    }


}

void MainWindow::generateDB(QSqlQuery *query)
{
    //créé les tables
    query->exec("CREATE TABLE `currency` ( `ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,`currency`	CHAR(50) NOT NULL);");
    query->exec("CREATE TABLE exchange (ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, ID_Currency INT(11) NOT NULL, ID_Sitename INT(11) NOT NULL, CONSTRAINT FK_exchange_currency_ID FOREIGN KEY (ID_Currency) REFERENCES currency(ID) ON DELETE RESTRICT ON UPDATE RESTRICT, CONSTRAINT FK_exchange_sites_ID FOREIGN KEY (ID_Sitename) REFERENCES sites(ID) ON DELETE RESTRICT ON UPDATE RESTRICT);");
    query->exec("CREATE TABLE `sites` ( `ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `sitename`	TEXT NOT NULL, apiKey VARCHAR(255) DEFAULT NULL, secretKey VARCHAR(255) DEFAULT NULL, `ident`	INTEGER, `passphrase`	TEXT);");

    //add 3 currency de bases
    query->exec("INSERT INTO sites (sitename) VALUES ('quadriga'), ('kraken'), ('poloniex');");
    query->exec("INSERT INTO currency (currency) VALUES ('CAD'), ('USD'), ('EURO'), ('XMR');");
    query->exec("INSERT INTO exchange (ID_Currency, ID_Sitename) VALUES ( (SELECT ID from currency WHERE currency='CAD'),     (SELECT ID from sites WHERE sitename='kraken') ), ( (SELECT ID from currency WHERE currency='CAD'),     (SELECT ID from sites WHERE sitename='quadriga') ), ( (SELECT ID from currency WHERE currency='USD'),     (SELECT ID from sites WHERE sitename='quadriga') ), ( (SELECT ID from currency WHERE currency='USD'),     (SELECT ID from sites WHERE sitename='poloniex')), ( (SELECT ID from currency WHERE currency='USD'),     (SELECT ID from sites WHERE sitename='kraken') ), ( (SELECT ID from currency WHERE currency='XMR'),     (SELECT ID from sites WHERE sitename='poloniex') ), ( (SELECT ID from currency WHERE currency='XMR'),     (SELECT ID from sites WHERE sitename='kraken') );");
}

MainWindow::~MainWindow()
{
    delete m_query;
    delete m_trade;
    delete ui;
    foreach (BTCexchange* solo, m_sites)
    {
        delete solo;
    }
}

void MainWindow::on_pushButton_clicked()
{
    Trade profit(&m_sites, m_query);
    delete profit.calculProfitability(ui->text_amountProfitability->text().replace(',','.').toDouble());
    //delete calculProfitability(ui->text_amountProfitability->text().replace(',','.').toDouble());
}

void MainWindow::on_text_amountProfitability_returnPressed()
{
    ui->pushButton->click();
}

void MainWindow::updateNeg()
{
     ui->lineEdit_Neg->setText(QString::number(ui->lineEdit_Neg->text().toDouble() + 1));
}

void MainWindow::updatePos()
{
     ui->lineEdit_Pos->setText(QString::number(ui->lineEdit_Pos->text().toDouble() + 1));
}

void MainWindow::updateFunds()
{
     ui->LineEdit_fund->setText(QString::number(ui->LineEdit_fund->text().toDouble() + 1));
}

void MainWindow::on_pushButton_refreshBalance_clicked()
{
    foreach (BTCexchange* solo, m_sites)
    {
        if (*solo->get_apiKey() != "")
        {
            solo->loadBalance();
        }
    }
}
