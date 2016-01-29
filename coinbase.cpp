#include "coinbase.h"


CoinBase::CoinBase(QString currency, QString apiKey, QString secretKey, QString passphrase, QSqlQuery *query) : BTCexchange(currency, apiKey, secretKey, query)
{
    m_apiUrl = "https://api.exchange.coinbase.com";
    orderBookAddr = m_apiUrl + "/products/BTC-" + currentCurrency + "/book?level=2";
    m_passphrase = passphrase;
    m_siteName = "coinbase";

    m_feeMaker = 0.00;
    m_feeTaker = 0.25;
}


void CoinBase::signerHeaders(QNetworkRequest *requete, QString timeStamp, QString method, QString *requestPath, QByteArray *parameters){


    QByteArray message = (timeStamp + method + *requestPath + QString(*parameters)).toLatin1();

    requete->setRawHeader("content-type", "application/json");

    requete->setRawHeader("CB-ACCESS-KEY", apiKey.toLatin1());
    requete->setRawHeader("CB-ACCESS-SIGN", (*hmacSignature(&message,QCryptographicHash::Sha256,true)).toBase64());
    requete->setRawHeader("CB-ACCESS-TIMESTAMP",  (timeStamp).toLatin1());
    requete->setRawHeader("CB-ACCESS-PASSPHRASE", m_passphrase.toLatin1());

}

void CoinBase::loadBalance(){

    QString urlPath = "/accounts";
    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray jsonString = "";

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("GET"), &urlPath, &jsonString);

    QNetworkAccessManager *m_qnam = new QNetworkAccessManager();
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(interpreterLoadBalance(QNetworkReply*)));

    m_qnam->get(request);

}

void CoinBase::viewOpenOrder()
{
    /*
    QString urlPath = "/orders";
    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray jsonString = "";

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("GET"), &urlPath, &jsonString);

    QNetworkAccessManager *m_qnam = new QNetworkAccessManager();
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(interpreterCrap(QNetworkReply*)));

    m_qnam->get(request);
    */
    QByteArray jsonString = "";


    // Url de la requete

    QString urlPath = "/orders";
    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("GET"), &urlPath, &jsonString);

    bool test = interpreterLookOrders(&request, &jsonString, QNetworkAccessManager::GetOperation);
}


bool CoinBase::buyOrder(double amount, double price)
{
    QByteArray jsonString;

    if (price != 0)
    {
        jsonString = "{\"product_id\": \"BTC-"+currentCurrency.toLatin1()+"\""
            ", \"size\": " + QString::number(amount).toLatin1() + ""
            ", \"price\": " + QString::number(price).toLatin1() + ""
            " ,\"side\": \"buy\"}";
    }
    else
    {
        jsonString = "{\"product_id\": \"BTC-"+currentCurrency.toLatin1()+"\""
            ", \"funds\": " + QString::number(amount).toLatin1() + ""
            ", \"type\": \"market\""
            " ,\"side\": \"buy\"}";
    }
    // Url de la requete

    QString urlPath = "/orders";
    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("POST"), &urlPath, &jsonString);

    return interpreterBuySell(&request, BTCexchange::typeBuy, &price, &amount, &jsonString);
}

bool CoinBase::sellOrder(double amount, double price)
{
    QByteArray jsonString;

    if (price != 0)
    {
        jsonString = "{\"product_id\": \"BTC-"+currentCurrency.toLatin1()+"\""
            ", \"size\": " + QString::number(amount).toLatin1() + ""
            ", \"price\": " + QString::number(price).toLatin1() + ""
            " ,\"side\": \"sell\"}";
    }
    else
    {
        jsonString = "{\"product_id\": \"BTC-"+currentCurrency.toLatin1()+"\""
            ", \"size\": " + QString::number(amount).toLatin1() + ""
            ", \"type\": \"market\""
            " ,\"side\": \"sell\"}";
    }
    // Url de la requete

    QString urlPath = "/orders";
    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("POST"), &urlPath, &jsonString);

    return interpreterBuySell(&request, BTCexchange::typeSell, &price, &amount, &jsonString);
}

bool CoinBase::cancelOrder(QString orderID)
{
    QString urlPath;

    if (orderID == "")
        urlPath = "/orders/";
    else
        urlPath = "/orders/" + orderID;

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray jsonString = "";

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("DELETE"), &urlPath, &jsonString);

    return interpreterCancelOrders(&request, &jsonString, &orderID, QNetworkAccessManager::DeleteOperation);
}

void CoinBase::lookOrder(QString orderID)
{
    QString urlPath = "/orders/" + orderID;
    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray jsonString = "";

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("GET"), &urlPath, &jsonString);

    QNetworkAccessManager *m_qnam = new QNetworkAccessManager();
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(interpreterCrap(QNetworkReply*)));

    m_qnam->get(request);
}


void CoinBase::interpreterLoadBalance(QNetworkReply *reply)
{
    // Gestion des erreurs
    if (errorRequete(reply))
        return;

    // Laleur de reply->readAll() se vide apres usage
    QString reponse = reply->readAll();

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());

    QJsonArray json_array = jsonDocument.array();
    foreach (const QJsonValue &value, json_array)
    {
       QJsonObject json_obj = value.toObject();

       if (json_obj["currency"].toString() == currentCurrency)
       {
           m_balance_fiat = json_obj["available"].toString().replace(',','.').toDouble();
           m_balance_fiatHold = json_obj["hold"].toString().replace(',','.').toDouble();
        }
        else if (json_obj["currency"].toString() == "BTC")
        {
           m_balance_btc = json_obj["available"].toString().replace(',','.').toDouble();
           m_balance_btcHold = json_obj["hold"].toString().replace(',','.').toDouble();
        }
    }

    qDebug() << "CoinBase";
    qDebug() << "btc" << m_balance_btc;
    qDebug() << "btcHold"  << m_balance_btcHold;
    qDebug() << "fiat"  << m_balance_fiat;
    qDebug() << "fiatHold"  << m_balance_fiatHold;


    delete reply;
}

void CoinBase::interpreterCrap(QNetworkReply* reply)
{
    // Gestion des erreurs
    if(reply->error())
    {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de la requete : " + reply->errorString());
        msgBox.exec();
        return;
    }

    // Laleur de reply->readAll() se vide apres usage
    QString reponse = reply->readAll();
    qDebug() << reponse;

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    qDebug() << jsonObject["date"].toString();

    delete reply;
}
