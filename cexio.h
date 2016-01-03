#ifndef CEXIO_H
#define CEXIO_H

#include "btcexchange.h"

//just a commit test

class CexIO : public BTCexchange
{

public:
    explicit CexIO();

     bool rafraichirJson();


};

#endif // CEXIO_H
