/*!
 * \file main_window.h
 * \brief Interface of the main window of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 * Kepeng Luan changed, Southeast University, 2023
 */



#ifndef GNSS_SDR_MONITOR_MAIN_WINDOW_H_
#define GNSS_SDR_MONITOR_MAIN_WINDOW_H_

#include "AlertWidget.h"
#include "AltitudeWidget.h"
#include "ChannelTableModel.h"
#include "CustomChannelView.h"
#include "CustomChartView.h"
#include "CustomTabWidget.h"
#include "CustomTableView.h"
#include "CustomToolBar.h"
#include "DopWidget.h"
#include "PVTTableModel.h"
#include "SocketGnss.h"
#include "SocketPVT.h"
#include "TabBarStyle.h"
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void saveSettings();
    void loadSettings();

public slots:
    void toggleCapture();
    void receiveGnssSynchro(const std::vector<ChannelStruct>& vector);
    void receiveMonitorPvt(PVTStruct in);
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
    Ui::MainWindow *ui;

    int screenWidth_;
    int screenHeight_;

    CustomToolBar *custom_tool_bar_;
    CustomTabWidget *tab_widget_;
    CustomChannelView *channel_view_;

    QQuickWidget *map_widget_;

    TelecommandWidget *telecommandWidget_;
    AltitudeWidget *altitude_widget_;
    DOPWidget *dop_widget_;

    ChannelTableModel *channelTableModel_;
    PVTTableModel *pvt_table_model_;

    std::unique_ptr<SocketGnss> socketGnssSynchro_;
    std::unique_ptr<SocketPVT> socketMonitorPvt_;
    MonitorPvtWrapper *monitorPvtWrapper_;
    QSettings settings_;
    QTimer updateTimer_;

    QAction *start_;
    QAction *stop_;
    QAction *clear_;
    QAction *closePlotsAction_;

    std::map<int, CustomChartView *> plotsConstellation_;
    std::map<int, CustomChartView *> plotsCn0_;
    std::map<int, CustomChartView *> plotsDoppler_;
};

#endif  // GNSS_SDR_MONITOR_MAIN_WINDOW_H_
