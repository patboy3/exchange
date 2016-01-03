#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // Crée un test
    BTCexchange *test = new CoinBase();
    test->rafraichirOrderBook();
}

MainWindow::~MainWindow()
{
    delete ui;
}
