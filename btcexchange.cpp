#include "btcexchange.h"



BTCexchange::BTCexchange(QString currency, QString liveApiKey, QString liveSecretKey)
{
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
