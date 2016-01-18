#ifndef BTCEXCHANGE_H
#define BTCEXCHANGE_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/x509.h>
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

    virtual void viewOpenOrder() = 0;
    virtual void loadBalance() = 0;
    virtual void buyOrder(double amount, double price = 0) = 0; //pour acheter des btc amount en btc... si price = 0... argent en fiat !
    virtual void sellOrder(double amount, double price = 0) = 0; //Pour vendre des btc amount en btc
    virtual void cancelOrder(QString orderID) = 0;
    virtual void lookOrder(QString orderID) = 0;    

    ~BTCexchange();
protected:    
    QString orderBookAddr;
    struct OrderBookElement *asks[5];
    struct OrderBookElement *bids[5];
    QString apiKey;
    QString secretKey;
    QString currentCurrency;
    virtual void signerHeaders(QNetworkRequest*) = 0;
    void hmac_sha256(
        const char *text,      /* pointer to data stream        */
        int                 text_len,   /* length of data stream         */
        const char *key,       /* pointer to authentication key */
        int                 key_len,    /* length of authentication key  */
        void                *digest);
    QString base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
    QString hex_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
    double m_minAmountBTC;
    double m_minAmouterFiat;
    double m_feeTaker;
    double m_feeMaker;
    QByteArray *hmacSignature(QByteArray *message, QCryptographicHash::Algorithm method);


private :


public slots :


private slots:

};

#endif // BTCEXCHANGE_H
