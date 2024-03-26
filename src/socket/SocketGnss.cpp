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
    QSharedPointer<QUdpSocket> socketGnssSynchro_ = QSharedPointer<QUdpSocket>(new QUdpSocket());
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
            auto stocks = readGnssSynchro(datagram.data().data(), datagram.data().size());
            emit sendData(stocks);
        }
    }
    socketGnssSynchro_->close();
}

gnss_sdr::Observables SocketGnss::readGnssSynchro(char *buff, int bytes)
{
    try
    {
        // 解析data到stocks中,因为不是static function，所以无法直接调用
        gnss_sdr::Observables stocks;
        stocks.ParseFromString(std::string(buff,bytes));

        // 将stock解析为结构体
        return stocks;


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

std::vector<ChannelStruct> SocketGnss::parseStruct(const gnss_sdr::Observables& stocks)
{
    std::vector<ChannelStruct> vector;
    vector.clear();
    for (int i = 0; i < stocks.observable_size(); ++i)
    {
        const auto& obs = stocks.observable(i);
        if (obs.fs() != 0){
            ChannelStruct i_struct{
                obs.system(),obs.signal(),obs.prn(),obs.channel_id(),
                obs.acq_delay_samples(),obs.acq_doppler_hz(),
                obs.acq_samplestamp_samples(),obs.acq_doppler_step(),
                obs.flag_valid_acquisition(),obs.fs(),obs.prompt_i(),obs.prompt_q(),
                obs.cn0_db_hz(),obs.carrier_doppler_hz(),obs.carrier_phase_rads(),
                obs.code_phase_samples(),obs.tracking_sample_counter(),
                obs.flag_valid_symbol_output(),obs.correlation_length_ms(),
                obs.flag_valid_word(),obs.tow_at_current_symbol_ms(),
                obs.pseudorange_m(),obs.rx_time(),obs.flag_valid_pseudorange(),
                obs.interp_tow_ms()
            };
            vector.push_back(i_struct);
        }
    }
    return vector;
}
