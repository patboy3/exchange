#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"

class CoinBase : public BTCexchange
{
    Q_OBJECT

public:
    explicit CoinBase(QString currency, QString apiKey, QString secretKey, QString passphrase);
    void loadBalance();
    bool buyOrder(double amount, double price = 0);
    bool sellOrder(double amount, double price = 0);
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
    void interpreterCrap(QNetworkReply* reply);
    void interpreterLoadBalance(QNetworkReply* reply);

};

#endif // COINBASE_H
