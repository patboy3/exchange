#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"

class CoinBase : public BTCexchange
{
    Q_OBJECT

public:
    explicit CoinBase(QString currency, QString apiKey, QString secretKey, QString passphrase);
    void loadBalance();
    void buyOrder(double amount, double price);
    void sellOrder(double amount, double price);
    void cancelOrder(QString orderID);
    void viewOpenOrder();
    void lookOrder(QString orderID);

private:
    QByteArray m_secret;
    QString m_passphrase;
    void signerHeaders(QNetworkRequest* requete, QString timeStamp, QString method, QString *requestPath, QByteArray *parameters);
protected:



public slots:


private slots :
    void interpreterOrderBook(QNetworkReply*);
    void interpreterLoadBalance(QNetworkReply*);

};

#endif // COINBASE_H
