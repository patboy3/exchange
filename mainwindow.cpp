#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


 BTCexchange *test = new CoinBase();
    test->rafraichirJson();
}

MainWindow::~MainWindow()
{
    delete ui;
}
