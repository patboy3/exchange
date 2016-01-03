#ifndef BTCEXCHANGE_H
#define BTCEXCHANGE_H

#include <QObject>

class BTCexchange
{
public:
    explicit BTCexchange();


    virtual bool rafraichirJson() = 0;

private:


};

#endif // BTCEXCHANGE_H
