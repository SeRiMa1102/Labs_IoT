#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "mainwindow.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;

QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    friend class MainWindow;
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
        bool localEchoEnabled;
    };

    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Settings settings() const;

private slots:
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::SettingsDialog *m_ui = nullptr;
    Settings m_currentSettings;
    QIntValidator *m_intValidator = nullptr;
};

#endif // SETTINGSDIALOG_H
