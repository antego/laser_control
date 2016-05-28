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
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readResponse);
    connect(ui->onButton, &QPushButton::released, this, &MainWindow::enableLaser);
    connect(ui->offButton, &QPushButton::released, this, &MainWindow::disableLaser);
}

void MainWindow::openSerialPort()
{
    serial->setPortName(ui->portComboBox->currentText());
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

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

void MainWindow::enableLaser()
{
    serial->write(QByteArray().append('e').append((char)ui->powerSpin->value()));
}

void MainWindow::setInterfaceEnable() {
    ui->onButton->setDisabled(true);
    ui->offButton->setEnabled(true);
}

void MainWindow::setInterfaceDisable() {
    ui->onButton->setEnabled(true);
    ui->offButton->setDisabled(true);
}

void MainWindow::disableLaser()
{
    serial->write(QByteArray().append('d'));
}

void MainWindow::updatePower(int newPowerPct)
{
    serial->write(QByteArray().append('p').append((char)newPowerPct));
}

void MainWindow::readResponse()
{
    QByteArray resp = serial->readAll();
    if ((char)resp.at(0) == 'e') {
        setInterfaceEnable();
        ui->powerSpin->setValue(resp.at(1));
    } else if ((char)resp.at(0) == 'd') {
        setInterfaceDisable();
    }
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
                       tr("Control laser by arduino."));
}

MainWindow::~MainWindow()
{
    delete ui;
}
