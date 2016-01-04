#include "coinbase.h"


/*

    lacroix.david1@gmail.com
    Clé d'API : SBH5GeIntChyxpax
    Clé d'API secrète : G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T

 */

CoinBase::CoinBase(QString currency, QString apiKey, QString secretKey) : BTCexchange(currency, apiKey, secretKey)
{
    orderBookAddr = "https://api.exchange.coinbase.com/products/BTC-" + currentCurrency + "/book?level=2";
}


bool CoinBase::authentifier()
{
    return false;
}


void CoinBase::interpreterOrderBook(QNetworkReply* reply)
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


