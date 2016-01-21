#include "btcexchange.h"



BTCexchange::BTCexchange(QString currency, QString liveApiKey, QString liveSecretKey)
{
    m_balance_fiat = 0;
    m_balance_fiatHold = 0;
    m_balance_btc = 0;
    m_balance_btcHold = 0;
    typeBuy = "buy";
    typeSell = "sell";

    apiKey = liveApiKey;
    secretKey = liveSecretKey;
    currentCurrency = currency;
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

bool BTCexchange::interpreterOrders(QNetworkRequest* request, QString type, double *price, double *amount, QByteArray *jsonString)
{
    QEventLoop eventLoop;

        // "quit()" the event-loop, when the network request "finished()"
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        QNetworkReply *reply = mgr.post(*request, *jsonString);
        eventLoop.exec(); // blocks stack until "finished()" has been called


        if (errorRequete(reply))
        {
            delete reply;
            return false;
        }
        else
        {
            if (price != 0)
            {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
                QJsonObject jsonObject = jsonDocument.object();

                orders current;
                current.amount = *amount;
                current.price = *price;
                current.order_id = jsonObject.value("id").toString();
                current.type = type;

                m_orders.append(current);
            }
            else
            {
                //trade direct .. faudrait le noté !

            }

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

        return true;
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

bool BTCexchange::errorRequete(QNetworkReply* reply)
{
    if(reply->error())
    {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de la requete : " + reply->errorString());
        msgBox.exec();

        delete reply;
        return true;
    }

    return false;
}

void BTCexchange::interpreterOrderBook(QNetworkReply* reply)
{
    // Gestion des erreurs
    if (errorRequete(reply))
        return;

    // Laleur de reply->readAll() se vide apres usage
    QString reponse = reply->readAll();

    // Crée un object Json avec la réponse obtenure
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reponse.toUtf8());

    QJsonObject jsonObject = jsonDocument.object();

    QJsonArray asks = jsonObject["asks"].toArray();
    m_asks.clear();;
    foreach (const QJsonValue & value, asks)
    {
        OrderBookElement now;
        now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
        now.prixVente = value.toArray()[0].toString().replace(',','.').toDouble();

        m_asks.append(now);
    }

    QJsonArray bids = jsonObject["bids"].toArray();
    m_bids.clear();
    foreach (const QJsonValue & value, bids)
    {
        OrderBookElement now;
        now.nbBtc = value.toArray()[1].toString().replace(',','.').toDouble();
        now.prixVente = value.toArray()[0].toString().replace(',','.').toDouble();

        m_bids.append(now);
    }

    foreach (OrderBookElement solo, m_bids)
    {
        qDebug() << "bids - btc : "  << solo.nbBtc;
        qDebug() << "bids - price : "  << solo.prixVente;
    }

    foreach (OrderBookElement solo, m_asks)
    {
        qDebug() << "asks - btc : "  << solo.nbBtc;
        qDebug() << "asks - price : "  << solo.prixVente;
    }

    delete reply;

}

bool BTCexchange::rafraichirOrderBook()
{

    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest *request = new QNetworkRequest();

    // Url de la requete
    request->setUrl(QUrl(orderBookAddr));

    // Connecte le signal Finished du networkManaget au Slot lireJsonFinished
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(interpreterOrderBook(QNetworkReply*)));

    // Lance la requete pour obtenir la réponse
    networkManager->get(*request);

    return false;
}

BTCexchange::~BTCexchange()
{

}
