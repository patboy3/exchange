#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"

class CoinBase : public BTCexchange
{
    Q_OBJECT

public:
    explicit CoinBase(QString currency, QString apiKey, QString secretKey);
    void loadBalance();

private:

protected:
    void signerHeaders(QNetworkRequest*);


public slots:


private slots :
    void interpreterOrderBook(QNetworkReply*);
    void interpreterLoadBalance(QNetworkReply*);

};

#endif // COINBASE_H
