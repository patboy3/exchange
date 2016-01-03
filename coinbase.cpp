#include "coinbase.h"
#include <QDebug>

CoinBase::CoinBase()
{

}


void CoinBase::lireJsonFinished(QNetworkReply* reply)
{

    // Gestion des erreurs
    if(reply->error())
    {
        qDebug() << "Erreur lors de la requête : " << reply->errorString();
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

bool CoinBase::rafraichirJson()
{

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    // Url de la requete
    request->setUrl(QUrl("http://time.jsontest.com"));

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(lireJsonFinished(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->get(*request);

    return false;
}


