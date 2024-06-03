#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead );
    connect(socket, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);

    nextSize = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotReadyRead()
{
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_7);

    if (input.status() == QDataStream::Ok){

        while (true){
            if (nextSize == 0){
                if (socket->bytesAvailable() < 2){
                    break;
                }
                input >> nextSize;
            }

            if (socket->bytesAvailable() < nextSize){
                break;
            }
            QString str;
            QTime time;
            input >> time >> str;
            nextSize = 0;
            ui->textBrowser->append(time.toString() +  " " + str);
            break;
        }


    } else {
        ui->textBrowser->append("read error");
    }
}

void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1", 2388);
}

void MainWindow::sendToServer(QString str)
{
    data.clear();
    QDataStream output(&data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_7);
    output << quint16(0) << QTime::currentTime() << str;
    output.device()->seek(0);

    output << quint16(data.size() - sizeof(quint16));
    socket->write(data);
    ui->lineEdit->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    sendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
    sendToServer(ui->lineEdit->text());
}

