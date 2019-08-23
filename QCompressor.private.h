#ifndef QCOMPRESSORprivate_H
#define QCOMPRESSORprivate_H

#include <QChartView>
#include <QXYSeries>
#include <QtCore/QtMath>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QVBoxLayout>

QT_CHARTS_USE_NAMESPACE

using LogisticUI = std::tuple<QVBoxLayout*,QSlider*,QSlider*,QSlider*>;
class MyLogistic;
class QLogisticChartView : public QChartView
{

public:
    explicit QLogisticChartView(QChart *chart, QXYSeries *_series, QXYSeries *_marker, MyLogistic *_logistic, QWidget *parent = 0);
    ~QLogisticChartView(){}
    void adjustRanges();

private:
    QPushButton *generator, *reset;
    QXYSeries  *g_series;
    QXYSeries *series;
    QXYSeries *marker;
    QXYSeries *control;
    QPointF pivote;
    QSlider *pivote_x, *pivote_y;
    bool cached, inhibit, persistence;
    MyLogistic *logistic;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

};
#endif // QCOMPRESSOR_H
