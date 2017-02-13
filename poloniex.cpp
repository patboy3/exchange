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

    //QByteArray postData = "command=returnBalances&nonce="+QString::number(nonce).toLatin1();
    QByteArray postData = "command=returnCompleteBalances&nonce="+QString::number(nonce).toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    interpreterLoadBalance(&request, &postData);
}

void Poloniex::viewOpenOrder()
{
    //
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "tradingApi";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray postData = "command=returnOpenOrders&nonce="+QString::number(nonce).toLatin1()+ "&currencyPair=BTC_" + currentCurrencyMinor.toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    interpreterLookOrders(&request, &postData, QNetworkAccessManager::PostOperation);
}

int Poloniex::buyOrder(double amount, double price)
{
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "tradingApi";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray postData = "command=buy&nonce="+QString::number(nonce).toLatin1()+"&currencyPair=BTC_" + currentCurrencyMinor.toLatin1() + "&rate=" + QString::number(price).toLatin1() + "&amount=" + QString::number(amount).toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    return interpreterBuySell(&request, BTCexchange::typeBuy, &price, &amount, &postData);
}

int Poloniex::sellOrder(double amount, double price)
{
    QByteArray jsonString = "";
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "tradingApi";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray postData = "command=sell&nonce="+QString::number(nonce).toLatin1()+"&currencyPair=BTC_" + currentCurrencyMinor.toLatin1() + "&rate=" + QString::number(price).toLatin1() + "&amount=" + QString::number(amount).toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    return interpreterBuySell(&request, BTCexchange::typeSell, &price, &amount, &postData);
}

bool Poloniex::cancelOrder(QString orderID)
{
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "tradingApi";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray postData = "command=cancelOrder&nonce="+QString::number(nonce).toLatin1()+ "&orderNumber=" + orderID.toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);

    return interpreterCancelOrders(&request, &postData, &orderID, QNetworkAccessManager::PostOperation);
}

void Poloniex::lookOrder(QString orderID)
{
    //
    // Url de la requete

    qint64 nonce = QDateTime::currentMSecsSinceEpoch();

    QString urlPath = "tradingApi";

    QNetworkRequest request(QUrl(m_apiUrl + urlPath));

    QByteArray postData = "command=returnOrderTrades&nonce="+QString::number(nonce).toLatin1()+ "&orderNumber=" + orderID.toLatin1();
    //QByteArray postData = "command=returnOpenOrders&nonce="+QString::number(nonce).toLatin1()+ "&currencyPair=BTC_" + currentCurrencyMinor.toLatin1();

    signerHeaders(&request, QString(QString::number(nonce)), &urlPath, &postData);


    QEventLoop eventLoop;

    QNetworkAccessManager networkManager;

    QNetworkReply *reply = networkManager.post(request, postData);
    QObject::connect(&networkManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec(); // blocks stack until "finished()" has been called

    QString testt = reply->readAll();

    qDebug() << postData;
    qDebug() << testt;
}

QList<orders>* Poloniex::interpreterLookOrders(QNetworkRequest* request, QByteArray *jsonString, QNetworkAccessManager::Operation operation)
{
    QList<orders> exchangeOrders;

    QEventLoop eventLoop;

        // "quit()" the event-loop, when the network request "finished()"
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        QNetworkReply *reply;
        if (operation == QNetworkAccessManager::PostOperation)
            reply = mgr.post(*request, *jsonString);
        else if (operation == QNetworkAccessManager::GetOperation)
            reply = mgr.get(*request);
        eventLoop.exec(); // blocks stack until "finished()" has been called


        if (errorRequete(reply))
        {
            delete reply;
            return new QList<orders>;
        }
        else
        {
            //faut grabber toute les id dispo et les comparer avec m_orders
            //faut retourner la différence !(action passé sur le site)
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());

            QJsonArray jsonArray = jsonDocument.array();

            foreach (const QJsonValue & value, jsonArray) {
                QJsonObject obj = value.toObject();
                orders now;
                now.order_id = obj["orderNumber"].toString();
                exchangeOrders.append(now);

                qDebug() << obj["orderNumber"].toString();
            }

            delete reply;
            return &exchangeOrders;
        }
}

int Poloniex::interpreterBuySell(QNetworkRequest* request, QString type, double *price, double *amount, QByteArray *jsonString)
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

        if (price != 0)
        {
            if (jsonObject.value("orderNumber").toString() != "")
            {
                orders current;
                current.amount = *amount;
                current.price = *price;
                current.order_id = jsonObject.value("orderNumber").toString();
                current.type = type;

                m_orders.append(current);
            }
        }

        //faut metre le id pi le tick tant que y etre
        m_query->exec("INSERT INTO "+ type +" (OrderId, Tick, Amount, Price) VALUES ('" + jsonObject.value("orderNumber").toString() + "', "+ QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000) +", "+QString::number(*amount)+", "+QString::number(*price)+");");

        m_query->exec("SELECT ID from " + type + " where OrderID = '" + jsonObject.value("orderNumber").toString() + "'");

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

    QJsonObject jsonObject_BTC = jsonObject["BTC"].toObject();
    QJsonObject jsonObject_minorCurrency = jsonObject[currentCurrencyMinor].toObject();

    m_balance_btc = jsonObject_BTC["available"].toString().replace(',','.').toDouble();
    m_balance_btcHold = jsonObject_BTC["onOrders"].toString().replace(',','.').toDouble();
    m_balance_fiat = jsonObject_minorCurrency["available"].toString().replace(',','.').toDouble();
    m_balance_fiatHold = jsonObject_minorCurrency["onOrders"].toString().replace(',','.').toDouble();

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
