#include "mainwindow.h"
#include <QApplication>

#include "btcexchange.h"
#include "coinbase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    BTCexchange *test = new CoinBase();

    test->rafraichirJson();

    w.show();


    return a.exec();
}
