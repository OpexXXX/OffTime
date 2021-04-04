#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include "treemodel.h"
#include <QTreeView>
#include <QPushButton>
#include <QDebug>
#include <QFileSystemModel>

#include <QtSql>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("topo.db");
//VS010-0006185 ВЛ-10 кВ ф.32-14 Полтавка

    if (!sdb.open()) {
        qDebug()<<sdb.lastError().text();
    }
    QSqlQuery query("SELECT * FROM topo");

    QHash<QString, QStringList> hash;

     QRegExp exPS( "PS[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9]");
     QRegExp exTP( "TP[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
     QRegExp exVS( "VS[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
     QRegExp exVN( "VN[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");

    while (query.next()) { //PS035-000455
        QStringList valueSt;
        valueSt<<query.value(4).toString()<<query.value(5).toString()<<query.value(6).toString()<<query.value(7).toString();
        hash.insert(query.value(4).toString(),valueSt);
        if(exPS.exactMatch(query.value(4).toString())) qDebug() << "matched" << query.value(5).toString();
        if(exVS.exactMatch(query.value(4).toString())) qDebug() << "matched" << query.value(5).toString();
    }
    qDebug()<<"db read";
    TreeModel* model = new TreeModel(hash);
    ui->treeView->setModel(model);

}

MainWindow::~MainWindow()
{
    delete ui;
}

