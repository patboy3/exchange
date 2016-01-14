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



// Build your JSON string as usual
    QByteArray jsonString = "{\"method\":\"AuthenticatePlain\",\"loginName\":\"lacroix.david1@gmail.com\",\"password\":\"mypass\"}";

    // For your "Content-Length" header
    QByteArray postDataSize = QByteArray::number(jsonString.size());

    // Time for building your request
    QUrl serviceURL("https://api.exchange.coinbase.com/accounts/<account-id>");
    QNetworkRequest request(serviceURL);

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    request.setRawHeader("CB-ACCESS-SIGN", "My app name v0.1");
    request.setRawHeader("CB-ACCESS-TIMESTAMP",  timeStamp.toLatin1());
    request.setRawHeader("CB-ACCESS-KEY", apiKey.toLatin1());
    request.setRawHeader("CB-ACCESS-PASSPHRASE", postDataSize);
    request.setRawHeader("Content-Type", "application/json");


QNetworkAccessManager *m_qnam = new QNetworkAccessManager();
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(handleNetworkData(QNetworkReply*)));

        QNetworkReply * reply = m_qnam->post(request, jsonString);


//qDebug() << reply;

    return false;
}



void CoinBase::handleNetworkData(QNetworkReply* reponse){
    qDebug(" ");
    qDebug(" ");
    qDebug("CoinBASE DEBUG ");
    qDebug(" ");
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


