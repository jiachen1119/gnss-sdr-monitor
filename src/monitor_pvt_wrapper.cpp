/*!
 * \file monitor_pvt_wrapper.cpp
 * \brief Implementation of a wrapper class for MonitorPvt objects.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
 *
 */


#include "monitor_pvt_wrapper.h"
#include <QDebug>
#include <QGeoCoordinate>

/*!
 Constructs a MonitorPvtWrapper object.
 */
MonitorPvtWrapper::MonitorPvtWrapper(QObject *parent) : QObject(parent)
{
    bufferSize_ = BUFFER_SIZE_FOR_PVT;

    bufferMonitorPvt_.resize(bufferSize_);
    bufferMonitorPvt_.clear();

    path_.resize(bufferSize_);
    path_.clear();
}

/*!
 Populates the internal data structures with the data of the \a monitor_pvt MonitorPvt object.
 */
void MonitorPvtWrapper::addMonitorPvt(const gnss_sdr::MonitorPvt &monitor_pvt)
{
    bufferMonitorPvt_.push_back(monitor_pvt);

    Coordinates coord{monitor_pvt.latitude(),monitor_pvt.longitude()};
    path_.push_back(coord);

    emit dataChanged();
    emit altitudeChanged(monitor_pvt.tow_at_current_symbol_ms(), monitor_pvt.height());
    emit dopChanged(monitor_pvt.tow_at_current_symbol_ms(), monitor_pvt.gdop(), monitor_pvt.pdop(), monitor_pvt.hdop(), monitor_pvt.vdop());
}

/*!
 Gets the last MonitorPvt object.
 */
gnss_sdr::MonitorPvt MonitorPvtWrapper::getLastMonitorPvt()
{
    return bufferMonitorPvt_.back();
}

/*!
 Clears all the data from the internal data structures.
 */
void MonitorPvtWrapper::clearData()
{
    bufferMonitorPvt_.clear();
    path_.clear();

    emit dataChanged();
}

/*!
 Sets the size of the internal circular buffers that store the data.
 */
void MonitorPvtWrapper::setBufferSize(size_t size)
{
    bufferSize_ = size;
    bufferMonitorPvt_.resize(bufferSize_);
}

/*!
 Returns the last known position.
 */
QVariant MonitorPvtWrapper::position() const
{
    if (!bufferMonitorPvt_.empty())
    {
        auto pvt = bufferMonitorPvt_.back();
        return QVariant::fromValue(QGeoCoordinate(pvt.latitude(), pvt.longitude()));
    }
    else
    {
        return QVariant::fromValue(QGeoCoordinate());
    }
}

/*!
 Returns the path formed by the history of recorded positions.
 */
QVariantList MonitorPvtWrapper::path() const
{
    if (!path_.empty())
    {
        QVariantList list;
        for (auto coord : path_)
        {
            list << QVariant::fromValue(QGeoCoordinate(coord.latitude, coord.longitude));
        }
        return list;
    }
    else
    {
        return QVariantList();
    }
}
