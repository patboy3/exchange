#ifndef CEXIO_H
#define CEXIO_H

#include "btcexchange.h"

class CexIO : public BTCexchange
{

public:
    explicit CexIO(QString adresseAPI);

     bool rafraichirJson();


};

#endif // CEXIO_H
