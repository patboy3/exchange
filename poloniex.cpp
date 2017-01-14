#include "poloniex.h"


Poloniex::Poloniex(QString currency, QString apiKey, QString secretKey, QString passphrase, QSqlQuery *query) : BTCexchange(currency, apiKey, secretKey, query)
{
    currentCurrencyMinor = currencyNameModification();

    m_apiUrl = "https://poloniex.com/";
    if (currentCurrencyMinor.contains("USD"))
        orderBookAddr = m_apiUrl + "public?command=returnOrderBook&currencyPair=" + currentCurrencyMinor + "_BTC&depth=10";
    else
        orderBookAddr = m_apiUrl + "public?command=returnOrderBook&currencyPair=BTC_" + currentCurrencyMinor + "&depth=10";
    m_passphrase = passphrase;
    m_siteName = "poloniex";

    m_feeMaker = 0.15;
    m_feeTaker = 0.25;

}

QString Poloniex::get_currentCurrencyMinor()
{
    return currencyNameModification(true);
}

QString Poloniex::currencyNameModification(bool remove)
{
    if (!remove)
    {
        if (currentCurrencyMinor.contains("USD"))
            return currentCurrencyMinor + "T";
        else
            return currentCurrencyMinor;
    }
    else
    {
        if (currentCurrencyMinor.contains("USD"))
            return currentCurrencyMinor.mid(0, currentCurrencyMinor.size() - 1);
        else
            return currentCurrencyMinor;
    }
}


void Poloniex::signerHeaders(QNetworkRequest *requete, QString timeStamp, QString *requestPath, QByteArray *postData){

    QByteArray message =  *postData;

    requete->setRawHeader("content-type", "application/x-www-form-urlencoded");
    requete->setRawHeader("Key", apiKey.toLatin1());

    //HMAC512 (postdata) avec secret key decoded... on remet en hex
    requete->setRawHeader("Sign", (*hmacSignature(&message,QCryptographicHash::Sha512,false)).toHex());
}

void Poloniex::loadBalance(){

    QByteArray jsonString = "";
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "tradingApi";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray postData = "command=returnBalances&nonce="+QString::number(nonce).toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    interpreterLoadBalance(&request, &postData);

}




void Poloniex::viewOpenOrder()
{
}

int Poloniex::buyOrder(double amount, double price)
{
    return 1;
}

int Poloniex::sellOrder(double amount, double price)
{
    return 1;
}

bool Poloniex::cancelOrder(QString orderID)
{
    return false;
}

void Poloniex::lookOrder(QString orderID)
{

}

void Poloniex::interpreterLoadBalance(QNetworkRequest* request, QByteArray *postData)
{
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkReply *reply = mgr.post(*request, *postData);

    eventLoop.exec(); // blocks stack until "finished()" has been called


    QString reponse = reply->readAll();

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());

    QJsonObject jsonObject = jsonDocument.object();


    QJsonObject premierPalier = jsonObject["result"].toObject();

    m_balance_btc = premierPalier["XXBT"].toString().replace(',','.').toDouble();
    m_balance_fiat = premierPalier[currentCurrencyMinor].toString().replace(',','.').toDouble();

    /*
    foreach (const QJsonValue &value, json_array)
    {
       QJsonObject json_obj = value.toObject();

       if (json_obj["currency"].toString() == currentCurrency)
       {
           m_balance_fiat = json_obj["available"].toString().replace(',','.').toDouble();
           m_balance_fiatHold = json_obj["hold"].toString().replace(',','.').toDouble();
        }
        else if (json_obj["currency"].toString() == "XXBT")
        {
           m_balance_btc = json_obj["available"].toString().replace(',','.').toDouble();
           m_balance_btcHold = json_obj["hold"].toString().replace(',','.').toDouble();
        }
    }*/

    qDebug() << "Poloniex";
    qDebug() << "btc" << m_balance_btc;
    qDebug() << "btcHold"  << m_balance_btcHold;
    qDebug() << currencyNameModification(true).toLatin1().data() << m_balance_fiat;
    qDebug() << (currencyNameModification(true) + "Hold").toLatin1().data()  << m_balance_fiatHold;


    delete reply;
}

void Poloniex::interpreterCrap(QNetworkReply* reply)
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

    delete reply;
}
