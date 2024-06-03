#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "modelmy.h"
#include "delegate.h"

#include <QTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    dataMap["Root"] << "Item 1" << "Item 2";
    dataMap["Item 1"] << "Subitem 1";
    dataMap["Item 2"] << "Subitem 2";

    model = new CustomModel(dataMap, this);
    Delegate *delegate = new Delegate(this);

    ui->treeView->setModel(model);
    ui->treeView->setItemDelegate(delegate);
    ui->treeView->expandAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString count = QString::number(QTime::currentTime().second());
    dataMap["Root"] << "New Item " + count;
    dataMap["New Item " + count] << "Subitem " + count;

    model->updateData(dataMap);

    ui->treeView->expandAll();
}

