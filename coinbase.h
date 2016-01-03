#ifndef COINBASE_H
#define COINBASE_H

#include "btcexchange.h"
#include <QtcpSocket>


class CoinBase : public BTCexchange
{
public:
    CoinBase();
    bool rafraichirJson();

};

#endif // COINBASE_H
