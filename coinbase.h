#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"

class CoinBase : public BTCexchange
{
    Q_OBJECT

public:
    explicit CoinBase(QString currency, QString apiKey, QString secretKey);

private:

protected:
    void signerHeaders(QNetworkRequest*);

public slots:
    void loadBalance(QNetworkReply* reply = 0);

private slots :
    void interpreterOrderBook(QNetworkReply* reply);

};

#endif // COINBASE_H
