#include "btcexchange.h"



BTCexchange::BTCexchange(QString currency, QString liveApiKey, QString liveSecretKey)
{
    m_balance_fiat = 0;
    m_balance_fiatHold = 0;
    m_balance_btc = 0;
    m_balance_btcHold = 0;

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
    return &m_balance_fiat;
}

double* BTCexchange::get_balance_fiatHold()
{
    return &m_balance_fiatHold;
}

double* BTCexchange::get_balance_btc()
{
    return &m_balance_btc;
}

double* BTCexchange::get_balance_btcHold()
{
    return &m_balance_btcHold;
}

bool BTCexchange::errorRequete(QNetworkReply* reply)
{
    if(reply->error())
    {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de la requete : " + reply->errorString());
        msgBox.exec();

        delete reply;
        return true;
    }

    return false;
}

void BTCexchange::interpreterOrderBook(QNetworkReply* reply)
{
    // Gestion des erreurs
    if (errorRequete(reply))
        return;

    // Laleur de reply->readAll() se vide apres usage
    QString reponse = reply->readAll();

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());

    QJsonObject jsonObject = jsonDocument.object();

    QJsonArray asks = jsonObject["asks"].toArray();
    m_asks.clear();;
    foreach (const QJsonValue & value, asks)
    {
        OrderBookElement now;
        now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
        now.prixVente = value.toArray()[0].toString().replace(',','.').toDouble();

        m_asks.append(now);
    }

    QJsonArray bids = jsonObject["bids"].toArray();
    m_bids.clear();
    foreach (const QJsonValue & value, bids)
    {
        OrderBookElement now;
        now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
        now.prixVente = value.toArray()[0].toString().replace(',','.').toDouble();

        m_bids.append(now);
    }

    foreach (OrderBookElement solo, m_bids)
    {
        qDebug() << "bids - btc : "  << solo.nbBtc;
        qDebug() << "bids - price : "  << solo.prixVente;
    }

    foreach (OrderBookElement solo, m_asks)
    {
        qDebug() << "asks - btc : "  << solo.nbBtc;
        qDebug() << "asks - price : "  << solo.prixVente;
    }

    delete reply;

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
