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


void CoinBase::signerHeaders(QNetworkRequest *requete){

    QString timeStamp = QDateTime::currentDateTime().toString();

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    requete->setRawHeader("CB-ACCESS-KEY", "SBH5GeIntChyxpax");
    requete->setRawHeader("CB-ACCESS-SIGN", "My app name v0.1");
    requete->setRawHeader("CB-ACCESS-TIMESTAMP",  timeStamp.toLatin1());
    requete->setRawHeader("CB-ACCESS-PASSPHRASE", 0);

}


void CoinBase::loadBalance(){

    qDebug() << "Load";
    if(1 == 1){

        //------------QString reponse = reply->readAll();


        // Gestion des erreurs
        //--------------if(reply->error())
        {
            //-----------qDebug() << reply->errorString();
            //QMessageBox msgBox;
            //msgBox.setText("Erreur lors de la requete : " + reponse->errorString());
            //msgBox.exec();
            //return;
        }


    //-----------qDebug() << reponse;


        return;
    }


    QUrl serviceURL("https://api.exchange.coinbase.com/accounts");
    QNetworkRequest request(serviceURL);

    signerHeaders(&request);

    QNetworkAccessManager *m_qnam = new QNetworkAccessManager();
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(loadBalance(QNetworkReply*)));

    m_qnam->get(request);

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


