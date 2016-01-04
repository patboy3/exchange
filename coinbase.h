#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"
#include <QMessageBox>


class CoinBase : public BTCexchange
{
    Q_OBJECT

public:
    explicit CoinBase(QString currency, QString apiKey, QString secretKey);
    bool authentifier();

private:

protected:

public slots:

private slots :
    void interpreterOrderBook(QNetworkReply* reply);

};

#endif // COINBASE_H
