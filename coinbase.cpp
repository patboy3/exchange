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



    QString timeStamp = QDateTime::currentDateTime().toString();

    QString message = timeStamp;

    // For your "Content-Length" header
    //QByteArray postDataSize = QByteArray::number(j);

    // Time for building your request
    QUrl serviceURL("https://api.exchange.coinbase.com/accounts");
    QNetworkRequest request(serviceURL);

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    request.setRawHeader("CB-ACCESS-KEY", "SBH5GeIntChyxpax");
    request.setRawHeader("CB-ACCESS-SIGN", "My app name v0.1");
    request.setRawHeader("CB-ACCESS-TIMESTAMP",  timeStamp.toLatin1());
    request.setRawHeader("CB-ACCESS-PASSPHRASE", 0);
    //request.setRawHeader("Content-Type", "application/json");

    qDebug() << "TEST : " << request.rawHeader("CB-ACCESS-KEY");

    QNetworkAccessManager *m_qnam = new QNetworkAccessManager();
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(handleNetworkData(QNetworkReply*)));

    QNetworkReply * reply = m_qnam->get(request);



    qDebug() << reply->readAll();

    return false;
}



void CoinBase::handleNetworkData(QNetworkReply* reponse){
    qDebug(" ");
    qDebug(" ");
    qDebug("CoinBASE DEBUG ");
    qDebug(" ");


    // Gestion des erreurs
    if(reponse->error())
    {
        qDebug() << reponse->errorString();
        //QMessageBox msgBox;
        //msgBox.setText("Erreur lors de la requete : " + reponse->errorString());
        //msgBox.exec();
        //return;
    }


qDebug() << reponse->readAll();
}


void CoinBase::loadBalance(){

    authentifier();
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


