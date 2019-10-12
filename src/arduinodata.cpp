#include "arduinodata.h"
#include "ui_arduinodata.h"

#include <iostream>
#include <QDateTime>
#include <QFontDatabase>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

ArduinoData::ArduinoData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArduinoData),
    serialPort(new QSerialPort()),
    serialBuffer(new QByteArray()),
    baudRates(new QMap<int, QSerialPort::BaudRate>()),
    exportFile(new QFile()) {

    // Ui
    ui->setupUi(this);

    // Chart view
    ui->graphicsView->initChartView();

    // Serial port
    serialPortOpen = false;
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));

    // Combo box for baud rates
    baudRates->insert(1200, QSerialPort::Baud1200);
    baudRates->insert(2400, QSerialPort::Baud2400);
    baudRates->insert(4800, QSerialPort::Baud4800);
    baudRates->insert(9600, QSerialPort::Baud9600);
    baudRates->insert(19200, QSerialPort::Baud19200);
    baudRates->insert(38400, QSerialPort::Baud38400);
    baudRates->insert(57600, QSerialPort::Baud57600);
    baudRates->insert(115200, QSerialPort::Baud115200);

    QList<int> keys = baudRates->keys();
    QStringList keysString;
    for (int i = 0; i < keys.size(); i++) {
        keysString.append(QString::number(keys.at(i)));
    }

    ui->cBBaudRate->addItems(keysString);
    ui->cBBaudRate->setCurrentText(QString::number(
        baudRates->key(QSerialPort::Baud9600)));
    baudRate = QSerialPort::Baud9600;

    // Init the entries for the available devices
    updateAvailableDevices();

    // Set font
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->tELog->setFont(fixedFont);
    ui->tEPlainData->setFont(fixedFont);
}

ArduinoData::~ArduinoData() {
    delete ui;
    delete serialPort;
    delete baudRates;
    delete exportFile;
}

void ArduinoData::updateAvailableDevices() {
    ui->cBDevice->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo port : ports)
        ui->cBDevice->addItem(port.systemLocation());
}

void ArduinoData::startStop() {
    QString info;
    info += tr("----------\n");
    info += QDateTime::currentDateTime().toString() + tr(" > ");

    if (serialPortOpen == true) {
        // If port is already open, close it...
        serialPort->close();

        serialPortOpen = false;
        info += tr("Verbindung gestoppt!");

        // If export is on, close the file
        if (ui->gBExport->isChecked())
            exportFile->close();

        // Enable the settings tab
        ui->tabSettings->setEnabled(true);
    } else {
        // Disable the settings tab
        ui->tabSettings->setEnabled(false);

        // else, start opening and listen
        serialPort->setPortName(ui->cBDevice->currentText());
        serialPort->setBaudRate(baudRate);

        info += tr("Verbindung gestartet!\n") +
                tr("Gerät:\t\t") + ui->cBDevice->currentText() + tr("\n") +
                tr("Baudrate:\t") + ui->cBBaudRate->currentText() + tr("\n");

        // If export is on, open the file
        if (ui->gBExport->isChecked()) {
            exportFile->setFileName(ui->lEFileName->text());
            if (!exportFile->open(QIODevice::ReadWrite | QIODevice::Text))
                info += tr("Export:\t\tFehler! - ") +
                        exportFile->errorString() + tr("\n");
        }

        // Open the port and check whether an error occurs or not
        if (!serialPort->open(QIODevice::ReadWrite)) {
            info += tr("Status:\t\tFehler! - ") +
                    serialPort->errorString();

            // Enable the settings tab
            ui->tabSettings->setEnabled(true);
        } else {
            info += tr("Status:\t\tErfolgreich!");
            serialPortOpen = true;
        }
    }

    // Update the text on the start/stop button
    updateStartStopButton();

    // Add log info
    addLogInfo(info);
}

void ArduinoData::updateBaudRate(QString key) {
    baudRate = baudRates->value(key.toInt());
}

void ArduinoData::updateStartStopButton() {
    if (serialPortOpen)
        ui->pBStartStop->setText("Stop");
    else
        ui->pBStartStop->setText("Start");
}

void ArduinoData::readData() {
    QByteArray data = serialPort->readAll();

    // Add data to plain tab
    ui->tEPlainData->moveCursor(QTextCursor::EndOfLine);
    ui->tEPlainData->insertPlainText(data);

    serialBuffer->append(data);

    // Plot and/or export only data that ends with a newline char

    // Break if there is no : in bytes. This is protocoll specific. After
    // every value, a : occurs.
    if (!serialBuffer->contains(':'))
        return;

    // Get the first : char and the value before
    int pos = serialBuffer->indexOf(':');
    if (pos == -1)
        return;
    QByteArray tmp = serialBuffer->left(pos);
    qDebug() << "tmp: " << tmp;
    serialBuffer->remove(0, pos);
    qDebug() << "serialBuffer" << serialBuffer->data();

    // Write data to export file
    if (ui->gBExport->isChecked())
        exportBuffer(tmp);

    // Plot the data
    if (pos > -1)
        ui->graphicsView->parseAppendData(tmp);

    // Remove the data that have been exported and plotted
    if (pos > -1)
        serialBuffer->remove(0, pos+1);
}

void ArduinoData::addLogInfo(const QString &info) {
    ui->tELog->append(info);
}

void ArduinoData::clearLog() {
    ui->tELog->clear();
}

void ArduinoData::filePath() {
    QString	fn = QFileDialog::getSaveFileName(this, tr("Exportieren"),
                                              QDir::homePath());
    ui->lEFileName->setText(fn);
}

void ArduinoData::exportBuffer(QByteArray tmp) {
    // Check file object
    if (!exportFile->isOpen() || tmp.isEmpty())
        return;

    QTextStream out(exportFile);
    out << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
        << ";" << QString(tmp.data()).replace(".", ",") << "\n";
}
