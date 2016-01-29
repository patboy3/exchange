#include "trade.h"

Trade::Trade(QList<BTCexchange *> *sites)
{
    m_sites = *sites;
    m_minimumTrade = 0.2;
}

QList<struct_profitability>* Trade::calculProfitability(double amount)
{
    //faut calculer la profitabilité de chaque site en prenant compte de la currency
    //1 - 2 . 1 - 3 . 1 - 4 . 2 - 3 . 2 - 4 . 3 - 4

    //faut aller chercher l'order book de toute les site ... et comparer les site entre eux (buy/sell)
    foreach (BTCexchange* solo, m_sites)
    {
        if (*solo->get_apiKey() != "")
        {
            //faudrait partir sa en thread et lui faire attendre que aille toute fini avant le calcul de la profitabilité
            solo->rafraichirOrderBook();
        }
    }

    QList<struct_profitability> *profitability = new QList<struct_profitability>;
    //QList<double> profitability;
    for (int i=0;i<m_sites.count() - 1;i++)
    {

        double buyI(m_sites[i]->get_averagePrice(amount, BTCexchange::typeBuy, true));
        double selI(m_sites[i]->get_averagePrice(amount, BTCexchange::typeSell, true));
        for (int z = i + 1;z<m_sites.count();z++)
        {
            if (*m_sites[i]->get_currentCurrency() == *m_sites[z]->get_currentCurrency())
            {
                double buyZ(m_sites[z]->get_averagePrice(amount, BTCexchange::typeBuy, true));
                double selZ(m_sites[z]->get_averagePrice(amount, BTCexchange::typeSell, true));

                //calcul des profitabilité
                //buy sur la i sell sur la z
                struct_profitability prof1;
                prof1.buyExchange = m_sites[i];
                prof1.buyAverage = buyI;
                prof1.sellExchange = m_sites[z];
                prof1.sellAverage = selZ;
                prof1.profitPourcentage = (selZ / buyI - 1) * 100;
                profitability->append(prof1);

                //profitability.append((selZ / buyI - 1) * 100);
                //qDebug() << QString("profitabilité " + QString::number(profitability.count()) + " (buy sur " + *m_sites[i]->get_sitename() + "_" + *m_sites[i]->get_currentCurrency() + " sell sur " + *m_sites[z]->get_sitename() + "_" + *m_sites[z]->get_currentCurrency() + "): " + QString::number(profitability[profitability.count() - 1]) + "%");

                //buy sur la z sell sur la i
                //profitability.append((selI / buyZ - 1) * 100);
                //qDebug() << QString("profitabilité " + QString::number(profitability.count()) + " (buy sur " + *m_sites[z]->get_sitename() + "_" + *m_sites[z]->get_currentCurrency() + " sell sur " + *m_sites[i]->get_sitename() + "_" + *m_sites[i]->get_currentCurrency() + ") : " + QString::number(profitability[profitability.count() - 1]) + "%");
                struct_profitability prof2;
                prof2.buyExchange = m_sites[z];
                prof1.buyAverage = buyZ;
                prof2.sellExchange = m_sites[i];
                prof1.sellAverage = selI;
                prof2.profitPourcentage = (selI / buyZ - 1) * 100;
                profitability->append(prof2);
            }
        }
    }

    for (int i=0;i<profitability->count();i++)
    {
        qDebug() << QString("profitabilité " + QString::number(i + 1) + " (buy sur " + *(*profitability)[i].buyExchange->get_sitename() + "_" + *(*profitability)[i].buyExchange->get_currentCurrency() + " sell sur " + *(*profitability)[i].sellExchange->get_sitename() + "_" + *(*profitability)[i].sellExchange->get_currentCurrency() + ") : " + QString::number((*profitability)[i].profitPourcentage) + "%");
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

        return true;
    }
}

void Trade::run()
{
    while (true)
    {
        //faudrait trouver le bon amount a mettre (tout fond dispo ? seuelement une parti ?)
        double amount(0.01);
        QList<struct_profitability> *result = calculProfitability(amount);

        //si ya une probabilité positive.... faut lunch le buy ! faut checker si ya les fonds avant

        foreach (struct_profitability solo, *result)
        {
            if (solo.profitPourcentage > m_minimumTrade && checkFunds(amount, solo.buyAverage,solo.buyExchange,solo.sellExchange))
            {
                //clear les fonds ds checkFunds si font dispo en hold !

                //launch le trade faut transformer le prix en btc
                solo.buyExchange->buyOrder(solo.buyAverage * amount);
                solo.sellExchange->sellOrder(amount);

                //faut saver le trade ds la db faut retourner les id des transactions !
                //order id ds buy .. ds sell.. et mettre l'id de buy et sell ds trade
            }
        }

        QThread::sleep(2);
    }


}
