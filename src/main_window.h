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
#include "socket/SocketGnss.h"
#include "styles/TabBarStyle.h"


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
    ~MainWindow() override;

    gnss_sdr::MonitorPvt readMonitorPvt(char buff[], int bytes);
    void saveSettings();
    void loadSettings();

public slots:
    void toggleCapture();
    void receiveGnssSynchro(gnss_sdr::Observables stocks);
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
    static void updateChart(QtCharts::QChart *chart, QtCharts::QXYSeries *series, const QModelIndex &index);
    static void updateCnoChart(QtCharts::QChart *chart, QtCharts::QXYSeries *series, const QModelIndex &index);
    void  tabBarSetting();

    Ui::MainWindow *ui;

    int screenWidth_;
    int screenHeight_;

    QQuickWidget *mapWidget_;


    TelecommandWidget *telecommandWidget_;
    AltitudeWidget *altitudeWidget_;
    DOPWidget *DOPWidget_;

    ChannelTableModel *model_;

    std::unique_ptr<SocketGnss> socketGnssSynchro_;
    QUdpSocket *socketMonitorPvt_;
    gnss_sdr::Observables stocks_;
    MonitorPvtWrapper *monitorPvtWrapper_;
    gnss_sdr::MonitorPvt m_monitorPvt;
    std::vector<int> m_channels;
    quint16 m_portGnssSynchro;
    quint16 m_portMonitorPvt;
    QSettings settings_;
    QTimer updateTimer_;

    QAction *start_;
    QAction *stop_;
    QAction *clear_;
    QAction *closePlotsAction_;

    int bufferSize_;

    std::map<int, QtCharts::QChartView *> plotsConstellation_;
    std::map<int, QtCharts::QChartView *> plotsCn0_;
    std::map<int, QtCharts::QChartView *> plotsDoppler_;
};

#endif  // GNSS_SDR_MONITOR_MAIN_WINDOW_H_
