/*!
 * \file main_window.h
 * \brief Interface of the main window of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 * Kepeng Luan changed, Southeast University, 2023
 */



#ifndef GNSS_SDR_MONITOR_MAIN_WINDOW_H_
#define GNSS_SDR_MONITOR_MAIN_WINDOW_H_

#include "altitude_widget.h"
#include "channel_table_model.h"
#include "dop_widget.h"
#include "gnss_synchro.pb.h"
#include "monitor_pvt.pb.h"
#include "monitor_pvt_wrapper.h"
#include "telecommand_widget.h"
#include <QAbstractTableModel>
#include <QChart>
#include <QChartView>
#include <QMainWindow>
#include <QQuickWidget>
#include <QSettings>
#include <QTimer>
#include <QXYSeries>
#include <QtNetwork/QUdpSocket>

namespace Ui
{
class MainWindow;
}

namespace QtCharts
{
class QChart;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    gnss_sdr::Observables readGnssSynchro(char buff[], int bytes);
    gnss_sdr::MonitorPvt readMonitorPvt(char buff[], int bytes);
    void saveSettings();
    void loadSettings();

public slots:
    void toggleCapture();
    void receiveGnssSynchro();
    void receiveMonitorPvt();
    void clearEntries();
    void quit();
    void showPreferences();
    void setPort();
    void expandPlot(const QModelIndex &index);
    void closePlots();
    void deletePlots();
    void about();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void updateChart(QtCharts::QChart *chart, QtCharts::QXYSeries *series, const QModelIndex &index);

    Ui::MainWindow *ui;

    QDockWidget *mapDockWidget_;
    QDockWidget *telecommandDockWidget_;
    QDockWidget *altitudeDockWidget_;
    QDockWidget *m_DOPDockWidget;

    QQuickWidget *mapWidget_;
    TelecommandWidget *telecommandWidget_;
    AltitudeWidget *altitudeWidget_;
    DOPWidget *DOPWidget_;

    ChannelTableModel *model_;
    QUdpSocket *m_socketGnssSynchro;
    QUdpSocket *m_socketMonitorPvt;
    gnss_sdr::Observables m_stocks;
    MonitorPvtWrapper *monitorPvtWrapper_;
    gnss_sdr::MonitorPvt m_monitorPvt;
    std::vector<int> m_channels;
    quint16 m_portGnssSynchro;
    quint16 m_portMonitorPvt;
    QSettings m_settings;
    QTimer m_updateTimer;

    QAction *m_start;
    QAction *m_stop;
    QAction *clear_;
    QAction *m_closePlotsAction;

    int m_bufferSize;

    std::map<int, QtCharts::QChartView *> m_plotsConstellation;
    std::map<int, QtCharts::QChartView *> m_plotsCn0;
    std::map<int, QtCharts::QChartView *> m_plotsDoppler;
};

#endif  // GNSS_SDR_MONITOR_MAIN_WINDOW_H_
