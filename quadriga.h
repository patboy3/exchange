#ifndef QUADRIGA_H
#define QUADRIGA_H

#include "btcexchange.h"


class Quadriga : public BTCexchange
{
    Q_OBJECT

public:
    explicit Quadriga(QString currency, QString liveApiKey, QString liveSecretKey);
    bool authentifier();
    void loadBalance();

private:

protected:

public slots:

private slots :
    void interpreterOrderBook(QNetworkReply* reply);

};

#endif // QUADRIGA_H
