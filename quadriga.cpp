#include "quadriga.h"

Quadriga::Quadriga(QString currency, QString liveApiKey, QString liveSecretKey) : BTCexchange(currency, liveApiKey, liveSecretKey)
{
    orderBookAddr = "https://api.quadrigacx.com/v2/order_book?book=btc_" + currentCurrency.toLower();

}

void Quadriga::loadBalance()
{
    qint64 time =  QDateTime::currentMSecsSinceEpoch();
    QString test(QString::number(time).toStdString().c_str());

    const char *key= secretKey.toStdString().c_str();
    const char *message= (test + "36566" + apiKey).toStdString().c_str();

    //const char *key= "230664ae53cbe5a07c6c389910540729";
    //const char *message= "13916834993JJHlXeDcFM";

    char    digest[BUFSIZ];

    memset( digest, 0x00, BUFSIZ );
    hmac_sha256( message, strlen(message), key, strlen(key), digest );

    qDebug() << message;
    qDebug() << time;

    qDebug() << base64_encode((unsigned char*)digest, strlen(digest));
    qDebug() << hex_encode((unsigned char*)digest, strlen(digest));
}

bool Quadriga::authentifier()
{
    return false;
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
