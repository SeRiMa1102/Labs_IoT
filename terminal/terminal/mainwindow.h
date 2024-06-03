#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QMainWindow>
#include <QSerialPort>

#include "parser.h"

QT_BEGIN_NAMESPACE

class QStandardItemModel;
class QLabel;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showError();
private slots:
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void putData(QByteArray data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);


    void on_treeView_doubleClicked(const QModelIndex &index);

signals:
    void new_data(const QByteArray& input);

private:

    void initActionsConnections();

private:
    void showStatusMessage(const QString &message);
    QVector<double> x, y;
    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    Console *m_input = nullptr;
    Console *m_output = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;
    QStandardItemModel* model = nullptr;
    Parser pars;
    QThread thread1;
};

#endif // MAINWINDOW_H
