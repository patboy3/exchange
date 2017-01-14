#ifndef POLONIEX_H
#define POLONIEX_H

#include "btcexchange.h"

class Poloniex : public BTCexchange
{
    Q_OBJECT

public:
    explicit Poloniex(QString currency, QString apiKey, QString secretKey, QString passphrase, QSqlQuery *query);
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
    QString currencyNameModification(bool remove = false);
protected:



public slots:


private slots :
    void interpreterCrap(QNetworkReply* reply);

};



#endif // POLONIEX_H
