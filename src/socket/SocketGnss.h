//
// Created by tang on 24-2-29.
//

#ifndef GNSS_SDR_MONITOR_SOCKETGNSS_H
#define GNSS_SDR_MONITOR_SOCKETGNSS_H


#include "gnss_synchro.pb.h"
#include <QThread>
#include <QUdpSocket>
#include <QNetworkDatagram>

class SocketGnss: public QThread
{
    Q_OBJECT
public:
    explicit SocketGnss(QObject *parent= nullptr,quint16 port = 1111);
    void readGnssSynchro(char buff[], int bytes);
    void setPort(quint16 port);

public slots:
    void stopThread();

signals:
    void sendData(gnss_sdr::Observables stocks);

protected:
    void run() override;

private:
    gnss_sdr::Observables stocks_;
    quint16 port_;
    bool threadStop_= false;

};


#endif  // GNSS_SDR_MONITOR_SOCKETGNSS_H
