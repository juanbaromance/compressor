#include "QCompressor.h"
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QVector>
#include <QValueAxis>
#include "common_cpp/CLogistic.h"
#include "main.h"
#include <QToolTip>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QSpacerItem>
#include <QPushButton>
#include <QSlider>
#include <QVariant>


class MyLogistic : public CLogistic
{
public :
    MyLogistic( QVector<QXYSeries*> records, float plateau, float slope, float half_plateau, size_t sampling = 15 )
        : CLogistic ( plateau , slope , half_plateau )
    {
        float scaler = k2()*2/sampling;
        for( auto s : records )
        {
            for( size_t j = 0; j < sampling; j++ )
            {
                float x_map = j * scaler;
                s->append( round(x_map), round(eval( x_map )));
            }
        }
    }
    QString report(){ return QString("Plateau(%1)/Slope(%2)/HalfPlateau(%3)").arg(G()).arg(k1()).arg(k2()); }
};


QCompressor::QCompressor(QWidget *parent) : QWidget(parent)
{
    QSplineSeries *series;
    QScatterSeries *dots;
    QVector<QXYSeries*> records;
    MyLogistic logistic( records = { series = new QSplineSeries(this), dots = new QScatterSeries(this) }, 230, 0.236, 30 );

    series->setName("Logistic-SPlined");
    dots->setName("Logistic-Sampled");

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);


    QScatterSeries *marker= new QScatterSeries(this);
    marker->setColor(Qt::blue);
    marker->setName("Marker");
    marker->setMarkerSize( marker->markerSize()* 1.50 );
    records.append( marker );

    for( auto s : records )
        chart->addSeries(s);

    chart->setTitle(QString("<b><big>Logistic</big></b><br><small>%1</small>").arg(logistic.report()));
    chart->createDefaultAxes();
    chart->axisX()->setTitleText("Effort Units");
    chart->axisY()->setTitleText("U.A");
    chart->axisX()->setGridLineVisible(true);
    chart->axisX()->setMinorGridLineVisible(true);
    chart->axisY()->setMinorGridLineVisible(true);

    QSlider *x_pivote = new QSlider(this), *y_pivote = new QSlider(this);

    x_pivote->setObjectName("PivoteX");
    QVariant value;
    value.setValue(QString("X "));
    x_pivote->setProperty("Auditor", value );

    y_pivote->setObjectName("PivoteY");
    value.setValue(QString("Y "));
    y_pivote->setProperty("Auditor", value );

    QPushButton *reset= new QPushButton(this);
    reset->setObjectName("Reset");
    QPushButton *generator = new QPushButton(this);
    generator->setObjectName("Generator");

    QChartView *chartView = new QChartViewDerivated( chart, dots, marker, this );
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget( chartView );

    {
        QHBoxLayout *pivote_box = new QHBoxLayout;
        pivote_box->addItem( new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum) );

        for( auto s : { x_pivote, y_pivote } )
        {
            s->setPageStep(20);
            s->setOrientation(Qt::Horizontal);
            QLabel *l = new QLabel( "",this);
            pivote_box->addWidget( l );
            pivote_box->addWidget( s );
            connect( s, SIGNAL(valueChanged(int)),l,SLOT(setNum(int)));

        }
        y_pivote->setMaximum( logistic.G() * 1.2 );
        x_pivote->setMaximum( logistic.k2() * 2 * 1.2 );

        {
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/pixmaps/generator.png"), QSize(), QIcon::Normal);
            generator->setIcon(icon);
        }

        pivote_box->addWidget( generator );

        {
            QPushButton *zoom= new QPushButton(this);
            zoom->setObjectName("zoom");
            pivote_box->addWidget( zoom );
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/pixmaps/zoom.png"), QSize(), QIcon::Normal);
            zoom->setIcon(icon);
            QObject::connect(zoom,&QPushButton::clicked,[=](){
                chart->zoomReset();
            });
        }

        {
            pivote_box->addWidget( reset );
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/pixmaps/reset.png"), QSize(), QIcon::Normal);
            reset->setIcon(icon);
        }

        pivote_box->setSpacing(2);
        layout->addItem(pivote_box);
    }

    setLayout(layout);

}

