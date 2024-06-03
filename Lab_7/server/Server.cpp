#include "Server.h"
#include <QDebug>

Server::Server(){

    if (this->listen(QHostAddress::Any, 2388)){
        qDebug() << "start";
    } else {
        qDebug() << "error";
    }
    nextSize = 0;
}

Server::~Server(){
    qDebug() << "end";
}

void Server::sendToClient(QString str){
    data.clear();
    QDataStream output(&data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_7);

    output << quint16(0) << QTime::currentTime() << str;
    output.device()->seek(0);
    output << quint16(data.size() - sizeof(quint16));

    for (int i = 0; i < sockets.size(); i++){
        sockets[i]->write(data);
    }
}

void Server::incomingConnection(qintptr socketDescriptor){
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    sockets.push_back(socket);
    qDebug() << "client connected: " << socketDescriptor;
}
void Server::slotReadyRead(){
    socket = (QTcpSocket*)sender();
    QDataStream input(socket);

    input.setVersion(QDataStream::Qt_6_7);

    if (input.status() ==QDataStream::Ok){
        qDebug() << "reading...";
        while (true){
            if (nextSize == 0){
                qDebug() << "nextSize == 0";
                if (socket->bytesAvailable() < 2){
                    break;
                }
                input >> nextSize;
                qDebug() << "nextSize == " << nextSize;
            }

            if (socket->bytesAvailable() < nextSize){
                qDebug() << "Not full";
                break;
            }
            QString str;
            QTime time;
            input >> time >> str;
            nextSize = 0;
            qDebug() << str << "  OP";
            sendToClient(str);
            break;
        }
    } else {
        qDebug() << "QDataStream error";
    }
}

