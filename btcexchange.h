#ifndef BTCEXCHANGE_H
#define BTCEXCHANGE_H

#include <QObject>

class BTCexchange
{
public:
    explicit BTCexchange();

private:
    virtual bool rafraichirJson() = 0;


};

#endif // BTCEXCHANGE_H
