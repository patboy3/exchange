#include "threadorderbook.h"


ThreadOrderBook::ThreadOrderBook(BTCexchange* site)
{
    this->site = site;
}

void ThreadOrderBook::setSite(BTCexchange* site)
{
    this->site = site;
}

void ThreadOrderBook::run()
{

    if(site->rafraichirOrderBook())
        emit reussi();

    else
        emit erreur();

}

