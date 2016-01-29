#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QList>
#include <QtSql>
#include <QString>


#include "btcexchange.h"
#include "coinbase.h"
#include "quadriga.h"
#include "trade.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void loadSite();
    QList<BTCexchange*> m_sites;
    void generateDB(QSqlQuery *query);
    void Hmac256(const unsigned char *text,      /* pointer to data stream        */
                 int                 text_len,   /* length of data stream         */
                 const unsigned char *key,       /* pointer to authentication key */
                 int                 key_len    /* length of authentication key  */, void *digest);
    Trade *m_trade;
    QSqlQuery *m_query;
    QSqlDatabase m_db;
public slots:

private slots:
    void on_pushButton_clicked();
    void on_text_amountProfitability_returnPressed();
};

#endif // MAINWINDOW_H
