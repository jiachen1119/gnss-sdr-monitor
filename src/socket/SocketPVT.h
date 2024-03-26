//
// Created by tang on 24-3-26.
//

#ifndef GNSS_SDR_MONITOR_SOCKETPVT_H
#define GNSS_SDR_MONITOR_SOCKETPVT_H

#include "monitor_pvt.pb.h"
#include <QThread>
#include <QUdpSocket>
#include <QNetworkDatagram>

class SocketPVT: public QThread
{
    Q_OBJECT
public:
    explicit SocketPVT(QObject *parent= nullptr,quint16 port = 1111);
    void readPVTSynchro(char buff[], int bytes);
    void setPort(quint16 port);

public slots:
    void stopThread();

signals:
    void sendData(gnss_sdr::MonitorPvt stocks);

protected:
    void run() override;

private:
    gnss_sdr::MonitorPvt stocks_;
    quint16 port_;
    bool threadStop_= false;
};


#endif  // GNSS_SDR_MONITOR_SOCKETPVT_H
