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

    /*


# Requires python-requests. Install with pip:
#
#   pip install requests
#
# or, with easy-install:
#
#   easy_install requests

import json, hmac, hashlib, time, requests, base64
from requests.auth import AuthBase

# Create custom authentication for Exchange
class CoinbaseExchangeAuth(AuthBase):
    def __init__(self, api_key, secret_key, passphrase):
        self.api_key = api_key
        self.secret_key = secret_key
        self.passphrase = passphrase

    def __call__(self, request):
        timestamp = str(time.time())
        message = timestamp + request.method + request.path_url + (request.body or '')
        hmac_key = base64.b64decode(self.secret_key)
        signature = hmac.new(hmac_key, message, hashlib.sha256)
        signature_b64 = signature.digest().encode('base64').rstrip('\n')

        request.headers.update({
            'CB-ACCESS-SIGN': signature_b64,
            'CB-ACCESS-TIMESTAMP': timestamp,
            'CB-ACCESS-KEY': self.api_key,
            'CB-ACCESS-PASSPHRASE': self.passphrase,
            'Content-Type': 'application/json'
        })
        return request

api_url = 'https://api.exchange.coinbase.com/'
auth = CoinbaseExchangeAuth(API_KEY, API_SECRET, API_PASS)

# Get accounts
r = requests.get(api_url + 'accounts', auth=auth)
print r.json()
# [{"id": "a1b2c3d4", "balance":...

# Place an order
order = {
    'size': 1.0,
    'price': 1.0,
    'side': 'buy',
    'product_id': 'BTC-USD',
}
r = requests.post(api_url + 'orders', json=order, auth=auth)
print r.json()
# {"id": "0428b97b-bec1-429e-a94c-59992926778d"}



      */

    // http://stackoverflow.com/questions/12487620/correct-format-for-http-post-using-qnetworkrequest

    // Build your JSON string as usual
    QByteArray jsonString = "{\"method\":\"AuthenticatePlain\",\"loginName\":\"username@domain.com\",\"password\":\"mypass\"}";

    // For your "Content-Length" header
    QByteArray postDataSize = QByteArray::number(jsonString.size());

    // Time for building your request
    QUrl serviceURL("https://api.exchange.coinbase.com/");
    QNetworkRequest request(serviceURL);

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    request.setRawHeader("CB-ACCESS-SIGN", "My app name v0.1");
    request.setRawHeader("CB-ACCESS-TIMESTAMP",  QDateTime::currentDateTime().toString().toLatin1());
    request.setRawHeader("CB-ACCESS-KEY", apiKey.toLatin1());
    request.setRawHeader("CB-ACCESS-PASSPHRASE", postDataSize);
    request.setRawHeader("Content-Type", "application/json");


   // qDebug() << request;

    // Use QNetworkReply * QNetworkAccessManager::post(const QNetworkRequest & request, const QByteArray & data); to send your request. Qt will rearrange everything correctly.
    // QNetworkReply * reply = m_qnam->post(request, jsonString);


    return false;
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


