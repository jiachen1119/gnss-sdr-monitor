#include "SocketPVT.h"

SocketPVT::SocketPVT(QObject *parent,quint16 port): QThread(parent)
{
    port_=port;
}

void SocketPVT::run()
{
    QSharedPointer<QUdpSocket> socketPvt = QSharedPointer<QUdpSocket>(new QUdpSocket());
    socketPvt->abort();
    // 必须要加上延时环节
    QThread::msleep(500);
    if (!socketPvt->bind(QHostAddress::LocalHost, port_)) {
        qDebug() << "Failed to bind UDP socket to port " << port_;
        return;
    }

    while (socketPvt->state() == QAbstractSocket::BoundState&&!threadStop_){
        while (socketPvt->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = socketPvt->receiveDatagram();
            auto output = readGnssSynchro(datagram.data().data(), datagram.data().size());
            emit sendData(output);
        }
    }
    socketPvt->close();
}

PVTStruct SocketPVT::readGnssSynchro(char *buff, int bytes)
{
    try
    {
        // 解析data到stocks中,因为不是static function，所以无法直接调用
        gnss_sdr::MonitorPvt stocks;
        stocks.ParseFromString(std::string(buff,bytes));

        // 将stock解析为结构体
        return parseStruct(stocks);
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }
    return PVTStruct{};
}

void SocketPVT::setPort(quint16 port)
{
    port_=port;
}

void SocketPVT::stopThread()
{
    threadStop_= true;
    // 加入延时的原因就是防止窗口关闭先于线程关闭
    QThread::msleep(500);
}

PVTStruct SocketPVT::parseStruct(const gnss_sdr::MonitorPvt& in)
{
    PVTStruct output{
        in.tow_at_current_symbol_ms(),
        in.week(),
        in.rx_time(),
        in.user_clk_offset(),
        in.pos_x(),in.pos_y(),in.pos_z(),
        in.vel_x(),in.vel_y(),in.vel_z(),
        in.cov_xx(),in.cov_yy(),in.cov_zz(),
        in.cov_xy(),in.cov_yz(),in.cov_zx(),
        in.latitude(),in.longitude(),in.height(),
        in.valid_sats(),in.solution_status(),in.solution_type(),
        in.ar_ratio_factor(),in.ar_ratio_threshold(),
        in.gdop(),in.pdop(),in.hdop(),in.vdop()
    };
    return output;
}
