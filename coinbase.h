#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"


class CoinBase : public QObject, public BTCexchange
{
    Q_OBJECT

public:
    CoinBase();
    bool rafraichirJson();

private:

public slots:

private slots :
    void lireJsonFinished(QNetworkReply* reply);
};

#endif // COINBASE_H
