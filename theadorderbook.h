#ifndef THEADORDERBOOK_H
#define THEADORDERBOOK_H

#include <QThread>

class TheadOrderBook : public QThread
{
public:
    TheadOrderBook();

signals:
    void reussi();
    void erreur();

public slots :
        void run();
};

#endif // THEADORDERBOOK_H
