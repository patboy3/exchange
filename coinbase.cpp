#include "coinbase.h"
#include <QDebug>

#include <QJsonDocument>

#include <QNetworkReply>
#include <QUrl>

CoinBase::CoinBase()
{

}



bool CoinBase::rafraichirJson()
{

    qDebug("JSON COINBASE");


    QNetworkAccessManager networkManager;

   // QUrl url("https://api.quadrigacx.com/v2/transactions");
    QUrl url("http://gdata.youtube.com/feeds/api/standardfeeds/most_popular?v=2&alt=json");
    QNetworkRequest request;
    request.setUrl(url);

    QNetworkReply* currentReply = networkManager.get(request);  // GET



    //QTcpSocket *testSocket = new QTcpSocket();
    //testSocket->connectToHost("https://api.quadrigacx.com/v2/transactions", 80);
    //testSocket->waitForConnected(2000);
    //testSocket->write("\r\n\r\n\r\n");
   // testSocket->waitForReadyRead(3000);
    qDebug() << currentReply->readAll();

    return false;
}


