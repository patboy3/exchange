#include "coinbase.h"
#include <QDebug>

CoinBase::CoinBase()
{

}


void CoinBase::lireJsonFinished(QNetworkReply* reply)
{

    QString reponse = reply->readAll();
    qDebug() << reponse;

    QJsonDocument test = QJsonDocument::fromJson(reponse.toUtf8());
    QJsonObject array = test.object();

    qDebug() << array["date"].toString();

}

bool CoinBase::rafraichirJson()
{

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QUrl url("http://time.jsontest.com");
    QNetworkRequest request;
    request.setUrl(url);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(lireJsonFinished(QNetworkReply*)));
    networkManager->get(request);  // GET

    return false;
}


