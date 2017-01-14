#include "kraken.h"


Kraken::Kraken(QString currency, QString apiKey, QString secretKey, QString passphrase, QSqlQuery *query) : BTCexchange(currency, apiKey, secretKey, query)
{

    currentCurrencyMinor = currencyNameModification();

    m_apiUrl = "https://api.kraken.com";
    if (currentCurrencyMinor[0] == 'Z')
        orderBookAddr = m_apiUrl + "/0/public/Depth?pair=XBT" + currencyNameModification(true) + "&count=10";
    else
        orderBookAddr = m_apiUrl + "/0/public/Depth?pair=" + currencyNameModification(true) + "XBT&count=10";
    m_passphrase = passphrase;
    m_siteName = "kraken";

    m_feeMaker = 0.16;
    m_feeTaker = 0.26;

}

QString Kraken::get_currentCurrencyMinor()
{
    return currencyNameModification(true);
}

QString Kraken::currencyNameModification(bool remove)
{
    if (!remove)
    {
        if (currentCurrencyMinor == "CAD" || currentCurrencyMinor == "USD")
        {
            return "Z" + currentCurrencyMinor;
        }
        else
            return "X" + currentCurrencyMinor;
    }
    else
        return currentCurrencyMinor.mid(1);
}

void Kraken::signerHeaders(QNetworkRequest *requete, QString timeStamp, QString *requestPath, QByteArray *postData){

    //nonce + les postData
    QByteArray chaine = timeStamp.toLatin1() + *postData;

    //Hash en sha256
    QByteArray hash = QCryptographicHash::hash(chaine, QCryptographicHash::Sha256);

    //ajouter le path au hash
    QByteArray message =  (*requestPath).toUtf8() + hash;

    requete->setRawHeader("content-type", "application/x-www-form-urlencoded");
    requete->setRawHeader("API-Key", apiKey.toLatin1());

    //HMAC256 (path + hash) avec secret key decoded... on remet en base64
    requete->setRawHeader("API-Sign", (*hmacSignature(&message,QCryptographicHash::Sha512,true)).toBase64());
}

void Kraken::loadBalance(){

    QByteArray jsonString = "";
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "/0/private/Balance";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray postData = "nonce="+QString::number(nonce).toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    interpreterLoadBalance(&request, &postData);

}




void Kraken::viewOpenOrder()
{
}

int Kraken::buyOrder(double amount, double price)
{
    return 1;
}

int Kraken::sellOrder(double amount, double price)
{
    return 1;
}

bool Kraken::cancelOrder(QString orderID)
{
    return false;
}

void Kraken::lookOrder(QString orderID)
{

}

bool Kraken::interpreterOrderBook(QNetworkRequest* request)
{
    try
    {

        QEventLoop eventLoop;

        // "quit()" the event-loop, when the network request "finished()"
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        QNetworkReply *reply = mgr.get(*request);

        eventLoop.exec(); // blocks stack until "finished()" has been called


        QString reponse = reply->readAll();

        // Crée un object Json avec la réponse obtenure
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());

        QJsonObject jsonObject = jsonDocument.object();

        QJsonObject premierPalier = jsonObject["result"].toObject();
        QJsonObject deuxiemePalier;
        if (currentCurrencyMinor[0] == 'Z')
            deuxiemePalier= premierPalier["XXBT" + currentCurrencyMinor].toObject();
        else
            deuxiemePalier= premierPalier[currentCurrencyMinor +"XXBT"].toObject();

        QJsonArray asks = deuxiemePalier["asks"].toArray();

        m_asks.clear();


        foreach (const QJsonValue & value, asks)
        {
            OrderBookElement now;
            now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
            now.prixVente = value.toArray()[0].toString().replace(',','.').toDouble();

            m_asks.append(now);
        }

        QJsonArray bids = deuxiemePalier["bids"].toArray();
        m_bids.clear();


        foreach (const QJsonValue & value, bids)
        {
            OrderBookElement now;
            now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
            now.prixVente = value.toArray()[0].toString().replace(',','.').toDouble();

            m_bids.append(now);
        }

        /*
    foreach (OrderBookElement solo, m_bids)
    {
        qDebug() << m_siteName << ": bids - btc : "  << solo.nbBtc;
        qDebug() << m_siteName << ": bids - price : "  << solo.prixVente;
    }

    foreach (OrderBookElement solo, m_asks)
    {
        qDebug() << m_siteName  << ": asks - btc : "  << solo.nbBtc;
        qDebug() << m_siteName  << ": asks - price : "  << solo.prixVente;
    }*/

        delete reply;
    }
    catch (std::exception & e)
    {
        qDebug() << "erreur interpreter OrderBook";
    }


    delete request;

    return true;
}

void Kraken::interpreterLoadBalance(QNetworkRequest* request, QByteArray *postData)
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

    qDebug() << "Kraken";
    qDebug() << "btc" << m_balance_btc;
    qDebug() << "btcHold"  << m_balance_btcHold;
    qDebug() << currencyNameModification(true).toLatin1().data() << m_balance_fiat;
    qDebug() << (currencyNameModification(true) + "Hold").toLatin1().data()  << m_balance_fiatHold;


    delete reply;
}

void Kraken::interpreterCrap(QNetworkReply* reply)
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
