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
    loadSite(); //load les site ds la db et rempli les balances
}


void MainWindow::loadSite()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName( "./exchange.db");
    db.open();

    //Si sa connect... rentre les site ds une list de site

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

    query.exec("SELECT sites.sitename, currency.currency, sites.apiKey, sites.secretKey, sites.ident, sites.passphrase FROM exchange left join currency on currency.ID = exchange.ID_Currency left outer join sites on sites.ID = exchange.ID_Sitename;");

    while (query.next()) {
        if (query.value(0).toString() == "quadriga")
        {
            m_sites.append(new Quadriga(query.value(1).toString(),query.value(2).toString(),query.value(3).toString(),query.value(4).toInt()));
        }
        else if (query.value(0).toString() == "coinbase")
        {
            m_sites.append(new CoinBase(query.value(1).toString(),query.value(2).toString(),query.value(3).toString(), query.value(5).toString()));
        }
    }


    foreach (BTCexchange* solo, m_sites)
    {
        if (*solo->get_apiKey() != "")
        {
            //solo->viewOpenOrder();
            //solo->rafraichirOrderBook();
            solo->loadBalance();
            //solo->sellOrder(0.01,800);
            //solo->buyOrder(5.545);
            //solo->cancelOrder("qtw4c0ig2aks95bb969dpnbf4stlmuvcosh1szhua1nj6zvg64uvr5avx3gjycwm");
            //solo->lookOrder("eedd5065-fdf9-4da1-b23a-6f51b79dc32a");
        }
    }


}

QList<struct_profitability>* MainWindow::calculProfitability(double amount)
{
    //faut calculer la profitabilité de chaque site en prenant compte de la currency
    //1 - 2 . 1 - 3 . 1 - 4 . 2 - 3 . 2 - 4 . 3 - 4

    //faut aller chercher l'order book de toute les site ... et comparer les site entre eux (buy/sell)
    foreach (BTCexchange* solo, m_sites)
    {
        if (*solo->get_apiKey() != "")
        {
            //faudrait partir sa en thread et lui faire attendre que aille toute fini avant le calcul de la profitabilité
            solo->rafraichirOrderBook();
        }
    }

    QList<struct_profitability> *profitability = new QList<struct_profitability>;
    //QList<double> profitability;
    for (int i=0;i<m_sites.count() - 1;i++)
    {

        double buyI(m_sites[i]->get_averagePrice(amount, BTCexchange::typeBuy, true));
        double selI(m_sites[i]->get_averagePrice(amount, BTCexchange::typeSell, true));
        for (int z = i + 1;z<m_sites.count();z++)
        {
            if (*m_sites[i]->get_currentCurrency() == *m_sites[z]->get_currentCurrency())
            {
                double buyZ(m_sites[z]->get_averagePrice(amount, BTCexchange::typeBuy, true));
                double selZ(m_sites[z]->get_averagePrice(amount, BTCexchange::typeSell, true));

                //calcul des profitabilité
                //buy sur la i sell sur la z
                struct_profitability prof1;
                prof1.buyExchange = m_sites[i];
                prof1.sellExchange = m_sites[z];
                prof1.profitPourcentage = (selZ / buyI - 1) * 100;
                profitability->append(prof1);

                //profitability.append((selZ / buyI - 1) * 100);
                //qDebug() << QString("profitabilité " + QString::number(profitability.count()) + " (buy sur " + *m_sites[i]->get_sitename() + "_" + *m_sites[i]->get_currentCurrency() + " sell sur " + *m_sites[z]->get_sitename() + "_" + *m_sites[z]->get_currentCurrency() + "): " + QString::number(profitability[profitability.count() - 1]) + "%");

                //buy sur la z sell sur la i
                //profitability.append((selI / buyZ - 1) * 100);
                //qDebug() << QString("profitabilité " + QString::number(profitability.count()) + " (buy sur " + *m_sites[z]->get_sitename() + "_" + *m_sites[z]->get_currentCurrency() + " sell sur " + *m_sites[i]->get_sitename() + "_" + *m_sites[i]->get_currentCurrency() + ") : " + QString::number(profitability[profitability.count() - 1]) + "%");
                struct_profitability prof2;
                prof2.buyExchange = m_sites[z];
                prof2.sellExchange = m_sites[i];
                prof2.profitPourcentage = (selI / buyZ - 1) * 100;
                profitability->append(prof2);
            }
        }
    }

    for (int i=0;i<profitability->count();i++)
    {
        qDebug() << QString("profitabilité " + QString::number(i + 1) + " (buy sur " + *(*profitability)[i].buyExchange->get_sitename() + "_" + *(*profitability)[i].buyExchange->get_sitename() + " sell sur " + *(*profitability)[i].sellExchange->get_sitename() + "_" + *(*profitability)[i].sellExchange->get_sitename() + ") : " + QString::number((*profitability)[i].profitPourcentage) + "%");
    }

}

void MainWindow::generateDB(QSqlQuery *query)
{
    //créé les tables
    query->exec("CREATE TABLE `currency` ( `ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,`currency`	CHAR(50) NOT NULL);");
    query->exec("CREATE TABLE exchange (ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, ID_Currency INT(11) NOT NULL, ID_Sitename INT(11) NOT NULL, CONSTRAINT FK_exchange_currency_ID FOREIGN KEY (ID_Currency) REFERENCES currency(ID) ON DELETE RESTRICT ON UPDATE RESTRICT, CONSTRAINT FK_exchange_sites_ID FOREIGN KEY (ID_Sitename) REFERENCES sites(ID) ON DELETE RESTRICT ON UPDATE RESTRICT);");
    query->exec("CREATE TABLE `sites` ( `ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `sitename`	TEXT NOT NULL, apiKey VARCHAR(255) DEFAULT NULL, secretKey VARCHAR(255) DEFAULT NULL, `ident`	INTEGER, `passphrase`	TEXT);");

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

void MainWindow::on_pushButton_clicked()
{
    calculProfitability(ui->text_amountProfitability->text().replace(',','.').toDouble());
}
