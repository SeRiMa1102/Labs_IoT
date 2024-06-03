#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"


#include <QStandardItemModel>
#include <string>
#include <QLabel>
#include <QMessageBox>
#include <QComboBox>
#include <QDebug>
#include <cmath>
#include <string>
#include <iomanip>
#include <QInputDialog>

size_t SIZE_OF_BUFFER = 500;
int count_input = 0;
const QString prav = "0123456789abcdef";


unsigned char Crc8(const QByteArray& array)
{
    unsigned char crc = 0xFF;
    unsigned int i;

    int len = array.length()-3;
    int g = 2;
    while (len--)
    {
        crc ^= array[g++];

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_input(new Console),
    m_output(new Console),
    m_settings(new SettingsDialog),
    m_serial(new QSerialPort(this)),
    model(new QStandardItemModel())
{

    m_ui->setupUi(this);

    pars.moveToThread(&thread1);
    thread1.start();

    QStandardItem *parentItem = model->invisibleRootItem();

    int j = 0;
    while ( j<128){
        for (int i = 0; i < 16; ++i) {
            QStandardItem *item = new QStandardItem(QString("Bank %0").arg(i));
            parentItem->appendRow(item);
            for (int c = 0; c < 8; c++){
                std::stringstream stream;
                stream << std::setfill('0') << std::setw(2) << std::hex << j;
                QStandardItem *item_1 = new QStandardItem(QString("Adress 0x")+QString::fromStdString(stream.str()));
                item->appendRow(item_1);
                QStandardItem *item_2 = new QStandardItem(QString("Value 0x00"));
                item_2->setData(QString("00"));
                item_1->appendRow(item_2);
                for (int p = 0; p < 8; p++){
                    QStandardItem *item_3 = new QStandardItem(QString("Bit%0: 0").arg(p));
                    item_3->setData(0);
                    item_2->appendRow(item_3);
                }
                j++;
            }
        }
    }


    m_ui->treeView->setModel(0);
    m_ui->treeView->setHeaderHidden(true);
    m_ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();



    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);

    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);


    connect(this, &MainWindow::new_data, &pars, &Parser::input_pars);

    connect(m_output, &Console::getData, this, &MainWindow::writeData);
    connect(&pars, &Parser::putting_Data, this, &MainWindow::putData);
    connect(&pars, &Parser::emitting_error, this, &MainWindow::showError);
}

MainWindow::~MainWindow()
{
    thread1.exit();
    delete m_settings;
    delete m_ui;
}


void MainWindow::openSerialPort()
{
    pars.clearAll();

    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_output->setEnabled(true);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2")
                          .arg(p.name).arg(p.stringBaudRate));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
    m_serial->clear();

    char data[1];
    data[0]=255;
    m_serial->write(data, 1);
    qDebug() << "write";

}

void MainWindow::closeSerialPort()
{

    m_ui->treeView->setModel(0);
    if (m_serial->isOpen())
        m_serial->close();
    m_output->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
    pars.clearAll();
}

void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}


void MainWindow::putData(QByteArray data){

    QStandardItem *parentItem = model->invisibleRootItem();
    parentItem = parentItem->child(static_cast<uint8_t>(data[0])/8);//Bank
    parentItem = parentItem->child(static_cast<uint8_t>(data[0])%8);//Adress
    parentItem = parentItem->child(0);//value

    std::stringstream stream;
    stream << std::setfill('0') <<std::setw(2) << std::hex <<static_cast<uint16_t>(data[1]&0xff);


    parentItem->setData(QString::fromStdString(stream.str()));
    parentItem->setText(parentItem->text().replace(8,2,QString::fromStdString(stream.str())));

    int x = static_cast<uint16_t>(data[1]&0xff);
    for (int i = 0 ; i < 8; i++){
        QChar c = x%2 ? '1' : '0';
        parentItem->child(i)->setText(parentItem->child(i)->text().replace(6,1,c));
        parentItem->child(i)->setData(x%2);
        x = x/2;
    }

    if (static_cast<uint16_t>(data[0]&0xff)==127){
        m_ui->treeView->setModel(model);
    }


}

void MainWindow::showError(){
   QMessageBox::critical(this, tr("Error"), m_serial->errorString());
}

void MainWindow::readData()
{

    //if (m_serial->bytesAvailable() > 5000){
        emit new_data(m_serial->readAll());
    //}

    //pars.addToBuffer(m_serial->readAll());
    //pars.parseInput();

}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}


void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::fillPortsInfo);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_input, &Console::clear_mine);
    connect(m_ui->actionClear, &QAction::triggered, m_output, &Console::clear_mine);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}




void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{

    QStandardItem *item = model->itemFromIndex(index);

    if (item->data().isNull()){
        return;
    }

    bool ok;

    QString text;


    if (item->data().typeName() == QString("QString")) {

        text = QInputDialog::getText(this, item->parent()->text().remove(0, 7),
                                     QString("New value"), QLineEdit::Normal,
                                     QString(item->data().toString()),&ok);

        while (ok && ((text.length()!=2) ||  (!prav.count(text[0])) || (!prav.count(text[1])) ))
            text = QInputDialog::getText(this, item->parent()->text().remove(0, 7),
                                         QString("New value"), QLineEdit::Normal,
                                         QString(item->data().toString()),&ok);


        if (ok){

            if (item->data() == text) return;

            unsigned int x1;
            std::stringstream ss1;
            ss1 << std::hex << item->parent()->text().remove(0, 9).toStdString();
            ss1 >> x1;


            item->setText(QString("Value 0x")+text);
            item->setData(text);


            unsigned int x;
            std::stringstream ss;
            ss << std::hex << text.toStdString();
            ss >> x;

            char data[2];
            data[0]=static_cast<int>(x1);
            data[1]= static_cast<int>(x);

            m_serial->write(data,2);

            for (int i = 0 ; i < 8; i++){
                QChar c = x%2 ? '1' : '0';
                item->child(i)->setText(item->child(i)->text().replace(6,1,c));
                item->child(i)->setData(x%2);
                x = x/2;
            }
            return;
        }





    } else {

        QStringList items;
        items << tr("0") << tr("1");

        text = QInputDialog::getItem(this, item->parent()->parent()->text().remove(0, 7),
                                             item->text().remove(4,5), items, item->data().toInt(), false, &ok);

        if (item->data() == (text=='0'?0:1)) return;

        item->setData(text=='0'?0:1);
        item->setText(item->text().replace(6,1,text[0]));

        item = item->parent();
        int res = 0;
        for (int i = 7 ; i >= 0; i--){
            res *= 2;
            res+=item->child(i)->data().toInt();

        }
        std::stringstream stream;
        stream << std::setfill('0') <<std::setw(2) << std::hex <<res;



        item->setData(QString::fromStdString(stream.str()));
        item->setText(item->text().replace(8,2,QString::fromStdString(stream.str())));

        unsigned int x1;
        std::stringstream ss1;
        ss1 << std::hex << item->parent()->text().remove(0, 9).toStdString();
        ss1 >> x1;

        unsigned int x;
        std::stringstream ss;
        ss << std::hex << item->data().toString().toStdString();
        ss >> x;

        char data[2];
        data[0]=static_cast<int>(x1);
        data[1]= static_cast<int>(x);

        m_serial->write(data,2);



    }
}
