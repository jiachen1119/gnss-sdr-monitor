/*!
 * \file main_window.h
 * \brief Interface of the main window of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 * Kepeng Luan changed, Southeast University, 2023
 */



#ifndef GNSS_SDR_MONITOR_MAIN_WINDOW_H_
#define GNSS_SDR_MONITOR_MAIN_WINDOW_H_

#include "AltitudeWidget.h"
#include "ChannelTableModel.h"
#include "CustomChartView.h"
#include "CustomTableView.h"
#include "PVTTableModel.h"
#include "SocketGnss.h"
#include "dop_widget.h"
#include "monitor_pvt.pb.h"
#include "monitor_pvt_wrapper.h"
#include "styles/TabBarStyle.h"
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
    void receiveGnssSynchro(const std::vector<ChannelStruct>& vector);
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
    void  tabBarSetting();

    Ui::MainWindow *ui;

    int screenWidth_;
    int screenHeight_;

    QQuickWidget *mapWidget_;


    TelecommandWidget *telecommandWidget_;
    AltitudeWidget *altitudeWidget_;
    DOPWidget *DOPWidget_;

    ChannelTableModel *channelTableModel_;
    PVTTableModel *pvtTableModel_;

    std::unique_ptr<SocketGnss> socketGnssSynchro_;
    QUdpSocket *socketMonitorPvt_;
    MonitorPvtWrapper *monitorPvtWrapper_;
    gnss_sdr::MonitorPvt monitorPvt_;
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

    std::map<int, CustomChartView *> plotsConstellation_;
    std::map<int, CustomChartView *> plotsCn0_;
    std::map<int, CustomChartView *> plotsDoppler_;
};

#endif  // GNSS_SDR_MONITOR_MAIN_WINDOW_H_
