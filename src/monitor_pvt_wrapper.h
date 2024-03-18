/*!
 * \file monitor_pvt_wrapper.h
 * \brief Interface of a wrapper class for MonitorPvt objects.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
 */


#ifndef GNSS_SDR_MONITOR_MONITOR_PVT_WRAPPER_H_
#define GNSS_SDR_MONITOR_MONITOR_PVT_WRAPPER_H_

#include "monitor_pvt.pb.h"
#include <boost/circular_buffer.hpp>
#include <QObject>
#include <QVariant>
#include "lib/Variance.h"

class MonitorPvtWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant position READ position NOTIFY dataChanged)
    Q_PROPERTY(QVariantList path READ path NOTIFY dataChanged)

public:
    explicit MonitorPvtWrapper(QObject *parent = nullptr);
    void addMonitorPvt(const gnss_sdr::MonitorPvt &monitor_pvt);

    gnss_sdr::MonitorPvt getLastMonitorPvt();

    QVariant position() const;
    QVariantList path() const;

    struct Coordinates
    {
        double latitude;
        double longitude;
    };

signals:
    void dataChanged();
    void altitudeChanged(qreal newTow, qreal newAltitude);
    void dopChanged(qreal newTow, qreal newGdop, qreal newPdop, qreal newHdop, qreal newVdop);

public slots:
    void clearData();
    void setBufferSize(size_t size);

private:
    size_t bufferSize_;
    boost::circular_buffer<gnss_sdr::MonitorPvt> bufferMonitorPvt_;
    boost::circular_buffer<Coordinates> path_;
};

#endif  // GNSS_SDR_MONITOR_MONITOR_PVT_WRAPPER_H_
