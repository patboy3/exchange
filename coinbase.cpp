#include "coinbase.h"
#include <QDebug>
#include <QMessageBox>


/*

    lacroix.david1@gmail.com
    Clé d'API : SBH5GeIntChyxpax
    Clé d'API secrète : G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T

 */

CoinBase::CoinBase()
{
    orderBookAddr = "https://api.exchange.coinbase.com/products/BTC-CAD/book?level=2";
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
        Clé d'API : SBH5GeIntChyxpax

        Clé d'API secrète : G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T  }

    // Laleur de reply->readAll() se vide apres usage
    QString reponse = reply->readAll();
    qDebug() << reponse;

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();

    qDebug() << jsonObject["date"].toString();

}


