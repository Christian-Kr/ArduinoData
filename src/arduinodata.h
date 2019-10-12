#ifndef ARDUINODATA_H
#define ARDUINODATA_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFile>

namespace Ui {
class ArduinoData;
}

class ArduinoData : public QWidget {
    Q_OBJECT

public:
    explicit ArduinoData(QWidget *parent = 0);
    ~ArduinoData();

    // Start or stop the serial port connection
public slots:
    void startStop();

    // Read data from port
    void readData();

    // Update the baud rate after it has been changed
    void updateBaudRate(QString key);

    // Clear the log
    void clearLog();

    // Open a file to save
    void filePath();

private:
    // Update the text on the start/stop button
    void updateStartStopButton();

    // Add text to log
    void addLogInfo(const QString &info);

    // Export buffer to a file
    void exportBuffer(QByteArray tmp);

    // Update list of all available devices
    void updateAvailableDevices();

    Ui::ArduinoData *ui;

    QSerialPort *serialPort;
    QByteArray *serialBuffer;
    QMap<int, QSerialPort::BaudRate> *baudRates;
    QFile *exportFile;

    bool serialPortOpen;
    int baudRate;
};

#endif // ARDUINODATA_H
