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
void MonitorPvtWrapper::addMonitorPvt(const PVTStruct &struct_in)
{
    bufferMonitorPvt_.push_back(struct_in);

    Coordinates coord{struct_in.latitude,struct_in.longitude};
    path_.push_back(coord);

    emit dataChanged();
    emit altitudeChanged(struct_in.tow, struct_in.height);
    emit dopChanged(struct_in.tow, struct_in.gdop, struct_in.pdop,
        struct_in.hdop, struct_in.vdop);
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
        return QVariant::fromValue(QGeoCoordinate(pvt.latitude, pvt.longitude));
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
