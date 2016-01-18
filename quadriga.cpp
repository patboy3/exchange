#include "quadriga.h"

Quadriga::Quadriga() : BTCexchange("", "liveApiKey", "liveSecretKey")
{


}

Quadriga::Quadriga(QString currency, QString liveApiKey, QString liveSecretKey, int ident) : BTCexchange(currency, liveApiKey, liveSecretKey)
{
    orderBookAddr = "https://api.quadrigacx.com/v2/order_book?book=btc_" + currentCurrency.toLower();
    m_ident = new int(ident);
}

void Quadriga::signerHeaders(QNetworkRequest *requete)
{

}

void Quadriga::getSignature(signature *sign)
{
     qint64 timeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

     const char *key= secretKey.toLatin1();
     const char *message= (QString::number(timeStamp) + QString::number(*m_ident) + apiKey).toLatin1();

     char digest[BUFSIZ];

     memset( digest, 0x00, BUFSIZ );
     hmac_sha256( message, strlen(message), key, strlen(key), digest );

     sign->time = timeStamp;
     sign->hmac256 = hex_encode((unsigned char*)digest, strlen(digest));


     /*
     qDebug() << "message" << message;
     qDebug() << "apikey" << apiKey;
     qDebug() << "secret" << secretKey;


     qDebug() << "time/nonce" << QString::number(timeStamp);

     qDebug() << "base64_encode" <<base64_encode((unsigned char*)digest, strlen(digest));
     qDebug() << "hex" << hex_encode((unsigned char*)digest, strlen(digest));
     */

     delete message;
     delete key;

     //return sign;
}


void Quadriga::loadBalance()
{
    signature *sign = new signature;
    getSignature(sign);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    QByteArray jsonString = "key="+apiKey.toLatin1()+"&nonce="+QString::number(sign->time).toLatin1() +"&signature="+sign->hmac256.toLatin1()+"";

    // Url de la requete
    request->setUrl(QUrl("https://api.quadrigacx.com/v2/balance"));
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
    request->setUrl(QUrl("https://api.quadrigacx.com/v2/lookup_order"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterLoadBalance(QNetworkReply*)));

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
    request->setUrl(QUrl("https://api.quadrigacx.com/v2/cancel_order"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterLoadBalance(QNetworkReply*)));

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
    request->setUrl(QUrl("https://api.quadrigacx.com/v2/open_orders"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterLoadBalance(QNetworkReply*)));

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
    request->setUrl(QUrl("https://api.quadrigacx.com/v2/buy"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterLoadBalance(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;
}

void Quadriga::sellOrder(double amount, double price)
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
    request->setUrl(QUrl("https://api.quadrigacx.com/v2/sell"));
    request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterLoadBalance(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->post(*request, jsonString);

    delete sign;
}

void Quadriga::interpreterLoadBalance(QNetworkReply* reply)
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

}

void Quadriga::interpreterOrderBook(QNetworkReply* reply)
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

}

Quadriga::~Quadriga()
{
    delete m_ident;
}
