/*!
 * \file telecommand_widget.h
 * \brief Interface of a widget for sending commands to the receiver
 * through telnet.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
 *
 */


#ifndef GNSS_SDR_MONITOR_TELECOMMAND_WIDGET_H_
#define GNSS_SDR_MONITOR_TELECOMMAND_WIDGET_H_

#include "telnet_manager.h"
#include <QHostAddress>
#include <QSettings>
#include <QTcpSocket>
#include <QTimer>
#include <QWidget>

namespace Ui
{
class TelecommandWidget;
}

class TelecommandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TelecommandWidget(QWidget *parent = nullptr);
    ~TelecommandWidget();

signals:
    void inputsChanged();
    void receivedData(QByteArray data);

    // Forwarded from TelnetManager.
    void connected();
    void disconnected();
    void resetClicked();

public slots:
    void onAddressEditTextchanged();
    void onPortEditTextchanged();
    void handleInputsChanged();
    void clear();

    void onResetClicked();
    void onStandbyClicked();
    void onColdstartClicked();
    void onWarmstartClicked();
    void onHotstartClicked();
    void onStatusClicked();
    void onConnectClicked();
    void reconnect();

    void printText(const QByteArray& data);

    void setAddress(const QString& ip);
    void setPort(const QString& port);

    void statusConnected();
    void statusDisconnected();

    void saveSettings();
    void loadSettings();

    QString getArgs();

    void printError(QAbstractSocket::SocketError socketError);

private:
    Ui::TelecommandWidget *ui;

    TelnetManager telnetManager_;
    QSettings settings_;
    QTimer timer;
};

#endif  // GNSS_SDR_MONITOR_TELECOMMAND_WIDGET_H_
