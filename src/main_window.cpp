/*!
 * \file main_window.cpp
 * \brief Implementation of the main window of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 * Kepeng Luan changed, Southeast University, 2023-2024
 */


#include "main_window.h"
#include "cn0_delegate.h"
#include "constellation_delegate.h"
#include "doppler_delegate.h"
#include "led_delegate.h"
#include "preferences_dialog.h"
#include "ui_main_window.h"
#include <QDebug>
#include <QQmlContext>
#include <QtCharts>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkDatagram>
#include <iostream>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // 获取屏幕尺寸，为后续expandPlot做准备
    QScreen *screen=QGuiApplication::primaryScreen();
    QRect size=screen->availableGeometry();
    screenHeight_=size.height();
    screenWidth_=size.width();

    // 表格更新计时器
    updateTimer_.setInterval(1000);
    updateTimer_.setSingleShot(true);
    connect(&updateTimer_, &QTimer::timeout, [this] { model_->update(); });

    // UI设置
    ui->setupUi(this);

    // tab widget 设置
    ui->tabWidget->setTabPosition(QTabWidget::South);
    ui->tabWidget->setDocumentMode(true);

    // Monitor_Pvt_Wrapper.
    monitorPvtWrapper_ = new MonitorPvtWrapper();

    // Telecommand widget
    telecommandWidget_ = new TelecommandWidget(ui->telecomWidget);
    ui->telecomWidget->setMaximumWidth(this->width()/2);
    connect(telecommandWidget_, &TelecommandWidget::resetClicked, this, &MainWindow::clearEntries);

    // Map widget.
    mapWidget_ = new QQuickWidget(ui->tabWidget);
    ui->tabWidget->addTab(mapWidget_, QStringLiteral("Map"));
    mapWidget_->rootContext()->setContextProperty("monitor_pvt_wrapper_", monitorPvtWrapper_);
    mapWidget_->setSource(QUrl("qrc:/qml/main.qml"));
    mapWidget_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // Altitude widget.
    altitudeWidget_ = new AltitudeWidget(ui->tabWidget);
    ui->tabWidget->addTab(altitudeWidget_, QStringLiteral("Altitude"));
    connect(monitorPvtWrapper_, &MonitorPvtWrapper::altitudeChanged, altitudeWidget_, &AltitudeWidget::addData);
    connect(&updateTimer_, &QTimer::timeout, altitudeWidget_, &AltitudeWidget::redraw);

    // Dilution of precision widget.
    DOPWidget_ = new DOPWidget(ui->tabWidget);
    ui->tabWidget->addTab(DOPWidget_, QStringLiteral("DOP"));
    connect(monitorPvtWrapper_, &MonitorPvtWrapper::dopChanged, DOPWidget_, &DOPWidget::addData);
    connect(&updateTimer_, &QTimer::timeout, DOPWidget_, &DOPWidget::redraw);

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

    // Model.
    model_ = new ChannelTableModel();

    // QTableView.
    // Tie the model to the view.
    ui->tableView->setModel(model_);
    ui->tableView->setShowGrid(false);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegateForColumn(5, new ConstellationDelegate());
    ui->tableView->setItemDelegateForColumn(6, new Cn0Delegate());
    ui->tableView->setItemDelegateForColumn(7, new DopplerDelegate());
    ui->tableView->setItemDelegateForColumn(9, new LedDelegate());

    ui->tableView->resizeColumnsToContents();
    // ui->tableView->setAlternatingRowColors(true);
    // ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    // Sockets.
    socketGnssSynchro_ = std::make_unique<SocketGnss>(nullptr,1111);
    socketMonitorPvt_ = new QUdpSocket(this);
    socketGnssSynchro_->start();

    // Connect Signals & Slots.
    qRegisterMetaType<gnss_sdr::Observables>("gnss_sdr::Observables");
    connect(socketGnssSynchro_.get(), &SocketGnss::sendData, this, &MainWindow::receiveGnssSynchro);
    connect(qApp,&QApplication::aboutToQuit,socketGnssSynchro_.get(),&SocketGnss::stopThread);

    connect(socketMonitorPvt_, &QUdpSocket::readyRead, this, &MainWindow::receiveMonitorPvt);
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::quit);
    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::expandPlot);
    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::expandPlot);

    // about project 和 about qt 弹出窗口
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // Load settings from last session.
    loadSettings();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event)
{
    deletePlots();

    QMainWindow::closeEvent(event);
}

void MainWindow::updateChart(QtCharts::QChart *chart, QtCharts::QXYSeries *series, const QModelIndex &index)
{
    QPointF p;
    QVector<QPointF> points;

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    double min_y = std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

    QList<QVariant> var = index.data(Qt::DisplayRole).toList();
    for (const auto & i : var)
    {
        p = i.toPointF();
        points << p;

        min_x = std::min(min_x, p.x());
        max_x = std::max(max_x, p.x());
        min_y = std::min(min_y, p.y());
        max_y = std::max(max_y, p.y());
    }
    series->replace(points);

    chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
    chart->axes(Qt::Vertical).back()->setRange(min_y, max_y);
}

