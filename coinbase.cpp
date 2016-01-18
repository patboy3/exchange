#include "coinbase.h"


CoinBase::CoinBase(QString currency, QString apiKey, QString secretKey, QString passphrase) : BTCexchange(currency, apiKey, secretKey)
{
    m_apiUrl = "https://api.exchange.coinbase.com";
    orderBookAddr = m_apiUrl + "/products/BTC-" + currentCurrency + "/book?level=2";
    m_passphrase = passphrase;
}


void CoinBase::signerHeaders(QNetworkRequest *requete, QString timeStamp, QString method, QString *requestPath){


    QByteArray message = (timeStamp +method + *requestPath).toLatin1();

    requete->setRawHeader("CB-ACCESS-KEY", apiKey.toLatin1());
    requete->setRawHeader("CB-ACCESS-SIGN", (*hmacSignature(&message,QCryptographicHash::Sha256,true)).toBase64());
    requete->setRawHeader("CB-ACCESS-TIMESTAMP",  (timeStamp).toLatin1());
    requete->setRawHeader("CB-ACCESS-PASSPHRASE", m_passphrase.toLatin1());

}


void CoinBase::interpreterLoadBalance(QNetworkReply *reply)
{
    QString reponse = reply->readAll();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        //QMessageBox msgBox;
        //msgBox.setText("Erreur lors de la requete : " + reponse->errorString());
        //msgBox.exec();
        //return;
    }

    qDebug() << reponse;

}


void CoinBase::loadBalance(){

    QString urlPath = "/accounts";
    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    signerHeaders(&request, QString(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)), QString("GET"), &urlPath);

    QNetworkAccessManager *m_qnam = new QNetworkAccessManager();
    connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(interpreterLoadBalance(QNetworkReply*)));

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

void CoinBase::viewOpenOrder()
{}


void CoinBase::buyOrder(double amount, double price)
{}

void CoinBase::sellOrder(double amount, double price)
{}

void CoinBase::cancelOrder(QString orderID)
{}

void CoinBase::lookOrder(QString orderID)
{}
