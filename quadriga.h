#ifndef QUADRIGA_H
#define QUADRIGA_H

#include "btcexchange.h"
#include <QMessageBox>


class Quadriga : public BTCexchange
{
    Q_OBJECT

public:
    explicit Quadriga();
    bool authentifier();

private:

protected:

public slots:

private slots :
    void interpreterOrderBook(QNetworkReply* reply);

};

#endif // QUADRIGA_H
