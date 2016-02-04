#ifndef TRADE_H
#define TRADE_H

#include <QThread>
#include "btcexchange.h"
#include "threadorderbook.h"

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
            Trade(QList<BTCexchange *> *sites, QSqlQuery *query);
            QList<struct_profitability> *calculProfitability(double amount);
    private:
            bool checkFunds(double amount, double averageBuyPrice, BTCexchange *buy, BTCexchange *sell);
            QList<BTCexchange*> m_sites;
            double m_minimumTrade;
            QSqlQuery *m_query;
    signals:
            void updateNeg();
            void updatePos();
            void updateFunds();
    public slots :
            void run();
};

#endif // TRADE_H
