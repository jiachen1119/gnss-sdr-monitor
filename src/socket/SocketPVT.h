//
// Created by tang on 24-3-26.
//

#ifndef GNSS_SDR_MONITOR_SOCKETPVT_H
#define GNSS_SDR_MONITOR_SOCKETPVT_H

#include "monitor_pvt.pb.h"
#include "PVTStruct.h"
#include <QThread>
#include <QUdpSocket>
#include <QNetworkDatagram>

class SocketPVT: public QThread
{
    Q_OBJECT
public:
    explicit SocketPVT(QObject *parent= nullptr,quint16 port = 1111);
    PVTStruct readGnssSynchro(char buff[], int bytes);
    void setPort(quint16 port);

public slots:
    void stopThread();

signals:
    void sendData(PVTStruct);

protected:
    void run() override;

private:
    quint16 port_;
    bool threadStop_= false;

    static PVTStruct parseStruct(const gnss_sdr::MonitorPvt& in);

};


#endif  // GNSS_SDR_MONITOR_SOCKETPVT_H
