#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"

class CoinBase : public BTCexchange
{
    Q_OBJECT

public:
    explicit CoinBase(QString currency, QString apiKey, QString secretKey);
    bool authentifier();
    void loadBalance();

private:

protected:

public slots:

private slots :
    void handleNetworkData(QNetworkReply*);
    void interpreterOrderBook(QNetworkReply* reply);

};

#endif // COINBASE_H
