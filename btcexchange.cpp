#include "btcexchange.h"


QString BTCexchange::typeBuy = "buy";
QString BTCexchange::typeSell = "sell";

BTCexchange::BTCexchange(QString currencyMinor, QString liveApiKey, QString liveSecretKey, QSqlQuery *query)
{
    m_balance_fiat = 0;
    m_balance_fiatHold = 0;
    m_balance_btc = 0;
    m_balance_btcHold = 0;

    apiKey = liveApiKey;
    secretKey = liveSecretKey;
    currentCurrencyMinor = currencyMinor;

    m_query = query;
}

QByteArray* BTCexchange::hmacSignature(QByteArray *message, QCryptographicHash::Algorithm method, bool secretKeyIsBase64)
{
    QByteArray key;

    if (!secretKeyIsBase64)
        key = secretKey.toLatin1();
    else
    {
        QByteArray ba;
        ba.append(secretKey);

        key = QByteArray::fromBase64(ba);
    }

    QMessageAuthenticationCode code(method);
    code.setKey(key);
    code.addData(*message);

    return new QByteArray(code.result());
}

bool BTCexchange::interpreterCancelOrders(QNetworkRequest* request, QByteArray *jsonString, QString *id_Orders, QNetworkAccessManager::Operation operation)
{
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkReply *reply;
    if (operation == QNetworkAccessManager::PostOperation)
        reply = mgr.post(*request, *jsonString);
    else if (operation == QNetworkAccessManager::DeleteOperation)
        reply = mgr.deleteResource(*request);

    eventLoop.exec(); // blocks stack until "finished()" has been called


    if (errorRequete(reply))
    {
        qDebug() << false;
        delete reply;
        return false;
    }
    else
    {
        QString resultat = reply->readAll();

        if (resultat != "OK" && resultat != "\"true\"")
        {
            qDebug() << false;
            delete reply;
            return false;
        }

        qDebug() << true;
        //faut le deleter de la list
        if (*id_Orders == "")
            m_orders.clear();
        else
        {
            for (int i=0; i<m_orders.count(); i++)
            {
                if (m_orders[i].order_id == *id_Orders)
                {
                    m_orders.removeAt(i);
                    i--;
                }
            }
        }

        delete reply;
    }

    foreach (orders solo, m_orders)
    {
        qDebug() << "cancel" << solo.type << " - id : "  << solo.order_id;
        qDebug() << "cancel" << solo.type << " - price : "  << solo.price;
        qDebug() << "cancel" << solo.type << " - amount : "  << solo.amount;
    }

    return true;
}

int BTCexchange::interpreterBuySell(QNetworkRequest* request, QString type, double *price, double *amount, QByteArray *jsonString)
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
            if (jsonObject.value("id").toString() != "")
            {
                orders current;
                current.amount = *amount;
                current.price = *price;
                current.order_id = jsonObject.value("id").toString();
                current.type = type;

                m_orders.append(current);
            }
        }

        //faut metre le id pi le tick tant que y etre
        m_query->exec("INSERT INTO "+ type +" (OrderId, Tick, Amount, Price) VALUES ('" + jsonObject.value("id").toString() + "', "+ QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000) +", "+QString::number(*amount)+", "+QString::number(*price)+");");

        m_query->exec("SELECT ID from " + type + " where OrderID = '" + jsonObject.value("id").toString() + "'");

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

QList<orders>* BTCexchange::interpreterLookOrders(QNetworkRequest* request, QByteArray *jsonString, QNetworkAccessManager::Operation operation)
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
                now.order_id = obj["id"].toString();
                exchangeOrders.append(now);
            }

            delete reply;
        }

        //faut comparer aik le m_orders et renvoyer si yen une une de retirer

        QList<orders> *executedOrders = new QList<orders>;

        for (int i=0; i<m_orders.count();i++)
        {
            bool present(false);
            for (int z =0;z<exchangeOrders.count();z++)
            {
                if (exchangeOrders[z].order_id == m_orders[i].order_id)
                {
                    present = true;
                    z = z<exchangeOrders.count();
                }
            }

            if (!present)
            {
                orders now;
                now.order_id = m_orders[i].order_id;

                executedOrders->append(now);

                m_orders.removeAt(i);
                i--;
            }

        }

        foreach (orders solo, *executedOrders)
        {
            qDebug() <<  "executed - id : "  << solo.order_id;
        }

        return executedOrders;
}