QChartViewDerivated::QChartViewDerivated(QChart *chart, QXYSeries *_series, QXYSeries *_marker, QWidget *parent)
    : QChartView(chart, parent), series( _series ), marker( _marker )
{
    g_series = new QSplineSeries(this);
    g_series->setName("Tweak");
    control = new QScatterSeries(this);
    cached = false;

    setRubberBand(QChartView::RectangleRubberBand);
    series->connect(series, &QXYSeries::clicked, [=](const QPointF &point){
        QPointF clickedPoint = point;
        QPointF closest(INT_MAX, INT_MAX);
        qreal distance(INT_MAX);
        const auto points = series->points();
        for (const QPointF &currentPoint : points) {
            qreal currentDistance = qSqrt((currentPoint.x() - clickedPoint.x())
                                              * (currentPoint.x() - clickedPoint.x())
                                          + (currentPoint.y() - clickedPoint.y())
                                                * (currentPoint.y() - clickedPoint.y()));
            if (currentDistance < distance) {
                distance = currentDistance;
                closest = currentPoint;
            }
        }
    });

    chart->acceptHoverEvents();
    pivote_x = parent->findChild<QSlider*>("PivoteX");
    pivote_y = parent->findChild<QSlider*>("PivoteY");

    series->connect(series, &QXYSeries::hovered, [=]( QPointF point, bool state ){
        if( state == false || ( inhibit == true ) )
            return;
        cached = false;
        setToolTip( QString("<small>(%1,%2)</small>").arg(point.x()).arg(point.y()) );
        marker->clear();
        marker->append( point.x(), point.y() );
        pivote = point;
        pivote_x->setValue( pivote.x() );
        pivote_y->setValue( pivote.y() );
        cached = true;
    });

    auto f = [=]( int value ) {
        (void)value;
        if( cached == false || ( inhibit == true ) )
            return;
        QPointF replacement( pivote_x->value(), pivote_y->value() );
        series->replace( pivote, replacement );
        marker->replace( pivote, replacement );
        pivote = replacement;
    };
    pivote_x->connect( pivote_x, QOverload<int>::of(& QSlider::valueChanged), f );
    pivote_y->connect( pivote_y, QOverload<int>::of(& QSlider::valueChanged), f );

    generator = parent->findChild<QPushButton*>("Generator");
    QObject::connect(generator,&QPushButton::clicked,[=](){
        chart->zoomReset();
        chart->removeSeries(g_series);
        g_series->clear();
        for( auto p : series->pointsVector() )
            g_series->append( p );
        chart->addSeries( g_series );
    });

    QObject::connect(parent->findChild<QPushButton*>("Reset"),&QPushButton::clicked,[=](){
        inhibit = true;
        chart->zoomReset();
        chart->removeSeries( control );
        control->clear();
        for( auto p : series->pointsVector() )
            control->append( p );
        chart->addSeries( control );

        series->clear();
        MyLogistic logistic( { series }, 230, 0.236, 30 );
        inhibit = false;
    });
}

void QChartViewDerivated::wheelEvent(QWheelEvent *event)
{
    qreal factor;
    if ( event->delta() > 0 )
        factor = 2.0;
    else
        factor = 0.5;

    QRectF r = QRectF(chart()->plotArea().left(),chart()->plotArea().top(),
                      chart()->plotArea().width()/factor,chart()->plotArea().height()/factor);
    QPointF mousePos = mapFromGlobal(QCursor::pos());
    r.moveCenter(mousePos);
    chart()->zoomIn(r);
    QPointF delta = chart()->plotArea().center() -mousePos;
    chart()->scroll(delta.x(),-delta.y());
}

void QChartViewDerivated::mouseMoveEvent(QMouseEvent *event)
{
    QChartView::mouseMoveEvent(event);

}
