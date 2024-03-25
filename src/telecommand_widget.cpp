/*!
 * \file telecommand_widget.cpp
 * \brief Implementation of a widget for sending commands to the receiver
 * through telnet.
 * -----------------------------------------------------------------------
 */


#include "telecommand_widget.h"
#include "ui_telecommand_widget.h"
#include <QScrollBar>

TelecommandWidget::TelecommandWidget(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::TelecommandWidget)
{
    ui->setupUi(this);

    //check mark
    ui->autoScrollCheckBox->setChecked(true);
    ui->reconnectCheckBox->setChecked(true);
    ui->clearCheckBox->setChecked(true);

    ui->connectPushButton->setEnabled(false);
    ui->plainTextEdit->setPlaceholderText("The sent and received messages will be logged here...");
    ui->dateTimeEdit->setDate(QDate::currentDate());

    // Disable the command buttons.
    ui->statusPushButton->setEnabled(false);
    ui->standbyPushButton->setEnabled(false);
    ui->resetPushButton->setEnabled(false);

    ui->coldStartPushButton->setEnabled(false);
    ui->warmStartPushButton->setEnabled(false);
    ui->hotStartPushButton->setEnabled(false);

    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, &TelecommandWidget::reconnect);

    // if the text is changed, goto handleInputsChanged
    connect(ui->addressLineEdit, &QLineEdit::textChanged, this, &TelecommandWidget::onAddressEditTextchanged);
    connect(ui->portLineEdit, &QLineEdit::textChanged, this, &TelecommandWidget::onPortEditTextchanged);
    connect(this, &TelecommandWidget::inputsChanged, this, &TelecommandWidget::handleInputsChanged);

    connect(ui->clearPushButton, &QPushButton::clicked, this, &TelecommandWidget::clear);

    // connect the pushbutton to the send message
    connect(ui->resetPushButton, &QPushButton::clicked, this, &TelecommandWidget::onResetClicked);
    connect(ui->standbyPushButton, &QPushButton::clicked, this, &TelecommandWidget::onStandbyClicked);
    connect(ui->coldStartPushButton, &QPushButton::clicked, this, &TelecommandWidget::onColdstartClicked);
    connect(ui->warmStartPushButton, &QPushButton::clicked, this, &TelecommandWidget::onWarmstartClicked);
    connect(ui->hotStartPushButton, &QPushButton::clicked, this, &TelecommandWidget::onHotstartClicked);
    connect(ui->statusPushButton, &QPushButton::clicked, this, &TelecommandWidget::onStatusClicked);
    connect(ui->connectPushButton, &QPushButton::clicked, this, &TelecommandWidget::onConnectClicked);

    connect(&telnetManager_, &TelnetManager::txData, this, &TelecommandWidget::printText);
    connect(&telnetManager_, &TelnetManager::rxData, this, &TelecommandWidget::printText);

    // connect button and save configure
    connect(&telnetManager_, &TelnetManager::connected, this, &TelecommandWidget::statusConnected);
    connect(&telnetManager_, &TelnetManager::disconnected, this, &TelecommandWidget::statusDisconnected);

    // Forward signals from TelnetManager.
    connect(&telnetManager_, &TelnetManager::connected, this, &TelecommandWidget::connected);
    connect(&telnetManager_, &TelnetManager::disconnected, this, &TelecommandWidget::disconnected);

    connect(&telnetManager_, &TelnetManager::error, this, &TelecommandWidget::printError);

    // Load settings from last session.
    loadSettings();

    handleInputsChanged();
}

TelecommandWidget::~TelecommandWidget()
{
    telnetManager_.disconnectTcp();

    QSettings settings;
    settings.beginGroup("TelecommandWidget");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    delete ui;
}

void TelecommandWidget::setAddress(const QString& addr_str)
{
    telnetManager_.setAddress(addr_str);
    ui->addressLineEdit->setText(addr_str);
}

/*!
 *
 * @param port_str
 * @brief set the port of telecommand
 */
void TelecommandWidget::setPort(const QString& port_str)
{
    telnetManager_.setPort(port_str);
    ui->portLineEdit->setText(port_str);
}

// decide the connect button state
void TelecommandWidget::handleInputsChanged()
{
    if (ui->addressLineEdit->text().isEmpty() || ui->portLineEdit->text().isEmpty())
    {
        ui->connectPushButton->setEnabled(false);
        ui->connectPushButton->setStyleSheet(styleSheet());
    }
    else
    {
        ui->connectPushButton->setEnabled(true);
        ui->connectPushButton->setStyleSheet("background-color:#2ECC40;");
    }
}

void TelecommandWidget::clear()
{
    ui->plainTextEdit->clear();
}

void TelecommandWidget::onAddressEditTextchanged()
{
    emit inputsChanged();
}

void TelecommandWidget::onPortEditTextchanged()
{
    emit inputsChanged();
}

void TelecommandWidget::onResetClicked()
{
    telnetManager_.sendCommand(TelnetManager::Command::Reset);

    if (ui->reconnectCheckBox->isChecked())
    {
        timer.start();
    }

    if (ui->clearCheckBox->isChecked())
    {
        emit resetClicked();
    }
}

