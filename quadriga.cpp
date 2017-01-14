#include "quadriga.h"


Quadriga::Quadriga(QString currency, QString liveApiKey, QString liveSecretKey, int ident, QSqlQuery *query) : BTCexchange(currency, liveApiKey, liveSecretKey, query)
{
    m_apiUrl = "https://api.quadrigacx.com/v2";
    orderBookAddr = m_apiUrl + "/order_book?book=btc_" + currentCurrencyMinor.toLower();
    m_ident = ident;
    m_siteName = "quadriga";

    m_feeMaker = 0.5;
    m_feeTaker = 0.5;
}

void Quadriga::getSignature(signature *sign)
{
     qint64 timeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

     QByteArray message = (QString::number(timeStamp) + QString::number(m_ident) + apiKey).toLatin1();

     sign->time = timeStamp;
     sign->hmac256 = (*hmacSignature(&message, QCryptographicHash::Sha256)).toHex();
}


void Quadriga::loadBalance()
{
    signature *sign = new signature;
    getSignature(sign);

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"";
    // Url de la requete
    QNetworkRequest request(QUrl(m_apiUrl + "/balance"));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    delete sign;
    return interpreterLoadBalance(&request, &jsonString);
}

void Quadriga::lookOrder(QString orderID)
{
    signature *sign = new signature;
    getSignature(sign);

    getSignature(sign);


    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&id="+orderID.toLatin1();

    // Url de la requete
    request->setUrl(QUrl(m_apiUrl + "/lookup_order"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterCrap(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;
}

bool Quadriga::cancelOrder(QString orderID = "")
{    
    signature *sign = new signature;
    getSignature(sign);

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&id="+orderID.toLatin1();
    // Url de la requete
    QNetworkRequest request(QUrl(m_apiUrl + "/cancel_order"));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    delete sign;
    return interpreterCancelOrders(&request, &jsonString, &orderID, QNetworkAccessManager::PostOperation);
}

void Quadriga::viewOpenOrder()
{
    /*
    signature *sign = new signature;
    getSignature(sign);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&book=btc_" + currentCurrency.toLower().toLatin1();

    // Url de la requete
    request->setUrl(QUrl(m_apiUrl + "/open_orders"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterCrap(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;
    */

    signature *sign = new signature;
    getSignature(sign);

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&book=btc_" + currentCurrencyMinor.toLower().toLatin1();

    QNetworkRequest request;
    // Url de la requete
    request.setUrl(QUrl(m_apiUrl + "/open_orders"));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    delete sign;
    interpreterLookOrders(&request, &jsonString, QNetworkAccessManager::PostOperation);
}

int Quadriga::buyOrder(double amount, double price)
{
    signature *sign = new signature;
    getSignature(sign);

    QByteArray jsonString;

    if (price != 0)
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&price="+QString::number(price).toLatin1()+"&book=btc_" + currentCurrencyMinor.toLower().toLatin1();
    else
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&book=btc_" + currentCurrencyMinor.toLower().toLatin1();

    // Url de la requete
    QNetworkRequest request(QUrl(m_apiUrl + "/buy"));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    delete sign;
    return interpreterBuySell(&request, BTCexchange::typeBuy, &price, &amount, &jsonString);
}

int Quadriga::sellOrder(double amount, double price)
{
    signature *sign = new signature;
    getSignature(sign);

    QByteArray jsonString;

    if (price != 0)
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&price="+QString::number(price).toLatin1()+"&book=btc_" + currentCurrencyMinor.toLower().toLatin1();
    else
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&book=btc_" + currentCurrencyMinor.toLower().toLatin1();

    // Url de la requete

    QNetworkRequest request(QUrl(m_apiUrl + "/sell"));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    delete sign;
    return interpreterBuySell(&request, BTCexchange::typeSell, &price, &amount, &jsonString);
}


void Quadriga::interpreterLoadBalance(QNetworkRequest* request, QByteArray *jsonString)
{
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkReply *reply = mgr.post(*request, *jsonString);

    eventLoop.exec(); // blocks stack until "finished()" has been called


    QString reponse = reply->readAll();

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());

    QJsonObject jsonObject = jsonDocument.object();
    m_balance_btc = jsonObject.value("btc_available").toString().replace(',','.').toDouble();
    m_balance_btcHold = jsonObject.value("btc_reserved").toString().replace(',','.').toDouble();
    m_balance_fiat = jsonObject.value(currentCurrencyMinor.toLower() + "_available").toString().replace(',','.').toDouble();
    m_balance_fiatHold = jsonObject.value(currentCurrencyMinor.toLower() + "_reserved").toString().replace(',','.').toDouble();


    qDebug() << "QuadrigaCX";
    qDebug() << "btc" << m_balance_btc;
    qDebug() << "btcHold"  << m_balance_btcHold;
    qDebug() << currentCurrencyMinor.toLatin1().data()  << m_balance_fiat;
    qDebug() << (currentCurrencyMinor + "Hold").toLatin1().data()  << m_balance_fiatHold;


    delete reply;

}

void Quadriga::interpreterCrap(QNetworkReply* reply)
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

Quadriga::~Quadriga()
{

}
