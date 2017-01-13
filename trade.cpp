#include "trade.h"

Trade::Trade(QList<BTCexchange *> *sites, QSqlQuery *query)
{
    m_sites = *sites;
    m_minimumTrade = 0.9;
    m_query = query;
}

QList<struct_profitability>* Trade::calculProfitability(double amount)
{
    //faut calculer la profitabilité de chaque site en prenant compte de la currency
    //1 - 2 . 1 - 3 . 1 - 4 . 2 - 3 . 2 - 4 . 3 - 4

    // Un thread par site pour rafraichir le orderBook
    ThreadOrderBook* threads = new ThreadOrderBook[m_sites.size()];

    short compteur = 0;

    //faut aller chercher l'order book de toute les site ... et comparer les site entre eux (buy/sell)
    foreach (BTCexchange* solo, m_sites)
    {
        if (*solo->get_apiKey() != "")
        {
            threads[compteur].setSite(solo);

            threads[compteur].start();

            compteur ++;

            //faudrait partir sa en thread et lui faire attendre que aille toute fini avant le calcul de la profitabilité
            // solo->rafraichirOrderBook();
        }
    }


    // On attend que nos thread ait fini avant de continuer
    for( compteur-- ; compteur >= 0 ; compteur--)
            threads[compteur].wait();

    delete[] threads;

    QList<struct_profitability> *profitability = new QList<struct_profitability>;

    for (int i=0;i<m_sites.count() - 1;i++)
    {        
        double buyI[2];
        double selI[2];

        m_sites[i]->get_averagePrice(amount, BTCexchange::typeBuy, buyI, true);
        m_sites[i]->get_averagePrice(amount, BTCexchange::typeSell, selI, true);
        for (int z = i + 1;z<m_sites.count();z++)
        {
            if (m_sites[i]->get_currentCurrency() == m_sites[z]->get_currentCurrency())
            {
                double buyZ[2];
                double selZ[2];

                m_sites[z]->get_averagePrice(amount, BTCexchange::typeBuy, buyZ, true);
                m_sites[z]->get_averagePrice(amount, BTCexchange::typeSell, selZ, true);

                //calcul des profitabilité
                //buy sur la i sell sur la z
                struct_profitability prof1;
                prof1.buyMax = buyI[1];
                prof1.buyExchange = m_sites[i];
                prof1.buyAverage = buyI[0];
                prof1.sellExchange = m_sites[z];
                prof1.sellAverage = selZ[0];
                prof1.sellMax = selZ[1];
                prof1.profitPourcentage = (selZ[0] / buyI[0] - 1) * 100;
                profitability->append(prof1);

                //profitability.append((selZ / buyI - 1) * 100);
                //qDebug() << QString("profitabilité " + QString::number(profitability.count()) + " (buy sur " + *m_sites[i]->get_sitename() + "_" + *m_sites[i]->get_currentCurrency() + " sell sur " + *m_sites[z]->get_sitename() + "_" + *m_sites[z]->get_currentCurrency() + "): " + QString::number(profitability[profitability.count() - 1]) + "%");

                //buy sur la z sell sur la i
                //profitability.append((selI / buyZ - 1) * 100);
                //qDebug() << QString("profitabilité " + QString::number(profitability.count()) + " (buy sur " + *m_sites[z]->get_sitename() + "_" + *m_sites[z]->get_currentCurrency() + " sell sur " + *m_sites[i]->get_sitename() + "_" + *m_sites[i]->get_currentCurrency() + ") : " + QString::number(profitability[profitability.count() - 1]) + "%");
                struct_profitability prof2;
                prof2.buyExchange = m_sites[z];
                prof2.buyMax = buyZ[1];
                prof2.buyAverage = buyZ[0];
                prof2.sellExchange = m_sites[i];
                prof2.sellMax = selI[1];
                prof2.sellAverage = selI[0];
                prof2.profitPourcentage = (selI[0] / buyZ[0] - 1) * 100;
                profitability->append(prof2);
            }
        }
    }

    for (int i=0;i<profitability->count();i++)
    {
        qDebug() << QString("profitabilité " + QString::number(i + 1) + " (buy sur " + *(*profitability)[i].buyExchange->get_sitename() + "_" + (*profitability)[i].buyExchange->get_currentCurrency() + " sell sur " + *(*profitability)[i].sellExchange->get_sitename() + "_" + (*profitability)[i].sellExchange->get_currentCurrency() + ") : " + QString::number((*profitability)[i].profitPourcentage) + "%");
    }


    return profitability;

}

bool Trade::checkFunds(double amount, double averageBuyPrice, BTCexchange *buy, BTCexchange *sell)
{
    double fiat = *buy->get_balance_fiat() + *buy->get_balance_fiatHold();
    double btc = *sell->get_balance_btc() + *sell->get_balance_btcHold();

    if (averageBuyPrice * amount < *buy->get_balance_fiat() && amount < *sell->get_balance_btc())
        return true;

    if (fiat < averageBuyPrice * amount || btc < amount)
    {
        qDebug() << "fond insufisant";
        return false;
    }
    else
    {
        qDebug() << "fond insufisant";
        return false;

        /* faut jme mettre un truc pour qui detect si sa fait parti dun trade déja senser etre fait.. si oui le laisser la
        //faut clearer toute les orders pour pouvoir passé elle !
        //faudrait s'Arranger pour calculer optimalement pour deleter jsute les order kon veut (pas implenter)
        QList<orders> *buyOrders = buy->get_orders();
        QList<orders> *sellOrders = sell->get_orders();

        foreach (orders solo, *buyOrders)
        {
            buy->cancelOrder(solo.order_id);
        }

        foreach (orders solo, *sellOrders)
        {
            sell->cancelOrder(solo.order_id);
        }

        delete buyOrders;
        delete sellOrders;
        return true;*/
    }
}

void Trade::run()
{
    while (true)
    {
        //faudrait trouver le bon amount a mettre (tout fond dispo ? seuelement une parti ?)
        double amount(0.012);

        QList<struct_profitability> *result = calculProfitability(amount);

        //si ya une probabilité positive.... faut lunch le buy ! faut checker si ya les fonds avant

        foreach (struct_profitability solo, *result)
        {
            if (solo.profitPourcentage > m_minimumTrade && checkFunds(amount, solo.buyAverage,solo.buyExchange,solo.sellExchange))
            {
                emit updatePos();
                //clear les fonds ds checkFunds si font dispo en hold !

                //int buyID(1);
                //int sellID(2);
                int buyID = solo.buyExchange->buyOrder(amount, solo.buyMax);
                int sellID = solo.sellExchange->sellOrder(amount, solo.sellMax);

                //faut saver le trade ds la db faut retourner les id des transactions !
                //order id ds buy .. ds sell.. et mettre l'id de buy et sell ds trade
                m_query->exec("INSERT INTO trade (ID_Buy, ID_Sell, Profitability) VALUES (" + QString::number(buyID) + ", " + QString::number(sellID) + ", " + QString::number(solo.profitPourcentage) + ");");

                //faut retirer les 2 order de leur variable respective !
                QThread::sleep(1);

                //updater la balance des 2 sites !
                solo.buyExchange->loadBalance();
                solo.sellExchange->loadBalance();
            }
            else if (solo.profitPourcentage > m_minimumTrade)
            {
                emit updateFunds();
            }
            else
            {
                emit updateNeg();
            }
        }

        delete result;
        QThread::sleep(2);
    }


}
