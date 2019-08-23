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
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

QT_CHARTS_USE_NAMESPACE

using LogisticUI = std::tuple<QVBoxLayout*,QSlider*,QSlider*,QSlider*>;
class MyLogistic;
class QLogisticChartView : public QChartView
{

public:
    explicit QLogisticChartView(QChart *chart, QXYSeries *_series, QXYSeries *_marker, MyLogistic *_logistic, QWidget *parent = 0);
    ~QLogisticChartView();
    void adjustRanges();

private:
    QPushButton *generator, *reset, *saver;
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

    struct {
        gsl_interp_accel *acc;
        gsl_spline *spline;
        double x_knot[20], y_knot[20];
        std::tuple <double[100],double[100]> interpolator;
        size_t interpolator_size;
    } gsl;
};
#endif // QCOMPRESSOR_H

