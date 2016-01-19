#include "btcexchange.h"



BTCexchange::BTCexchange(QString currency, QString liveApiKey, QString liveSecretKey)
{
    balance_fiat = 0;
    balance_fiatHold = 0;
    balance_btc = 0;
    balance_btcHold = 0;

    apiKey = liveApiKey;
    secretKey = liveSecretKey;
    currentCurrency = currency;
}

QByteArray* BTCexchange::hmacSignature(QByteArray *message, QCryptographicHash::Algorithm method, bool secretKeyIsBase64)
{
    QByteArray key;

    if (!secretKeyIsBase64)
        key = secretKey.toLatin1();
    else
    {
        QByteArray ba;
        ba.append(secretKey);

        key = QByteArray::fromBase64(ba);
    }

    QMessageAuthenticationCode code(method);
    code.setKey(key);
    code.addData(*message);

    return new QByteArray(code.result());
}


QString* BTCexchange::get_apiKey()
{
    return &apiKey;
}

double* BTCexchange::get_balance_fiat()
{
    return &balance_fiat;
}

double* BTCexchange::get_balance_fiatHold()
{
    return &balance_fiatHold;
}

double* BTCexchange::get_balance_btc()
{
    return &balance_btc;
}

double* BTCexchange::get_balance_btcHold()
{
    return &balance_btcHold;
}



bool BTCexchange::rafraichirOrderBook()
{

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    // Url de la requete
    request->setUrl(QUrl(orderBookAddr));

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterOrderBook(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->get(*request);

    return false;
}

BTCexchange::~BTCexchange()
{

}
