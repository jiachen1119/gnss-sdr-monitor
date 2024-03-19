//
// Created by tang on 24-3-18.
//

#ifndef GNSS_SDR_MONITOR_PVTTABLEMODEL_H
#define GNSS_SDR_MONITOR_PVTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QVariant>
#include <qdebug.h>
#include "monitor_pvt.pb.h"
#include "lib/PVTStruct.h"
#include <QFont>

class PVTTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PVTTableModel(QObject *parent);

    // override part
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void updatePVT(const gnss_sdr::MonitorPvt& in);
public slots:
    void update();
private:
    PVTStruct pvtStruct_{};
};


#endif  // GNSS_SDR_MONITOR_PVTTABLEMODEL_H
