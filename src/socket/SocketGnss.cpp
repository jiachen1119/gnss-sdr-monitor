//
// Created by tang on 24-2-29.
//

#include "SocketGnss.h"

SocketGnss::SocketGnss(QObject *parent,quint16 port): QThread(parent)
{
    port_=port;
}

void SocketGnss::run()
{
    std::unique_ptr<QUdpSocket> socketGnssSynchro_ = std::make_unique<QUdpSocket>();
    socketGnssSynchro_->abort();
    // 必须要加上延时环节
    QThread::msleep(500);
    if (!socketGnssSynchro_->bind(QHostAddress::LocalHost, port_)) {
        qDebug() << "Failed to bind UDP socket to port " << port_;
        return;
    }

    while (socketGnssSynchro_->state() == QAbstractSocket::BoundState&&!threadStop_){
        while (socketGnssSynchro_->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = socketGnssSynchro_->receiveDatagram();
            readGnssSynchro(datagram.data().data(), datagram.data().size());
            emit sendData(stocks_);
        }
    }
    socketGnssSynchro_->close();
}

void SocketGnss::readGnssSynchro(char *buff, int bytes)
{
    try
    {
        std::string data(buff, bytes);
        stocks_.ParseFromString(data);
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }
}
void SocketGnss::setPort(quint16 port)
{
    port_=port;
}
void SocketGnss::stopThread()
{
    threadStop_= true;
}
