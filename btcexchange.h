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
#include <QtSql>

struct OrderBookElement{
    double nbBtc;
    double prixVente;
};

struct orders{
    QString order_id;
    double amount;
    double price;
    QString type;
};

class BTCexchange : public QObject
{

    Q_OBJECT

public:
    static QString typeBuy;
    static QString typeSell;
    explicit BTCexchange(QString currency, QString liveApiKey, QString liveSecretKey, QSqlQuery *query);
    bool rafraichirOrderBook();
    void get_averagePrice(double amount, QString type, double *result, bool includeFees = false);

    QString* get_sitename();
    QString* get_currentCurrency();
    QString* get_apiKey();
    double* get_balance_fiat();
    double* get_balance_fiatHold();
    double* get_balance_btc();
    double* get_balance_btcHold();
    QList<orders>* get_orders();


    virtual void viewOpenOrder() = 0;  //sort les openOrders live.. et les comaprer avec notre list pour voir si ya eu des delete ! retourner la différence !
    virtual void loadBalance() = 0;
    virtual bool buyOrder(double amount, double price = 0) = 0; //pour acheter des btc amount en btc... si price = 0... argent en fiat !
    virtual bool sellOrder(double amount, double price = 0) = 0; //Pour vendre des btc amount en btc
    virtual bool cancelOrder(QString orderID = "") = 0; //cancel l'order sur l'Exchange et la delete de la list des order présente !
    virtual void lookOrder(QString orderID) = 0;    

    ~BTCexchange();
protected:

    QByteArray *hmacSignature(QByteArray *message, QCryptographicHash::Algorithm method, bool secretKeyIsBase64 = false);
    bool errorRequete(QNetworkReply* reply);
    virtual bool interpreterOrderBook(QNetworkRequest* request);
    virtual int interpreterBuySell(QNetworkRequest* request, QString type, double *price, double *amount, QByteArray *jsonString);
    virtual bool interpreterCancelOrders(QNetworkRequest* request, QByteArray *jsonString, QString *id_Orders, QNetworkAccessManager::Operation operation);
    virtual QList<orders>* interpreterLookOrders(QNetworkRequest* request, QByteArray *jsonString, QNetworkAccessManager::Operation operation);
    QString orderBookAddr;
    QList<orders> m_orders;
    QList<OrderBookElement> m_asks;
    QList<OrderBookElement> m_bids;
    QString apiKey;
    QString secretKey;
    QString currentCurrency;
    QString m_apiUrl;
    QString m_siteName;

    double m_minAmountBTC;
    double m_minAmouterFiat;
    double m_feeTaker;
    double m_feeMaker;

    double m_balance_fiat;
    double m_balance_fiatHold;
    double m_balance_btc;
    double m_balance_btcHold;

    QSqlQuery *m_query;

private :

protected slots:



public slots :


private slots:

};

#endif // BTCEXCHANGE_H
