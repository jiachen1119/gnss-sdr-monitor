/*!
 * \file telnet_manager.cpp
 * \brief Implementation of a connection manager class for sending
 * commands to the receiver through telnet.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
 *
 */


#include "telnet_manager.h"

TelnetManager::TelnetManager(QObject *parent) : QObject(parent)
{
    tcpSocket_ = new QTcpSocket(this);
    connect(tcpSocket_, &QIODevice::readyRead, this, &TelnetManager::readResponse);

    // Forward signals from QAbstractSocket.
    connect(tcpSocket_, &QAbstractSocket::connected, this, &TelnetManager::connected);
    connect(tcpSocket_, &QAbstractSocket::disconnected, this, &TelnetManager::disconnected);
    connect(tcpSocket_, &QAbstractSocket::stateChanged, this, &TelnetManager::stateChanged);
    connect(tcpSocket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TelnetManager::error);
}

void TelnetManager::setAddress(const QString& addr_str)
{
    hostAddress_.setAddress(addr_str);
}

void TelnetManager::setPort(const QString& port_str)
{
    port_ = port_str.toInt();
}

QHostAddress TelnetManager::getAddress() const
{
    return hostAddress_;
}

quint16 TelnetManager::getPort() const
{
    return port_;
}

QAbstractSocket::SocketState TelnetManager::getState() const
{
    return tcpSocket_->state();
}

void TelnetManager::connectTcp()
{
    tcpSocket_->connectToHost(hostAddress_, port_);
}

void TelnetManager::disconnectTcp()
{
    tcpSocket_->disconnectFromHost();
}

bool TelnetManager::sendCommand(Command cmd, QString args)
{
    if (tcpSocket_->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray data;
        switch (cmd)
        {
        case Command::Reset:
            data = "reset\r\n";
            break;
        case Command::Standby:
            data = "standby\r\n";
            break;
        case Command::ColdStart:
            data = "coldstart\r\n";
            break;
        case Command::WarmStart:
            data = QString("%1 %2 %3").arg("warmstart", args, "\r\n").toUtf8();
            break;
        case Command::HotStart:
            data = QString("%1 %2 %3").arg("hotstart", args, "\r\n").toUtf8();
            break;
        case Command::Status:
            data = "status\r\n";
            break;
        case Command::Exit:
            data = "exit\r\n";
            break;
        default:
            data = "";
            break;
        }

        if (!data.isEmpty())
        {
            tcpSocket_->write(data, data.size());
            if (tcpSocket_->waitForBytesWritten())
            {
                emit txData(data);
                return true;
            }
        }
    }
    return false;
}

void TelnetManager::readResponse()
{
    QByteArray data = tcpSocket_->readAll();
    emit rxData(data);
}
