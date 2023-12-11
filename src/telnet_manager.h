/*!
 * \file telnet_manager.h
 * \brief Interface of a connection manager class for sending
 * commands to the receiver through telnet.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
 *
 */


#ifndef GNSS_SDR_MONITOR_TELNET_MANAGER_H_
#define GNSS_SDR_MONITOR_TELNET_MANAGER_H_

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>

class TelnetManager : public QObject
{
    Q_OBJECT

public:
    //设置一个command枚举
    enum class Command
    {
        Reset,
        Standby,
        ColdStart,
        WarmStart,
        HotStart,
        Status,
        Exit
    };

    explicit TelnetManager(QObject *parent = nullptr);

    void setAddress(const QString& addr_str);
    void setPort(const QString& port_str);

    [[nodiscard]] QHostAddress getAddress() const;
    [[nodiscard]] quint16 getPort() const;
    [[nodiscard]] QAbstractSocket::SocketState getState() const;

signals:
    void txData(QByteArray data);
    void rxData(QByteArray data);

    // Forwarded from QAbstractSocket.
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void error(QAbstractSocket::SocketError socketError);

public slots:
    void connectTcp();
    void disconnectTcp();
    bool sendCommand(Command cmd, QString args = "");
    void readResponse();

private:
    //socket class
    QTcpSocket *tcpSocket_;
    //the address and port to used by socket
    QHostAddress hostAddress_;
    quint16 port_;
};

#endif  // GNSS_SDR_MONITOR_TELNET_MANAGER_H_
