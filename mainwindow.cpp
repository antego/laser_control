#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    status = new QLabel;
    ui->statusBar->addWidget(status);
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->portComboBox->addItem(info.portName());
    }

    connect(ui->connectButton, &QPushButton::released, this, &MainWindow::openSerialPort);
    connect(ui->disconnectButton, &QPushButton::released, this, &MainWindow::closeSerialPort);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->powerSlider, &QSlider::valueChanged, ui->powerSpin, &QSpinBox::setValue);
    connect(ui->powerSpin, SIGNAL(valueChanged(int)), ui->powerSlider, SLOT(setValue(int)));
    connect(ui->powerSpin, SIGNAL(valueChanged(int)), this, SLOT(updatePower(int)));
}

void MainWindow::openSerialPort()
{
    serial->setPortName(ui->portComboBox->currentText());
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->setDataTerminalReady(false);

    if (serial->open(QIODevice::ReadWrite)) {
        ui->connectButton->setDisabled(true);
        ui->disconnectButton->setEnabled(true);
        ui->laserControls->setEnabled(true);
        showStatusMessage(tr("Connected"));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
    {
        serial->close();
    }
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setDisabled(true);
    ui->laserControls->setDisabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::updatePower(int newPowerPct)
{
    serial->write(QString("p%1\r").arg(newPowerPct).toStdString().c_str());
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Laser Control"),
                       tr("Control laser by arduino.\nhttps://github.com/antego/laser_control"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
