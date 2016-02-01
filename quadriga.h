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
    explicit Quadriga(QString currency, QString liveApiKey, QString liveSecretKey, int ident, QSqlQuery *query);
    void loadBalance();
    int buyOrder(double amount, double price = 0);
    int sellOrder(double amount, double price = 0);
    bool cancelOrder(QString orderID);
    void viewOpenOrder();
    void lookOrder(QString orderID);
    ~Quadriga();
private:
    int m_ident;
    void getSignature(signature *sign);
    void interpreterLoadBalance(QNetworkRequest* request, QByteArray *jsonString);
protected:

public slots:


private slots :
    void interpreterCrap(QNetworkReply* reply);

};

#endif // QUADRIGA_H
