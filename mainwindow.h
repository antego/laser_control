#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    static const char enableLaserKey = 'e';
    static const char disableLaserKey = 'd';
    static const char powerLaserKey = 'p';

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void enableLaser();
    void disableLaser();
    void updatePower(int newPowerPct);
    void showStatusMessage(const QString &message);
    void handleError(QSerialPort::SerialPortError error);

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QLabel *status;
    bool laserEnabled;

};

#endif // MAINWINDOW_H
