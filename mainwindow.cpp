#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // Crée un test
    BTCexchange *test = new CoinBase("CAD", "SBH5GeIntChyxpax","G1WGo4vRMRleNVEkssfuhs8fDpT3UQ8T");
    test->rafraichirOrderBook();

    BTCexchange *test2 = new Quadriga("CAD","","");
    test2->rafraichirOrderBook();
}

MainWindow::~MainWindow()
{
    delete ui;
}
