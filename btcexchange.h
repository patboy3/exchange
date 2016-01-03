#ifndef BTCEXCHANGE_H
#define BTCEXCHANGE_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

class BTCexchange
{
public:
    explicit BTCexchange();
    virtual bool rafraichirJson() = 0;

private:

};

#endif // BTCEXCHANGE_H
