#ifndef BTCEXCHANGE_H
#define BTCEXCHANGE_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <iostream>
#include <QMessageAuthenticationCode>

struct OrderBookElement{
    double nbBtc;
    double prixVente;
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

    QString orderBookAddr;
    struct OrderBookElement *asks[5];
    struct OrderBookElement *bids[5];
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


public slots :


private slots:

};

#endif // BTCEXCHANGE_H
