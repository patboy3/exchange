#ifndef QUADRIGA_H
#define QUADRIGA_H

#include "btcexchange.h"


class Quadriga : public BTCexchange
{
    Q_OBJECT

public:
    explicit Quadriga(QString currency, QString liveApiKey, QString liveSecretKey);


private:

protected:
    void signerHeaders(QNetworkRequest *);

public slots:
void loadBalance(QNetworkReply* reply = 0);

private slots :
    void interpreterOrderBook(QNetworkReply* reply);

};

#endif // QUADRIGA_H
