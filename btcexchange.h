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
#include <QCryptographicHash>
#include <iostream>

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
    struct OrderBookElement *asks[5];
    struct OrderBookElement *bids[5];
    QString apiKey;
    QString secretKey;
    QString currentCurrency;
private :
    void hmac_sha256(
        const char *text,      /* pointer to data stream        */
        int                 text_len,   /* length of data stream         */
        const char *key,       /* pointer to authentication key */
        int                 key_len,    /* length of authentication key  */
        void                *digest);
    QString base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
    QString hex_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

public slots :
    virtual void interpreterOrderBook(QNetworkReply* reply) = 0;


private slots:

};

#endif // BTCEXCHANGE_H
