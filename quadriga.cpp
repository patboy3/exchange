#include "quadriga.h"

Quadriga::Quadriga() : BTCexchange("", "liveApiKey", "liveSecretKey")
{


}

Quadriga::Quadriga(QString currency, QString liveApiKey, QString liveSecretKey, int ident) : BTCexchange(currency, liveApiKey, liveSecretKey)
{
    m_apiUrl = "https://api.quadrigacx.com/v2";
    orderBookAddr = m_apiUrl + "/order_book?book=btc_" + currentCurrency.toLower();
    m_ident = ident;
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

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"";

    // Url de la requete
    request->setUrl(QUrl(m_apiUrl + "/balance"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterLoadBalance(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;
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

void Quadriga::cancelOrder(QString orderID)
{
    signature *sign = new signature;
    getSignature(sign);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&id="+orderID.toLatin1();

    // Url de la requete
    request->setUrl(QUrl(m_apiUrl + "/cancel_order"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterCrap(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;
}

void Quadriga::viewOpenOrder()
{
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
}

void Quadriga::buyOrder(double amount, double price)
{
    signature *sign = new signature;
    getSignature(sign);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    QByteArray jsonString;

    if (price != 0)
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&price="+QString::number(price).toLatin1()+"&book=btc_" + currentCurrency.toLower().toLatin1();
    else
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&book=btc_" + currentCurrency.toLower().toLatin1();

    // Url de la requete
    request->setUrl(QUrl(m_apiUrl + "/buy"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterCrap(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;
}

void Quadriga::sellOrder(double amount, double price)
{
    /*
    signature *sign = new signature;
    getSignature(sign);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    QByteArray jsonString;

    if (price != 0)
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&price="+QString::number(price).toLatin1()+"&book=btc_" + currentCurrency.toLower().toLatin1();
    else
        jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&book=btc_" + currentCurrency.toLower().toLatin1();

    // Url de la requete
    request->setUrl(QUrl(m_apiUrl + "/sell"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterOrders(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;*/

    // create custom temporary event loop on stack
        QEventLoop eventLoop;

        // "quit()" the event-loop, when the network request "finished()"
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        // the HTTP request

        signature *sign = new signature;
        getSignature(sign);

        QNetworkRequest *request = new QNetworkRequest();

        QByteArray jsonString;

        if (price != 0)
            jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&price="+QString::number(price).toLatin1()+"&book=btc_" + currentCurrency.toLower().toLatin1();
        else
            jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"&amount="+QString::number(amount).toLatin1()+"&book=btc_" + currentCurrency.toLower().toLatin1();

        // Url de la requete
        request->setUrl(QUrl(m_apiUrl + "/sell"));
        request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

        QNetworkReply *reply = mgr.post(*request, jsonString);
        eventLoop.exec(); // blocks stack until "finished()" has been called


        if (errorRequete(reply))
        {
            delete reply;
            return;
        }
        else
        {
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObject = jsonDocument.object();

            orders current;
            current.amount = amount;
            current.price = price;
            current.order_id = jsonObject.value("id").toString();

            m_orderSell.append(current);
            delete reply;
        }

        foreach (orders solo, m_orderSell)
        {
            qDebug() << "sell - id : "  << solo.order_id;
            qDebug() << "sell - price : "  << solo.price;
            qDebug() << "sell - amount : "  << solo.amount;
        }
}

void Quadriga::interpreterOrders(QNetworkReply* reply, double *amount)
{
    // Gestion des erreurs
    if (errorRequete(reply))
        return;

    // Laleur de reply->readAll() se vide apres usage
    QString reponse = reply->readAll();

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    qDebug() << reponse;
    qDebug() << amount;
    delete reply;

}

void Quadriga::interpreterLoadBalance(QNetworkReply* reply)
{
    // Gestion des erreurs
    if (errorRequete(reply))
        return;

    // Laleur de reply->readAll() se vide apres usage
    QString reponse = reply->readAll();

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    m_balance_btc = jsonObject.value("btc_available").toString().replace(',','.').toDouble();
    m_balance_btcHold = jsonObject.value("btc_reserved").toString().replace(',','.').toDouble();
    m_balance_fiat = jsonObject.value(currentCurrency.toLower() + "_available").toString().replace(',','.').toDouble();
    m_balance_fiatHold = jsonObject.value(currentCurrency.toLower() + "_reserved").toString().replace(',','.').toDouble();


    qDebug() << "QuadrigaCX";
    qDebug() << "btc" << m_balance_btc;
    qDebug() << "btcHold"  << m_balance_btcHold;
    qDebug() << "fiat"  << m_balance_fiat;
    qDebug() << "fiatHold"  << m_balance_fiatHold;


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
