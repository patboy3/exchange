#include "kraken.h"


Kraken::Kraken(QString currency, QString apiKey, QString secretKey, QString passphrase, QSqlQuery *query) : BTCexchange(currency, apiKey, secretKey, query)
{

    currentCurrencyMinor = currencyNameModification(currentCurrencyMinor);

    m_apiUrl = "https://api.kraken.com";
    if (currentCurrencyMinor[0] == 'Z')
        orderBookAddr = m_apiUrl + "/0/public/Depth?pair=XBT" + currencyNameModification(currentCurrencyMinor,true) + "&count=10";
    else
        orderBookAddr = m_apiUrl + "/0/public/Depth?pair=" + currencyNameModification(currentCurrencyMinor,true) + "XBT&count=10";
    m_passphrase = passphrase;
    m_siteName = "kraken";

    m_feeMaker = 0.16;
    m_feeTaker = 0.26;

}

QString Kraken::get_currentCurrencyMinor()
{
    return currencyNameModification(currentCurrencyMinor,true);
}

QString Kraken::currencyNameModification(QString currency, bool remove)
{
    if (!remove)
    {
        if (currency == "CAD" || currency == "USD") //SI fiat ... commence par Z
        {
            return "Z" + currency;
        }
        else
            return "X" + currency;
    }
    else
        return currency.mid(1);
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
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "/0/private/AddOrder";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    //XXMRXXBT

    QString majorCurrency = currencyNameModification("XBT");

    QByteArray postData;
    if (currentCurrencyMinor[0] != 'Z')
        postData = "nonce="+QString::number(nonce).toLatin1() + "&pair=" + currentCurrencyMinor.toLatin1() + majorCurrency.toLatin1() + "&type=buy&ordertype=limit&price=" + QString::number(price).toLatin1() + "&volume=" + QString::number(amount).toLatin1();
    else
        postData = "nonce="+QString::number(nonce).toLatin1() + "&pair=" + majorCurrency.toLatin1() + currentCurrencyMinor.toLatin1() + "&type=buy&ordertype=limit&price=" + QString::number(price).toLatin1() + "&volume=" + QString::number(amount).toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    return interpreterBuySell(&request, BTCexchange::typeBuy, &price, &amount, &postData);
}

int Kraken::sellOrder(double amount, double price)
{
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "/0/private/AddOrder";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    //XXMRXXBT

    QString majorCurrency = currencyNameModification("XBT");

    QByteArray postData;
    if (currentCurrencyMinor[0] != 'Z')
        postData = "nonce="+QString::number(nonce).toLatin1() + "&pair=" + currentCurrencyMinor.toLatin1() + majorCurrency.toLatin1() + "&type=sell&ordertype=limit&price=" + QString::number(price).toLatin1() + "&volume=" + QString::number(amount).toLatin1();
    else
        postData = "nonce="+QString::number(nonce).toLatin1() + "&pair=" + majorCurrency.toLatin1() + currentCurrencyMinor.toLatin1() + "&type=sell&ordertype=limit&price=" + QString::number(price).toLatin1() + "&volume=" + QString::number(amount).toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    return interpreterBuySell(&request, BTCexchange::typeSell, &price, &amount, &postData);
}

bool Kraken::cancelOrder(QString orderID)
{
    return false;
}

void Kraken::lookOrder(QString orderID)
{

}

int Kraken::interpreterBuySell(QNetworkRequest* request, QString type, double *price, double *amount, QByteArray *jsonString)
{
    int returnID(-1);
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;

    QNetworkReply *reply = mgr.post(*request, *jsonString);

    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec(); // blocks stack until "finished()" has been called


    if (errorRequete(reply))
    {
        delete reply;
        //delete request;
        return returnID;
    }
    else
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObject = jsonDocument.object();
        QJsonObject jsonObject2 = jsonObject["result"].toObject();
        QJsonArray qArray = jsonObject2.value("txid").toArray();
        QString ID  = qArray.takeAt(0).toString();

        if (price != 0)
        {
            if (ID != "")
            {
                orders current;
                current.amount = *amount;
                current.price = *price;
                current.order_id = ID;
                current.type = type;

                m_orders.append(current);
            }
        }

        //faut metre le id pi le tick tant que y etre
        m_query->exec("INSERT INTO "+ type +" (OrderId, Tick, Amount, Price) VALUES ('" + ID + "', "+ QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000) +", "+QString::number(*amount)+", "+QString::number(*price)+");");

        m_query->exec("SELECT ID from " + type + " where OrderID = '" + ID + "'");

        while (m_query->next()) {
            returnID = m_query->value(0).toInt();
        }

        //delete request;
        delete reply;
    }

    foreach (orders solo, m_orders)
    {
        if (solo.type == type)
        {
            qDebug() << type << " - id : "  << solo.order_id;
            qDebug() << type << " - price : "  << solo.price;
            qDebug() << type << " - amount : "  << solo.amount;
        }
    }

    return returnID;
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
    qDebug() << currencyNameModification(currentCurrencyMinor, true).toLatin1().data() << m_balance_fiat;
    qDebug() << (currencyNameModification(currentCurrencyMinor,true) + "Hold").toLatin1().data()  << m_balance_fiatHold;


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
