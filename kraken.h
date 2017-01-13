#ifndef KRAKEN_H
#define KRAKEN_H

#include "btcexchange.h"

class Kraken : public BTCexchange
{
    Q_OBJECT

public:
    explicit Kraken(QString currency, QString apiKey, QString secretKey, QString passphrase, QSqlQuery *query);
    void loadBalance();
    int buyOrder(double amount, double price = 0);
    int sellOrder(double amount, double price = 0);
    bool cancelOrder(QString orderID);
    void viewOpenOrder();
    void lookOrder(QString orderID);
    QString get_currentCurrency();

private:
    QByteArray m_secret;
    QString m_passphrase;
    void signerHeaders(QNetworkRequest* requete, QString timeStamp, QString *requestPath, QByteArray *postData);
    void interpreterLoadBalance(QNetworkRequest* request, QByteArray *postData);
    bool interpreterOrderBook(QNetworkRequest* request);
    QString currencyNameModification(bool remove = false);
protected:



public slots:


private slots :
    void interpreterCrap(QNetworkReply* reply);

};


#endif // KRAKEN_H