void MainWindow::updateCnoChart(QtCharts::QChart *chart, QtCharts::QXYSeries *series, const QModelIndex &index)
{
    QPointF p;
    QVector<QPointF> points;

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    double min_y = std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

    QList<QVariant> var = index.data(Qt::DisplayRole).toList();
    for (const auto & i : var)
    {
        p = i.toPointF();
        points << p;

        min_x = std::min(min_x, p.x());
        min_y = std::min(min_y, p.y());

        max_x = std::max(max_x, p.x());
        max_y = std::max(max_y, p.y());
    }
    chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
    chart->axes(Qt::Vertical).back()->setRange(25, 50);
    series->replace(points);
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

void MainWindow::receiveGnssSynchro(gnss_sdr::Observables stocks)
{
    stocks_ =std::move(stocks);
    if (stop_->isEnabled())
    {
        model_->populateChannels(&stocks_);
        clear_->setEnabled(true);
    }
    if (!updateTimer_.isActive())
    {
        updateTimer_.start();
    }
}

void MainWindow::receiveMonitorPvt()
{
    while (socketMonitorPvt_->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socketMonitorPvt_->receiveDatagram();
        m_monitorPvt =
            readMonitorPvt(datagram.data().data(), datagram.data().size());

        if (stop_->isEnabled())
        {
            monitorPvtWrapper_->addMonitorPvt(m_monitorPvt);
            // clear->setEnabled(true);
        }
    }
}

void MainWindow::clearEntries()
{
    model_->clearChannels();
    model_->update();

    altitudeWidget_->clear();
    DOPWidget_->clear();

    clear_->setEnabled(false);
}

void MainWindow::quit() { saveSettings(); }

gnss_sdr::MonitorPvt MainWindow::readMonitorPvt(char buff[], int bytes)
{
    try
    {
        std::string data(buff, bytes);
        m_monitorPvt.ParseFromString(data);
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }

    return m_monitorPvt;
}

void MainWindow::saveSettings()
{
    settings_.beginGroup("Main_Window");
    // 记录MainWindow的位置和大小
    settings_.setValue("pos", pos());
    settings_.setValue("size", size());
    settings_.endGroup();

    settings_.beginGroup("tableView");
    settings_.beginWriteArray("column");
    for (int i = 0; i < model_->getColumns(); i++)
    {
        settings_.setArrayIndex(i);
        settings_.setValue("width", ui->tableView->columnWidth(i));
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
    for (int i = 0; i < model_->getColumns(); i++)
    {
        settings_.setArrayIndex(i);
        ui->tableView->setColumnWidth(i, settings_.value("width", 100).toInt());
    }
    settings_.endArray();
    settings_.endGroup();

    setPort();
}

void MainWindow::showPreferences()
{
    PreferencesDialog *preferences = new PreferencesDialog(this);
    connect(preferences, &PreferencesDialog::accepted, model_,
        &ChannelTableModel::setBufferSize);
    connect(preferences, &PreferencesDialog::accepted, this,
        &MainWindow::setPort);
    preferences->exec();
}

void MainWindow::setPort()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    m_portGnssSynchro = settings.value("port_gnss_synchro", 1111).toInt();
    m_portMonitorPvt = settings.value("port_monitor_pvt", 1112).toInt();
    settings.endGroup();

    socketGnssSynchro_->setPort(m_portGnssSynchro);
    socketMonitorPvt_->bind(QHostAddress::Any, m_portMonitorPvt);
}

void MainWindow::expandPlot(const QModelIndex &index)
{
    int channel_id = model_->getChannelId(index.row());

    QChartView *chartView = nullptr;

    if (index.column() == 5)  // Constellation
    {
        if (plotsConstellation_.find(index.row()) == plotsConstellation_.end())
        {
            auto *chart = new QChart();  // has no parent!
            chart->setTitle("Channel " + QString::number(channel_id));
            chart->legend()->hide();

            // 散点图
            auto *series = new QScatterSeries(chart);
            // 设置点的大小
            series->setMarkerSize(8);

            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axes(Qt::Horizontal).back()->setTitleText("I prompt");
            chart->axes(Qt::Vertical).back()->setTitleText("Q prompt");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            // 抗锯齿
            chartView->setRenderHint(QPainter::Antialiasing, true);
            chartView->setContentsMargins(0, 0, 0, 0);

            // Draw chart now.
            updateChart(chart, series, index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { plotsConstellation_.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&updateTimer_, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
            });

            plotsConstellation_[index.row()] = chartView;
        }
        else
        {
            chartView = plotsConstellation_.at(index.row());
        }
    }
    else if (index.column() == 6)  // CN0
    {
        if (plotsCn0_.find(index.row()) == plotsCn0_.end())
        {
            QChart *chart = new QChart();  // has no parent!
            chart->setTitle("CN0 CH " + QString::number(channel_id));
            chart->legend()->hide();

            QLineSeries *series = new QLineSeries(chart);
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
            chart->axes(Qt::Vertical).back()->setTitleText("C/N0 [db-Hz]");
            chart->axes(Qt::Vertical).back()->setRange(10,55);
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            // Draw chart now.
            updateCnoChart(chart, series, index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { plotsCn0_.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&updateTimer_, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateCnoChart(chart, series, index);
            });

            plotsCn0_[index.row()] = chartView;
        }
        else
        {
            chartView = plotsCn0_.at(index.row());
        }
    }
    else if (index.column() == 7)  // Doppler
    {
        if (plotsDoppler_.find(index.row()) == plotsDoppler_.end())
        {
            QChart *chart = new QChart();  // has no parent!
            chart->setTitle("Doppler CH " + QString::number(channel_id));
            chart->legend()->hide();

            QLineSeries *series = new QLineSeries(chart);
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
            chart->axes(Qt::Vertical).back()->setTitleText("Doppler [Hz]");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            // Draw chart now.
            updateChart(chart, series, index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { plotsDoppler_.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&updateTimer_, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
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
    chartView->raise();
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
