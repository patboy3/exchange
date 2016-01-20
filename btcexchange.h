#ifndef BTCEXCHANGE_H
#define BTCEXCHANGE_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <iostream>
#include <QMessageAuthenticationCode>
#include <QEventLoop>

struct OrderBookElement{
    double nbBtc;
    double prixVente;
};

struct orders{
    QString order_id;
    double amount;
    double price;
};

class BTCexchange : public QObject
{

    Q_OBJECT

public:
    explicit BTCexchange(QString currency, QString liveApiKey, QString liveSecretKey);
    bool rafraichirOrderBook();

    QString* get_apiKey();
    double* get_balance_fiat();
    double* get_balance_fiatHold();
    double* get_balance_btc();
    double* get_balance_btcHold();


    virtual void viewOpenOrder() = 0;
    virtual void loadBalance() = 0;
    virtual void buyOrder(double amount, double price = 0) = 0; //pour acheter des btc amount en btc... si price = 0... argent en fiat !
    virtual void sellOrder(double amount, double price = 0) = 0; //Pour vendre des btc amount en btc
    virtual void cancelOrder(QString orderID) = 0;
    virtual void lookOrder(QString orderID) = 0;    

    ~BTCexchange();
protected:
    QByteArray *hmacSignature(QByteArray *message, QCryptographicHash::Algorithm method, bool secretKeyIsBase64 = false);
    bool errorRequete(QNetworkReply* reply);
    QString orderBookAddr;
    QList<orders> m_orderBuy;
    QList<orders> m_orderSell;
    QList<OrderBookElement> m_asks;
    QList<OrderBookElement> m_bids;
    QString apiKey;
    QString secretKey;
    QString currentCurrency;
    QString m_apiUrl;

    double m_minAmountBTC;
    double m_minAmouterFiat;
    double m_feeTaker;
    double m_feeMaker;

    double m_balance_fiat;
    double m_balance_fiatHold;
    double m_balance_btc;
    double m_balance_btcHold;

private :

protected slots:
    virtual void interpreterOrderBook(QNetworkReply* reply);

public slots :


private slots:

};

#endif // BTCEXCHANGE_H
