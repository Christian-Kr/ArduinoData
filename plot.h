#ifndef PLOT_H
#define PLOT_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class Plot : public QChartView {
    Q_OBJECT

public:
    /**
     * @brief Plot Contruction of the object.
     * @param parent Container object
     */
    explicit Plot(QWidget *parent = Q_NULLPTR);

    /**
     * @brief ~Plot Destruction including deletion of all objects.
     */
    ~Plot();

    /**
     * @brief allLineSeries Return all the created line series objects.
     * @return A pointer to all objects. (Deleted by this)
     */
    inline QList<QLineSeries*>* allLineSeries() { return series; }

    /**
     * @brief oneLineSeries Return one line series object.
     * @param i The number of the line series object.
     * @return The object or NULL when i does not exist.
     */
    inline QLineSeries* oneLineSeries(uint i) { return series->at(i); }

    /**
     * @brief createLineSeries Create a defined number of line series objects.
     *        The already existing objects won't be removed.
     * @param num The number of objects that should be created.
     * @return The new number of objects.
     */
    uint createLineSeries(uint num);

    /**
     * @brief deleteLineSeries Empty the chart and DELETE ALL line series
     *        objects.
     */
    void deleteLineSeries();

    /**
     * @brief lineSeriesCount Get the current number of line series objects.
     * @return The number of line series objects.
     */
    uint lineSeriesCount() { return series->size(); }

    /**
     * @brief parseAppendData Parse the data and append them to the line series
     *        objects.
     * @param data The data object.
     */
    void parseAppendData(QByteArray data);

    /**
     * @brief initChartView Initialize the view before the first action.
     */
    void initChartView();

signals:

public slots:

private:
    QChart *chart;
    QList<QLineSeries*> *series;
    QValueAxis *valueAxisX;
    QValueAxis *valueAxisY;

    float lowestValue;
    float highestValue;

    uint rangeX;
};

#endif // PLOT_H
