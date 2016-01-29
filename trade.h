#ifndef TRADE_H
#define TRADE_H

#include <QApplication>
#include <QtGUI>
#include <QThread>
#include "ui_mainwindow.h"
#include "btcexchange.h"

struct struct_profitability{
    double buyAverage;
    double buyMax;
    BTCexchange* buyExchange;
    double sellAverage;
    BTCexchange* sellExchange;
    double sellMax;
    double profitPourcentage;
};

class Trade : public QThread
{
    Q_OBJECT

    public:
            Trade(QList<BTCexchange *> *sites, QSqlQuery *query, Ui::MainWindow *ui);
            QList<struct_profitability> *calculProfitability(double amount);
    private:
            bool checkFunds(double amount, double averageBuyPrice, BTCexchange *buy, BTCexchange *sell);
            QList<BTCexchange*> m_sites;
            double m_minimumTrade;
            QSqlQuery *m_query;
            Ui::MainWindow *m_ui;
    public slots :
            void run();
};

#endif // TRADE_H
