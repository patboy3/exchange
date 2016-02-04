#ifndef THEADORDERBOOK_H
#define THEADORDERBOOK_H

#include <QThread>
#include "btcexchange.h"

class ThreadOrderBook : public QThread
{

    Q_OBJECT

public:
    ThreadOrderBook(BTCexchange* site = 0);
    void setSite(BTCexchange* site);


public slots :
        void run();

private :
        BTCexchange* site;

signals:
    void reussi();
    void erreur();

};

#endif // THEADORDERBOOK_H
