/*!
 * \file main_window.cpp
 * \brief Implementation of the main window of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 * Kepeng Luan changed, Southeast University, 2023-2024
 */

#include "main_window.h"
#include "CustomTabWidget.h"
#include "Cn0Delegate.h"
#include "ConstellationDelegate.h"
#include "DopplerDelegate.h"
#include "LedDelegate.h"
#include "preferences_dialog.h"
#include "ui_main_window.h"
#include <QDebug>
#include <QQmlContext>
#include <QtCharts>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // 获取屏幕尺寸，为后续expandPlot做准备
    QScreen *screen=QGuiApplication::primaryScreen();
    QRect size=screen->availableGeometry();
    screenHeight_=size.height();
    screenWidth_=size.width();

    // 表格更新计时器
    updateTimer_.setInterval(500);
    updateTimer_.setSingleShot(true);
    connect(&updateTimer_, &QTimer::timeout, [this] { channelTableModel_->update(); });
    connect(&updateTimer_, &QTimer::timeout, [this] { pvt_table_model_->update(); });

    // UI设置
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("GNSS SDR Monitor designed by SEU Tang"));

    //    QPalette palette;
    //    palette.setColor(QPalette::Window,Qt::black); // 设置背景色
    //    palette.setColor(QPalette::WindowText, Qt::white); // 设置前景色（文本颜色）
    //    ui->tabWidget_main->setPalette(palette);

    // 添加tab
    tab_widget_ = new CustomTabWidget();
    ui->gridLayout->addWidget(tab_widget_);

    // QTableView.
    channel_view_ = new CustomChannelView(tab_widget_->widget(TAB_CHANNEL));
    // Model.
    channelTableModel_ = new ChannelTableModel();
    channel_view_->setModel(channelTableModel_);
    // add delegates to channel view
    channel_view_->setItemDelegateForColumn(CHANNEL_CONSTELLATION, new ConstellationDelegate());
    channel_view_->setItemDelegateForColumn(CHANNEL_CN0, new Cn0Delegate());
    channel_view_->setItemDelegateForColumn(CHANNEL_DOPPLER, new DopplerDelegate());
    channel_view_->setItemDelegateForColumn(CHANNEL_TLM, new LedDelegate());

    // layout for channel
    auto *layout_channel = new QHBoxLayout(tab_widget_->widget(TAB_CHANNEL));
    layout_channel->setContentsMargins(0,0,0,0);
    layout_channel->addWidget(channel_view_);

    // Monitor_Pvt_Wrapper.
    monitorPvtWrapper_ = new MonitorPvtWrapper();

    // Telecommand widget
    auto layout_settings = new QVBoxLayout(tab_widget_->widget(TAB_SETTINGS));
    telecommandWidget_ = new TelecommandWidget();
    layout_settings->addWidget(telecommandWidget_);
    connect(telecommandWidget_, &TelecommandWidget::resetClicked, this, &MainWindow::clearEntries);

    // solution tab page
    auto layout_solution = new QGridLayout(tab_widget_->widget(TAB_SOLUTION));
    layout_solution->setRowStretch(0,3);
    layout_solution->setRowStretch(1,2);
    layout_solution->setColumnStretch(0,1);
    layout_solution->setColumnStretch(1,1);
    layout_solution->setColumnStretch(2,1);
    layout_solution->setColumnStretch(3,1);

    // PVT message table
    auto solution_table = new CustomTableView(tab_widget_->widget(TAB_SOLUTION));
    pvt_table_model_ = new PVTTableModel(solution_table);
    solution_table->setModel(pvt_table_model_);
    layout_solution->addWidget(solution_table,0,0,1,1);

    // Map widget.
    map_widget_ = new QQuickWidget(tab_widget_->widget(TAB_SOLUTION));
    layout_solution->addWidget(map_widget_,0,1,1,3);
    map_widget_->rootContext()->setContextProperty("monitor_pvt_wrapper_", monitorPvtWrapper_);
    map_widget_->setSource(QUrl("qrc:/qml/main.qml"));
    map_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // Altitude widget.
    altitude_widget_ = new AltitudeWidget(tab_widget_->widget(TAB_SOLUTION));
    layout_solution->addWidget(altitude_widget_,1,0,1,2);
    connect(monitorPvtWrapper_, &MonitorPvtWrapper::altitudeChanged, altitude_widget_, &AltitudeWidget::addData);
    connect(&updateTimer_, &QTimer::timeout, altitude_widget_, &AltitudeWidget::redraw);

    // Dilution of precision widget.
    dop_widget_ = new DOPWidget(tab_widget_->widget(TAB_SOLUTION));
    layout_solution->addWidget(dop_widget_,1,2,1,2);
    connect(monitorPvtWrapper_, &MonitorPvtWrapper::dopChanged, dop_widget_, &DOPWidget::addData);
    connect(&updateTimer_, &QTimer::timeout, dop_widget_, &DOPWidget::redraw);

    // QMenuBar.
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionQuit->setShortcuts(QKeySequence::Quit);

    ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-desktop"));
    ui->actionPreferences->setShortcuts(QKeySequence::Preferences);

    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferences);

    // QToolbar.
    start_ = ui->mainToolBar->addAction("Start");
    stop_ = ui->mainToolBar->addAction("Stop");
    clear_ = ui->mainToolBar->addAction("Clear");
    ui->mainToolBar->addSeparator();
    closePlotsAction_ = ui->mainToolBar->addAction("Close Plots");
    ui->mainToolBar->addSeparator();

    start_->setEnabled(false);
    stop_->setEnabled(true);
    clear_->setEnabled(false);
    connect(start_, &QAction::triggered, this, &MainWindow::toggleCapture);
    connect(stop_, &QAction::triggered, this, &MainWindow::toggleCapture);
    connect(clear_, &QAction::triggered, this, &MainWindow::clearEntries);
    connect(closePlotsAction_, &QAction::triggered, this, &MainWindow::closePlots);

    // Sockets.
    socketGnssSynchro_ = std::make_unique<SocketGnss>(nullptr,1234);
    socketMonitorPvt_ = std::make_unique<SocketPVT>(nullptr, 1111);
    socketGnssSynchro_->start();
    socketMonitorPvt_->start();

    // Connect Signals & Slots.
    qRegisterMetaType<std::vector<ChannelStruct>>("std::vector<ChannelStruct>");
    connect(socketGnssSynchro_.get(), &SocketGnss::sendData, this, &MainWindow::receiveGnssSynchro);
    connect(qApp,&QApplication::aboutToQuit,socketGnssSynchro_.get(),&SocketGnss::stopThread);

    qRegisterMetaType<PVTStruct>("PVTStruct");
    connect(socketMonitorPvt_.get(), &SocketPVT::sendData, this, &MainWindow::receiveMonitorPvt);
    connect(qApp, &QApplication::aboutToQuit, socketMonitorPvt_.get(), &SocketPVT::stopThread);

    connect(channel_view_, &QTableView::clicked, this, &MainWindow::expandPlot);
    connect(channel_view_, &QTableView::doubleClicked, this, &MainWindow::expandPlot);

    // about project 和 about qt 弹出窗口
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // Load settings from last session.
    loadSettings();
}

