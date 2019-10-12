#include <plot.h>

#include <QtCharts/QValueAxis>
#include <QtCore/QtDebug>
#include <QApplication>

Plot::Plot(QWidget *parent) :
    QChartView(parent),
    chart(new QChart()),
    series(new QList<QSplineSeries*>()),
    valueAxisX(new QValueAxis()),
    valueAxisY(new QValueAxis()) {
}

Plot::~Plot() {
    delete chart;
    delete valueAxisX;
    delete valueAxisY;
}

void Plot::initChartView() {
    // We need a minimum of one line series
    createLineSeries(1);

    // Number of values displayed on the x axis
    rangeX = 500;
    valueAxisX->setRange(0, rangeX);

    // Set the default values for both axis
    valueAxisX->setTickCount(10);
    valueAxisX->setTitleText(tr("Timestep []"));
    valueAxisY->setRange(0, 100);
    valueAxisY->setTickCount(10);

    // Set chart options
    chart->setAxisX(valueAxisX, series->at(0));
    chart->setAxisY(valueAxisY, series->at(0));
    chart->setTitle(tr("Arduino Signalwerte"));

    // Set different view options
    setRenderHint(QPainter::Antialiasing);

    // Set chart
    setChart(chart);
}

uint Plot::createLineSeries(uint num) {
    while (num > 0) {
        QSplineSeries *lineSeries = new QSplineSeries(chart);
        lineSeries->setUseOpenGL(true);
        series->append(lineSeries);
        lineSeries->setName(tr("Signal ") + QString::number(series->size()));
        chart->addSeries(lineSeries);
        num--;
    }

    return series->count();
}

void Plot::deleteLineSeries() {
    chart->removeAllSeries();
    while (series->size() > 0)
        delete series->first();
}

void Plot::parseAppendData(QByteArray data) {
    // The data we get need some format otherwise parsing will fail
    if (!data.endsWith('\n')) {
        qDebug() << "Error: data doesn't end with a newline char";
        return;
    }

    // Remove the last newline char, as now we don't need it anymore
    data.remove(data.size()-1, 1);

    /* Finding a space or \r indicates a new line series, so get a chunk of
    data and search for this chars. */
    QList<QByteArray> dataLines = data.split('\n');

    for (int i = 0; i < dataLines.size(); i++) {
        QByteArray dataLine = dataLines.at(i).simplified();

        /* If we already have to enought range elements, delete thr first one
        and append the new one. */
        if (series->size() < 1)
            createLineSeries(1);

        if (series->at(0)->points().size() > rangeX-1)
            series->at(0)->removePoints(0, 1);

        int x = 0;
        if (series->at(0)->points().size() > 0) {
            x = series->at(0)->points().last().x()+1;
            valueAxisX->setRange(x-rangeX, x);
        }

        // on the first data set minimum and maximum value
        if (x < 1) {
            highestValue = dataLine.toFloat()+1;
            lowestValue = dataLine.toFloat()-1;
        }

        if (dataLine.toFloat() > highestValue-1)
            highestValue = dataLine.toFloat()+1;
        if (dataLine.toFloat() < lowestValue+1)
            lowestValue = dataLine.toFloat()-1;

        valueAxisY->setRange(lowestValue, highestValue);

        series->at(0)->append(QPointF(x, dataLine.toFloat()));
        qDebug() << dataLine << " " << dataLine.toFloat();
    }
}