void BTCexchange::get_averagePrice(double amount, QString type, double *result, bool includeFees)
{
    int iteration = 0;
    double currentAmount(0);
    double spendMoney(0);

    QList<OrderBookElement>* now;

    if (type == BTCexchange::typeBuy)
    {
        now = &m_asks;
    }
    else if (type == BTCexchange::typeSell)
    {
        now = &m_bids;
    }

    while (currentAmount < amount && iteration != (*now).count())
    {
        if (currentAmount + (*now)[iteration].nbBtc < amount)
        {
            spendMoney = spendMoney + ((*now)[iteration].prixVente * (*now)[iteration].nbBtc);
            currentAmount = currentAmount + (*now)[iteration].nbBtc;
        }
        else
        {
            spendMoney = spendMoney + ((*now)[iteration].prixVente * (amount - currentAmount));
            currentAmount = currentAmount + (amount - currentAmount);
            result[1] = (*now)[iteration].prixVente;
        }

        iteration++;
    }

    double averagePrice(spendMoney / currentAmount);

    //qDebug() << m_siteName << ": averageprice (nofee) - " << type << " : " << averagePrice << "(" << currentAmount << " BTC)";

    if (includeFees)
    {
        if (type == BTCexchange::typeBuy)
            averagePrice = averagePrice + (m_feeTaker / 100 * averagePrice);
        else if (type == BTCexchange::typeSell)
            averagePrice = averagePrice - (m_feeTaker / 100 * averagePrice);
    }

    qDebug() << m_siteName << get_currentCurrencyMinor() << ": averagePrice - " << type << " : " << averagePrice << "(" << currentAmount << " BTC)";


    result[0] = averagePrice;
    //return averagePrice;
}

QList<orders>* BTCexchange::get_orders()
{
    return &m_orders;
}

QString BTCexchange::get_currentCurrencyMinor()
{
    return currentCurrencyMinor;
}

QString* BTCexchange::get_apiKey()
{
    return &apiKey;
}

double* BTCexchange::get_balance_fiat()
{
    return &m_balance_fiat;
}

double* BTCexchange::get_balance_fiatHold()
{
    return &m_balance_fiatHold;
}

double* BTCexchange::get_balance_btc()
{
    return &m_balance_btc;
}

double* BTCexchange::get_balance_btcHold()
{
    return &m_balance_btcHold;
}

QString* BTCexchange::get_sitename()
{
    return &m_siteName;
}

bool BTCexchange::errorRequete(QNetworkReply* reply)
{
    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();
        //delete reply;
        return true;
    }

    return false;
}

bool BTCexchange::interpreterOrderBook(QNetworkRequest* request)
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

        QJsonArray asks = jsonObject["asks"].toArray();
        m_asks.clear();


        foreach (const QJsonValue & value, asks)
        {
            OrderBookElement now;

            if (value.toArray()[1].toString().replace(',','.').toDouble() != 0)
                now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
            else
                now.nbBtc = value.toArray()[1].toDouble();

            now.prixVente = value.toArray()[0].toString().replace(',','.').toDouble();

            m_asks.append(now);
        }

        QJsonArray bids = jsonObject["bids"].toArray();
        m_bids.clear();


        foreach (const QJsonValue & value, bids)
        {
            OrderBookElement now;

            if (value.toArray()[1].toString().replace(',','.').toDouble() != 0)
                now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
            else
                now.nbBtc = value.toArray()[1].toDouble();

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



bool BTCexchange::rafraichirOrderBook()
{
    QNetworkRequest *request = new QNetworkRequest;
    // Url de la requete
    request->setUrl(QUrl(orderBookAddr));

    return interpreterOrderBook(request);
}

BTCexchange::~BTCexchange()
{

}
