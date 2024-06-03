#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTime>

class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();
    virtual ~Server() override;
    QTcpSocket *socket;

private:
    QVector<QTcpSocket*> sockets;
    QByteArray data;
    quint16 nextSize;

    void sendToClient(QString str);

public slots:
    virtual void incomingConnection(qintptr socketDescriptor) override;
    void slotReadyRead();
};
