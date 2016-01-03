#ifndef BTCEXCHANGE_H
#define BTCEXCHANGE_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

struct OrderBookElement{
    double nbBtc;
    double prixVente;
};

class BTCexchange : public QObject
{

    Q_OBJECT

public:

    explicit BTCexchange();
    bool rafraichirOrderBook();

protected:
    QString orderBookAddr;
    struct OrderBookElement orderBooks[5];


public slots :
    virtual void interpreterOrderBook(QNetworkReply* reply) = 0;


private slots:

};

#endif // BTCEXCHANGE_H
