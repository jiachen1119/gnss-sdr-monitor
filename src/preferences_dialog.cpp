/*!
 * \file preferences_dialog.cpp
 * \brief Implementation of the preferences dialog of the gui.
 */


#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"
#include <QDebug>
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent),
                                                        ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    ui->buffer_size_spinBox->setValue(settings.value("buffer_size", 1000).toInt());
    ui->port_gnss_synchro_spinBox->setValue(settings.value("port_gnss_synchro", 1111).toInt());
    ui->port_monitor_pvt_spinBox->setValue(settings.value("port_monitor_pvt", 1112).toInt());
    settings.endGroup();

    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::onAccept);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::onAccept()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    settings.setValue("buffer_size", ui->buffer_size_spinBox->value());
    settings.setValue("port_gnss_synchro", ui->port_gnss_synchro_spinBox->value());
    settings.setValue("port_monitor_pvt", ui->port_monitor_pvt_spinBox->value());
    settings.endGroup();

    qDebug() << "Preferences Saved";
}