MainWindow::~MainWindow() {
    quit();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    deletePlots();

    QMainWindow::closeEvent(event);
}

void MainWindow::toggleCapture()
{
    if (start_->isEnabled())
    {
        start_->setEnabled(false);
        stop_->setEnabled(true);
    }
    else
    {
        start_->setEnabled(true);
        stop_->setEnabled(false);
    }
}

void MainWindow::receiveGnssSynchro(const std::vector<ChannelStruct>& vector)
{
    if (stop_->isEnabled())
    {
        channelTableModel_->populateChannels(vector);
        clear_->setEnabled(true);
    }
    if (!updateTimer_.isActive())
    {
        updateTimer_.start();
    }
}

void MainWindow::receiveMonitorPvt(PVTStruct in)
{
    // 当有消息待处理
    if (stop_->isEnabled())
    {
        pvt_table_model_->populatePVT(in);
        monitorPvtWrapper_->addMonitorPvt(in);
        dynamic_cast<AlertWidget*>(tab_widget_->widget(TAB_ALERT))->positionShow(in);
    }
}

void MainWindow::clearEntries()
{
    channelTableModel_->clearChannels();
    channelTableModel_->update();

    altitude_widget_->clear();
    altitude_widget_->redraw();

    pvt_table_model_->clearData();
    pvt_table_model_->update();

    dop_widget_->clear();

    clear_->setEnabled(false);
}

void MainWindow::quit() { saveSettings(); }

void MainWindow::saveSettings()
{
    settings_.beginGroup("Main_Window");
    // 记录MainWindow的位置和大小
    settings_.setValue("pos", pos());
    settings_.setValue("size", size());
    settings_.endGroup();

    settings_.beginGroup("tableView");
    settings_.beginWriteArray("column");
    for (int i = 0; i < channelTableModel_->getColumns(); i++)
    {
        settings_.setArrayIndex(i);
        settings_.setValue("width", channel_view_->columnWidth(i));
    }
    settings_.endArray();
    settings_.endGroup();
}

