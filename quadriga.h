#ifndef QUADRIGA_H
#define QUADRIGA_H

#include "btcexchange.h"


struct signature{
    qint64 time;
    QString hmac256;
};

class Quadriga : public BTCexchange
{
    Q_OBJECT

public:
    explicit Quadriga();
    explicit Quadriga(QString currency, QString liveApiKey, QString liveSecretKey, int ident);
    void loadBalance();
    void buyOrder(double amount, double price = 0);
    void sellOrder(double amount, double price = 0);
    void cancelOrder(QString orderID);
    void viewOpenOrder();
    void lookOrder(QString orderID);
    ~Quadriga();
private:
    int *m_ident;
    void getSignature(signature *sign);

protected:
    void signerHeaders(QNetworkRequest *);


public slots:


private slots :
    void interpreterOrderBook(QNetworkReply* reply);
    void interpreterLoadBalance(QNetworkReply* reply);

};

#endif // QUADRIGA_H
