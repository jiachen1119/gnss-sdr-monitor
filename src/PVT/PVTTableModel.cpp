//
// Created by tang on 24-3-18.
//

#include "PVTTableModel.h"
PVTTableModel::PVTTableModel(QObject *parent) : QAbstractTableModel(parent){
}

QVariant PVTTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole){
        try
        {
            switch (index.row())
            {
            case 0:
                return pvtStruct_.week;
            case 1:
                return pvtStruct_.tow;
            case 2:
                return 0;
            case 3:
                return pvtStruct_.valid_satellite;
            case 4:
                return QString::number(pvtStruct_.latitude,'f',6);
            case 5:
                return QString::number(pvtStruct_.longitude,'f',6);
            case 6:
                return QString::number(pvtStruct_.height,'f',3);;
            case 7:
                return QString::number(pvtStruct_.x,'f',3);
            case 8:
                return QString::number(pvtStruct_.y,'f',3);;
            case 9:
                return QString::number(pvtStruct_.z,'f',3);;
            case 10:
                return QString::number(pvtStruct_.vx,'f',3);;
            case 11:
                return QString::number(pvtStruct_.vy,'f',3);;
            case 12:
                return QString::number(pvtStruct_.vz,'f',3);;
            default:
                return QVariant::Invalid;
            }
        }
        catch (const std::exception &exception)
        {
            qDebug() << exception.what();
            return QVariant::Invalid;
        }
    }
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return QVariant::Invalid;
}

QVariant PVTTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole){
        if(orientation == Qt::Vertical){
            switch (section)
            {
            case 0:
                return QStringLiteral("GPS Week");
            case 1:
                return QStringLiteral("GPS TOW (ms)");
            case 2:
                return QStringLiteral("GPS Time");
            case 3:
                return QStringLiteral("Sat Used");
            case 4:
                return QStringLiteral("Latitude (°)");
            case 5:
                return QStringLiteral("Longitude (°)");
            case 6:
                return QStringLiteral("Height (m)");
            case 7:
                return QStringLiteral("x (m)");
            case 8:
                return QStringLiteral("y (m)");
            case 9:
                return QStringLiteral("z (m)");
            case 10:
                return QStringLiteral("Vx (m/s)");
            case 11:
                return QStringLiteral("Vy (m/s)");
            case 12:
                return QStringLiteral("Vz (m/s)");
            default:
                return QVariant::Invalid;
            }
        }
    }
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return QVariant::Invalid;
}
int PVTTableModel::rowCount(const QModelIndex &parent) const
{
    return 13;
}
int PVTTableModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

void PVTTableModel::updatePVT(const gnss_sdr::MonitorPvt &in)
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
    pvtStruct_ = output;
}

void PVTTableModel::update()
{
    beginResetModel();
    endResetModel();
}
