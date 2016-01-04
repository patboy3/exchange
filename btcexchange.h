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
    explicit BTCexchange(QString currency, QString apiKey, QString secretKey);
    bool rafraichirOrderBook();
    virtual bool authentifier() = 0;

protected:
    QString orderBookAddr;
    struct OrderBookElement asks[5];
    struct OrderBookElement bids[5];
    QString apiKey;
    QString secretKey;
    QString currentCurrency;





public slots :
    virtual void interpreterOrderBook(QNetworkReply* reply) = 0;


private slots:

};

#endif // BTCEXCHANGE_H
