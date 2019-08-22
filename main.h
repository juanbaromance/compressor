#ifndef MAIN_H
#define MAIN_H

#include <QChartView>
#include <QXYSeries>
#include <QtCore/QtMath>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QSplineSeries>
#include <QScatterSeries>
QT_CHARTS_USE_NAMESPACE

class QChartViewDerivated : public QChartView
{
public:
    QChartViewDerivated(QChart *chart, QXYSeries *_series, QXYSeries *_marker, QWidget *parent = 0);

private:
    QPushButton *generator, *reset;
    QSplineSeries *g_series;
    QXYSeries *series;
    QXYSeries *marker;
    QScatterSeries *control;
    QPointF pivote;
    QSlider *pivote_x, *pivote_y;
    bool cached, inhibit;

    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

};

#endif // MAIN_H