void TelecommandWidget::onStandbyClicked()
{
    telnetManager_.sendCommand(TelnetManager::Command::Standby);
}

void TelecommandWidget::onColdstartClicked()
{
    telnetManager_.sendCommand(TelnetManager::Command::ColdStart);
}

void TelecommandWidget::onWarmstartClicked()
{
    telnetManager_.sendCommand(TelnetManager::Command::WarmStart, getArgs());
}

void TelecommandWidget::onHotstartClicked()
{
    telnetManager_.sendCommand(TelnetManager::Command::HotStart, getArgs());
}

void TelecommandWidget::onStatusClicked()
{
    telnetManager_.sendCommand(TelnetManager::Command::Status);
}

void TelecommandWidget::onConnectClicked()
{
    // What's the current state of the socket?
    QAbstractSocket::SocketState state = telnetManager_.getState();

    if (state == QAbstractSocket::ConnectedState)
    {
        // The socket is connected, so let's disconnect now.
        telnetManager_.sendCommand(TelnetManager::Command::Exit);
    }
    else if (state == QAbstractSocket::UnconnectedState)
    {
        // The socket is disconnected, so let's attempt a connection now.
        setAddress(ui->addressLineEdit->text());
        setPort(ui->portLineEdit->text());
        telnetManager_.connectTcp();
    }
}

void TelecommandWidget::reconnect()
{
    // What's the current state of the socket?
    QAbstractSocket::SocketState state = telnetManager_.getState();
    // if the connection is not established
    if (state != QAbstractSocket::ConnectedState && state != QAbstractSocket::ConnectingState)
    {
        // Attempt a connection
        telnetManager_.connectTcp();
    }

    if (state == QAbstractSocket::ConnectedState)
    {
        timer.stop();
    }
}

void TelecommandWidget::statusConnected()
{
    // The connection was successful so let's save the address and port number.
    QSettings settings;
    settings.beginGroup("tcp_socket");
    settings.setValue("address", telnetManager_.getAddress().toString());
    settings.setValue("port", telnetManager_.getPort());
    settings.endGroup();

    qDebug() << "Settings Saved";

    // Disable the ip address and port number edit fields.
    ui->addressLineEdit->setEnabled(false);
    ui->portLineEdit->setEnabled(false);

    // Enable the command buttons.
    ui->statusPushButton->setEnabled(true);
    ui->standbyPushButton->setEnabled(true);
    ui->resetPushButton->setEnabled(true);
    ui->coldStartPushButton->setEnabled(true);
    ui->warmStartPushButton->setEnabled(true);
    ui->hotStartPushButton->setEnabled(true);

    ui->connectPushButton->setText("Exit");
    ui->connectPushButton->setStyleSheet("background-color:#FF4136;");
}

void TelecommandWidget::statusDisconnected()
{
    // Enable the ip address and port number edit fields.
    ui->addressLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);

    // Disable the command buttons.
    ui->statusPushButton->setEnabled(false);
    ui->standbyPushButton->setEnabled(false);
    ui->resetPushButton->setEnabled(false);
    ui->coldStartPushButton->setEnabled(false);
    ui->warmStartPushButton->setEnabled(false);
    ui->hotStartPushButton->setEnabled(false);

    ui->connectPushButton->setText("Connect");
    ui->connectPushButton->setStyleSheet("background-color:#2ECC40;");
}

void TelecommandWidget::printText(const QByteArray& data)
{
    // Save current position of the vertical scroll bar.
    int scroll = ui->plainTextEdit->verticalScrollBar()->value();

    // Move cursor to the end.
    ui->plainTextEdit->moveCursor(QTextCursor::End);

    // Write text.
    QString text(data);
    ui->plainTextEdit->insertPlainText(text);

    // When finished writing restore scroll bar position or scroll to the end.
    if (ui->autoScrollCheckBox->isChecked())
    {
        // Scroll to the end.
        ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
    }
    else
    {
        // Restore saved scroll bar position.
        ui->plainTextEdit->verticalScrollBar()->setValue(scroll);
    }
}

void TelecommandWidget::loadSettings()
{
    settings_.beginGroup("TelecommandWidget");
    restoreGeometry(settings_.value("geometry").toByteArray());
    settings_.endGroup();

    settings_.beginGroup("tcp_socket");
    setAddress(settings_.value("address", "").toString());
    setPort(settings_.value("port", "").toString());
    settings_.endGroup();
}

QString TelecommandWidget::getArgs()
{
    QString args = QString("%1 %2 %3 %4").arg(ui->dateTimeEdit->text(), ui->latitudeDoubleSpinBox->text(), ui->longitudeDoubleSpinBox->text(), ui->heightDoubleSpinBox->text());

    return args;
}

void TelecommandWidget::printError(QAbstractSocket::SocketError socketError)
{
    qDebug() << socketError;
}

void TelecommandWidget::saveSettings()
{
    settings_.beginGroup("TelecommandWidget");
    settings_.setValue("geometry", saveGeometry());
    settings_.endGroup();

    settings_.beginGroup("tcp_socket");
    settings_.setValue("address", telnetManager_.getAddress().toString());
    settings_.setValue("port", telnetManager_.getPort());
    settings_.endGroup();

    qDebug() << "Settings Saved";
}
