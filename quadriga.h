#ifndef QUADRIGA_H
#define QUADRIGA_H

#include "btcexchange.h"


class Quadriga : public BTCexchange
{
    Q_OBJECT

public:
    explicit Quadriga();

private:

public slots:

private slots :
    void interpreterOrderBook(QNetworkReply* reply);
};

#endif // QUADRIGA_H
