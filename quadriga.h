#ifndef QUADRIGA_H
#define QUADRIGA_H

#include "btcexchange.h"


class Quadriga : public BTCexchange
{
    Q_OBJECT

public:
    explicit Quadriga(QString currency, QString liveApiKey, QString liveSecretKey, int ident);
    void loadBalance();
    ~Quadriga();
private:
    int *m_ident;

protected:
    void signerHeaders(QNetworkRequest *);


public slots:


private slots :
    void interpreterOrderBook(QNetworkReply* reply);
    void interpreterLoadBalance(QNetworkReply* reply);

};

#endif // QUADRIGA_H
