#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"


class CoinBase : public BTCexchange
{
    Q_OBJECT

public:
    explicit CoinBase();

private:

public slots:

private slots :
    void interpreterOrderBook(QNetworkReply* reply);
};

#endif // COINBASE_H
