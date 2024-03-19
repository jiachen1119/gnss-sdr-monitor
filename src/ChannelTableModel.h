/*!
 * \file channel_table_model.h
 * \brief Interface of a model for storing the channels information
 * in a table-like data structure.
 */


#ifndef GNSS_SDR_MONITOR_CHANNEL_TABLE_MODEL_H_
#define GNSS_SDR_MONITOR_CHANNEL_TABLE_MODEL_H_

#include "gnss_synchro.pb.h"
#include <boost/circular_buffer.hpp>
#include <QAbstractTableModel>
#include <QTimer>
#include "Variance.h"

typedef boost::circular_buffer<double> CBuff;

class ChannelTableModel : public QAbstractTableModel
{
public:
    //initialize the channel of gnss signals
    ChannelTableModel();

    // update the print
    void update();

    void populateChannels(const gnss_sdr::Observables *m_stocks);
    void populateChannel(const gnss_sdr::GnssSynchro *ch);
    void clearChannel(int ch_id);
    void clearChannels();

    // 检查通道是否存在失锁退出的情况
    void checkChannels();

    QString getSignalPrettyName(const gnss_sdr::GnssSynchro *ch);
    int getColumns() const;
    void setBufferSize();
    int getChannelId(int row);

    // List of virtual functions that must be implemented in a read-only table model.
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    gnss_sdr::GnssSynchro getChannelData(int key);

protected:
    int columns_;
    int bufferSize_;
    gnss_sdr::Observables m_stocks;

    QTimer checkTimer_;
    std::map<int,double> checkNum_;

    std::vector<int> channelsId_;
    std::map<int, gnss_sdr::GnssSynchro> channels_;
    std::map<int, QString> channelsSignal_;
    std::map<int, boost::circular_buffer<double>> channelsTime_;
    std::map<int, boost::circular_buffer<double>> channelsI_;
    std::map<int, boost::circular_buffer<double>> channelsQ_;
    std::map<int, boost::circular_buffer<double>> channelsCn0_;
    std::map<int, boost::circular_buffer<double>> channelsDoppler_;

private:
    std::map<std::string, QString> mapSignalName_;
};

#endif  // GNSS_SDR_MONITOR_CHANNEL_TABLE_MODEL_H_