void MainWindow::loadSettings()
{
    settings_.beginGroup("Main_Window");
    move(settings_.value("pos", QPoint(0, 0)).toPoint());
    resize(settings_.value("size", QSize(1400, 600)).toSize());
    settings_.endGroup();

    settings_.beginGroup("tableView");
    settings_.beginReadArray("column");
    for (int i = 0; i < channelTableModel_->getColumns(); i++)
    {
        settings_.setArrayIndex(i);
        channel_view_->setColumnWidth(i, settings_.value("width", 100).toInt());
    }
    settings_.endArray();
    settings_.endGroup();

    setPort();
}

void MainWindow::showPreferences()
{
    auto *preferences = new PreferencesDialog(this);
    connect(preferences, &PreferencesDialog::accepted, channelTableModel_,
        &ChannelTableModel::setBufferSize);
    connect(preferences, &PreferencesDialog::accepted, this,
        &MainWindow::setPort);
    preferences->exec();
}

void MainWindow::setPort()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    socketGnssSynchro_->setPort(settings.value("port_gnss_synchro", 1234).toInt());
    socketMonitorPvt_->setPort(settings.value("port_monitor_pvt", 1111).toInt());
    settings.endGroup();
}

void MainWindow::expandPlot(const QModelIndex &index)
{
    int channel_id = channelTableModel_->getChannelId(index.row());

    CustomChartView *chartView = nullptr;

    if (index.column() == CHANNEL_CONSTELLATION)
    {
        if (plotsConstellation_.find(index.row()) == plotsConstellation_.end())
        {
            chartView = new CustomChartView(nullptr, true);
            chartView->setTitle("Channel " + QString::number(channel_id)+" Constellation");
            chartView->setAxisTitle("I prompt","Q prompt");
            chartView->updateChart(index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { plotsConstellation_.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&updateTimer_, &QTimer::timeout, this, [index,chartView]() {
                chartView->updateChart(index);
            });

            plotsConstellation_[index.row()] = chartView;
        }
        else
        {
            chartView = plotsConstellation_.at(index.row());
        }
    }
    else if (index.column() == CHANNEL_CN0)
    {
        if (plotsCn0_.find(index.row()) == plotsCn0_.end())
        {
            // 必须是空指针，否则不会出现独立窗口
            chartView = new CustomChartView(nullptr, false);
            chartView->setTitle("CN0 CH " + QString::number(channel_id));
            chartView->setAxisTitle("TOW [s]","C/N0 [db-Hz]");
            chartView->updateCN0Chart(index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { plotsCn0_.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&updateTimer_, &QTimer::timeout, this, [index,chartView]() {
                chartView->updateCN0Chart(index);
            });

            plotsCn0_[index.row()] = chartView;
        }
        else
        {
            chartView = plotsCn0_.at(index.row());
        }
    }
    else if (index.column() == CHANNEL_DOPPLER)
    {

        if (plotsDoppler_.find(index.row()) == plotsDoppler_.end())
        {
            chartView = new CustomChartView(nullptr, false);
            chartView->setTitle("Doppler CH " + QString::number(channel_id));
            chartView->setAxisTitle("TOW [s]","Doppler [Hz]");

            // Draw chart now.
            chartView->updateChart(index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { plotsDoppler_.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&updateTimer_, &QTimer::timeout, this, [ index,chartView]() {
                chartView->updateChart(index);
            });

            plotsDoppler_[index.row()] = chartView;
        }
        else
        {
            chartView = plotsDoppler_.at(index.row());
        }
    }

    if (!chartView)  // Equivalent to: if (chartView == nullptr)
    {
        return;
    }

    chartView->resize(screenWidth_/2, screenHeight_/2);
    chartView->show();
//    chartView->raise();
}

void MainWindow::closePlots()
{
    for (auto const &ch : plotsConstellation_)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }

    for (auto const &ch : plotsCn0_)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }

    for (auto const &ch : plotsDoppler_)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }
}

void MainWindow::deletePlots()
{
    for (auto const &ch : plotsConstellation_)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    plotsConstellation_.clear();

    for (auto const &ch : plotsCn0_)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    plotsCn0_.clear();

    for (auto const &ch : plotsDoppler_)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    plotsDoppler_.clear();
}

void MainWindow::about()
{
    const QString text =
        "<h3>gnss-sdr-monitor</h3>"
        "基于TCP-IP的GNSS SDR用户交互界面"
        "<p>由东南大学Xinhua Tang团队，修改自 Álvaro Cebrián Juan.</p>"
        "<p>bug和功能建议可以联系我们： kxn1119@gmail.com</p>";

    QMessageBox::about(this, "About gnss-sdr-monitor", text);
}
