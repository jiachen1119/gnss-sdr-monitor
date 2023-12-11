/*!
 * \file main_window.cpp
 * \brief Implementation of the main window of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 * Kepeng Luan changed, Southeast University, 2023
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
    // Use a timer to delay updating the model to a fixed amount of times per
    // second.
    m_updateTimer.setInterval(500);
    m_updateTimer.setSingleShot(true);
    connect(&m_updateTimer, &QTimer::timeout, [this] { model_->update(); });

    ui->setupUi(this);

    // Monitor_Pvt_Wrapper.
    monitorPvtWrapper_ = new MonitorPvtWrapper();

    // Telecommand widget.
    telecommandDockWidget_ = new QDockWidget("Telecommand", this);
    telecommandWidget_ = new TelecommandWidget(telecommandDockWidget_);
    telecommandDockWidget_->setWidget(telecommandWidget_);
    addDockWidget(Qt::TopDockWidgetArea, telecommandDockWidget_);
    connect(telecommandWidget_, &TelecommandWidget::resetClicked, this, &MainWindow::clearEntries);

    // Map widget.
    mapDockWidget_ = new QDockWidget("Map", this);
    mapWidget_ = new QQuickWidget(this);
    mapWidget_->rootContext()->setContextProperty("m_monitor_pvt_wrapper", monitorPvtWrapper_);
    mapWidget_->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    mapWidget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    mapDockWidget_->setWidget(mapWidget_);
    addDockWidget(Qt::TopDockWidgetArea, mapDockWidget_);

    // Altitude widget.
    altitudeDockWidget_ = new QDockWidget("Altitude", this);
    altitudeWidget_ = new AltitudeWidget(altitudeDockWidget_);
    altitudeDockWidget_->setWidget(altitudeWidget_);
    addDockWidget(Qt::TopDockWidgetArea, altitudeDockWidget_);
    connect(monitorPvtWrapper_, &MonitorPvtWrapper::altitudeChanged, altitudeWidget_, &AltitudeWidget::addData);
    connect(&m_updateTimer, &QTimer::timeout, altitudeWidget_, &AltitudeWidget::redraw);

    // Dilution of precision widget.
    m_DOPDockWidget = new QDockWidget("DOP", this);
    DOPWidget_ = new DOPWidget(m_DOPDockWidget);
    m_DOPDockWidget->setWidget(DOPWidget_);
    addDockWidget(Qt::TopDockWidgetArea, m_DOPDockWidget);
    connect(monitorPvtWrapper_, &MonitorPvtWrapper::dopChanged, DOPWidget_, &DOPWidget::addData);
    connect(&m_updateTimer, &QTimer::timeout, DOPWidget_, &DOPWidget::redraw);

    // QMenuBar.
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionQuit->setShortcuts(QKeySequence::Quit);

    ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-desktop"));
    ui->actionPreferences->setShortcuts(QKeySequence::Preferences);

    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferences);

    // QToolbar.
    m_start = ui->mainToolBar->addAction("Start");
    m_stop = ui->mainToolBar->addAction("Stop");
    clear_ = ui->mainToolBar->addAction("Clear");
    ui->mainToolBar->addSeparator();
    m_closePlotsAction = ui->mainToolBar->addAction("Close Plots");
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(telecommandDockWidget_->toggleViewAction());
    ui->mainToolBar->addAction(mapDockWidget_->toggleViewAction());
    ui->mainToolBar->addAction(altitudeDockWidget_->toggleViewAction());
    ui->mainToolBar->addAction(m_DOPDockWidget->toggleViewAction());
    m_start->setEnabled(false);
    m_stop->setEnabled(true);
    clear_->setEnabled(false);
    connect(m_start, &QAction::triggered, this, &MainWindow::toggleCapture);
    connect(m_stop, &QAction::triggered, this, &MainWindow::toggleCapture);
    connect(clear_, &QAction::triggered, this, &MainWindow::clearEntries);
    connect(m_closePlotsAction, &QAction::triggered, this, &MainWindow::closePlots);

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
    // ui->tableView->setAlternatingRowColors(true);
    // ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    // Sockets.
    m_socketGnssSynchro = new QUdpSocket(this);
    m_socketMonitorPvt = new QUdpSocket(this);

    // Connect Signals & Slots.
    connect(m_socketGnssSynchro, &QUdpSocket::readyRead, this, &MainWindow::receiveGnssSynchro);
    connect(m_socketMonitorPvt, &QUdpSocket::readyRead, this, &MainWindow::receiveMonitorPvt);
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::quit);
    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::expandPlot);
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
    for (int i = 0; i < var.size(); i++)
    {
        p = var.at(i).toPointF();
        points << p;

        min_x = std::min(min_x, p.x());
        min_y = std::min(min_y, p.y());

        max_x = std::max(max_x, p.x());
        max_y = std::max(max_y, p.y());
    }

    series->replace(points);

    chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
    chart->axes(Qt::Vertical).back()->setRange(min_y, max_y);
}

void MainWindow::toggleCapture()
{
    if (m_start->isEnabled())
    {
        m_start->setEnabled(false);
        m_stop->setEnabled(true);
    }
    else
    {
        m_start->setEnabled(true);
        m_stop->setEnabled(false);
    }
}

void MainWindow::receiveGnssSynchro()
{
    bool newData = false;
    while (m_socketGnssSynchro->hasPendingDatagrams())
    {
        newData = true;
        QNetworkDatagram datagram = m_socketGnssSynchro->receiveDatagram();
        m_stocks = readGnssSynchro(datagram.data().data(), datagram.data().size());

        if (m_stop->isEnabled())
        {
            model_->populateChannels(&m_stocks);
            clear_->setEnabled(true);
        }
    }
    if (newData && !m_updateTimer.isActive())
    {
        m_updateTimer.start();
    }
}

void MainWindow::receiveMonitorPvt()
{
    while (m_socketMonitorPvt->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_socketMonitorPvt->receiveDatagram();
        m_monitorPvt =
            readMonitorPvt(datagram.data().data(), datagram.data().size());

        if (m_stop->isEnabled())
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

gnss_sdr::Observables MainWindow::readGnssSynchro(char buff[], int bytes)
{
    try
    {
        std::string data(buff, bytes);
        m_stocks.ParseFromString(data);
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }

    return m_stocks;
}

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
    m_settings.beginGroup("Main_Window");
    m_settings.setValue("pos", pos());
    m_settings.setValue("size", size());
    m_settings.endGroup();

    m_settings.beginGroup("tableView");
    m_settings.beginWriteArray("column");
    for (int i = 0; i < model_->getColumns(); i++)
    {
        m_settings.setArrayIndex(i);
        m_settings.setValue("width", ui->tableView->columnWidth(i));
    }
    m_settings.endArray();
    m_settings.endGroup();

    qDebug() << "Settings Saved";
}

void MainWindow::loadSettings()
{
    m_settings.beginGroup("Main_Window");
    move(m_settings.value("pos", QPoint(0, 0)).toPoint());
    resize(m_settings.value("size", QSize(1400, 600)).toSize());
    m_settings.endGroup();

    m_settings.beginGroup("tableView");
    m_settings.beginReadArray("column");
    for (int i = 0; i < model_->getColumns(); i++)
    {
        m_settings.setArrayIndex(i);
        ui->tableView->setColumnWidth(i, m_settings.value("width", 100).toInt());
    }
    m_settings.endArray();
    m_settings.endGroup();

    setPort();

    qDebug() << "Settings Loaded";
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

    m_socketGnssSynchro->disconnectFromHost();
    m_socketGnssSynchro->bind(QHostAddress::Any, m_portGnssSynchro);
    m_socketMonitorPvt->bind(QHostAddress::Any, m_portMonitorPvt);
}

void MainWindow::expandPlot(const QModelIndex &index)
{
    qDebug() << index;

    int channel_id = model_->getChannelId(index.row());

    QChartView *chartView = nullptr;

    if (index.column() == 5)  // Constellation
    {
        if (m_plotsConstellation.find(index.row()) == m_plotsConstellation.end())
        {
            QChart *chart = new QChart();  // has no parent!
            chart->setTitle("Constellation CH " + QString::number(channel_id));
            chart->legend()->hide();

            QScatterSeries *series = new QScatterSeries(chart);
            series->setMarkerSize(8);
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axes(Qt::Horizontal).back()->setTitleText("I prompt");
            chart->axes(Qt::Vertical).back()->setTitleText("Q prompt");
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
                [this, index]() { m_plotsConstellation.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
            });

            m_plotsConstellation[index.row()] = chartView;
        }
        else
        {
            chartView = m_plotsConstellation.at(index.row());
        }
    }
    else if (index.column() == 6)  // CN0
    {
        if (m_plotsCn0.find(index.row()) == m_plotsCn0.end())
        {
            QChart *chart = new QChart();  // has no parent!
            chart->setTitle("CN0 CH " + QString::number(channel_id));
            chart->legend()->hide();

            QLineSeries *series = new QLineSeries(chart);
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
            chart->axes(Qt::Vertical).back()->setTitleText("C/N0 [db-Hz]");
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
                [this, index]() { m_plotsCn0.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
            });

            m_plotsCn0[index.row()] = chartView;
        }
        else
        {
            chartView = m_plotsCn0.at(index.row());
        }
    }
    else if (index.column() == 7)  // Doppler
    {
        if (m_plotsDoppler.find(index.row()) == m_plotsDoppler.end())
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
                [this, index]() { m_plotsDoppler.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
            });

            m_plotsDoppler[index.row()] = chartView;
        }
        else
        {
            chartView = m_plotsDoppler.at(index.row());
        }
    }

    if (!chartView)  // Equivalent to: if (chartView == nullptr)
    {
        return;
    }

    chartView->resize(400, 180);
    chartView->show();
}

void MainWindow::closePlots()
{
    for (auto const &ch : m_plotsConstellation)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }

    for (auto const &ch : m_plotsCn0)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }

    for (auto const &ch : m_plotsDoppler)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }
}

void MainWindow::deletePlots()
{
    for (auto const &ch : m_plotsConstellation)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    m_plotsConstellation.clear();

    for (auto const &ch : m_plotsCn0)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    m_plotsCn0.clear();

    for (auto const &ch : m_plotsDoppler)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    m_plotsDoppler.clear();
}

void MainWindow::about()
{
    const QString text =
        "<h3>gnss-sdr-monitor</h3>"
        "A graphical user interface to monitor the GNSS-SDR status in real time."
        "<p>Written by Álvaro Cebrián Juan and licensed under GNU GPLv3 license.</p>"
        "<p>Report bugs and suggestions to acebrianjuan@gmail.com</p>";

    QMessageBox::about(this, "About gnss-sdr-monitor", text);
}
