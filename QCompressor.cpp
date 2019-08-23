#include "QCompressor.h"
#include "QCompressor.private.h"
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
#include <QHBoxLayout>
#include <tuple>
#include <QGraphicsTextItem>

using LogisticParams = logistic_parameters_t;
class MyLogistic : public CLogistic
{

public :

    LogisticUI ui;

    MyLogistic( QVector<QXYSeries*> records, float plateau, float slope, float half_plateau, QChart *_chart, size_t sampling = 15 )
        : CLogistic ( plateau , slope , half_plateau ), chart( _chart )
    {
       ui = makeUI();
       populate( records, sampling );
    }

    QString populate( QVector<QXYSeries*> & records,  size_t sampling = 15, LogisticParams *params  = nullptr)
    {

        G ( params == nullptr ? abs(std::get<1>(ui)->value())  : params->G);
        k1( params == nullptr ? std::get<2>(ui)->value()/1000. : params->k1);
        k2( params == nullptr ? std::get<3>(ui)->value()       : params->k2);

        {
            double _zero = fabs( eval( 0 ) );
            if( _zero > 2.0 )
            {
                status = QString("<small><b><big><font color=\"Navy\">OOrange</font><big></b> K1(%1)K2(%2) exceeds <b>%3</b></small>")
                                  .arg( G()).arg( k1() ).arg( _zero );
                chart->setTitle(status);
                return status;
            }
        }

        float scaler = ( k2()*2 )/sampling;
        for( auto s : records )
        {
            s->clear();
            for( size_t j = 0; j < sampling; j++ )
            {
                float x_map = j * scaler;
                s->append( round(x_map), round(eval( x_map )));
            }
        }
        status = QString("<small><b>Logistic(%1)</b> Plateau.G(%2)/Slope.k1(%3)/HalfPlateau.k2(%4)<small>")
                     .arg(sampling).arg(G()).arg(k1()).arg(k2());
        chart->setTitle(status);
        return status;
    }

private:
    QChart *chart;
    QString status;

    LogisticUI makeUI()
    {
        QVBoxLayout *logisticLayout = new QVBoxLayout();
        QHBoxLayout *horizontalLayout_13 = new QHBoxLayout();
        QHBoxLayout *horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));

        QLabel *label_9 = new QLabel("<small><i> Slope </i></small>");
        horizontalLayout_10->addWidget(label_9);
        QSpinBox *K1Spb = new QSpinBox();
        K1Spb->setObjectName(QStringLiteral("K1Spb"));
        K1Spb->setMinimum(5);
        K1Spb->setMaximum(800);
        K1Spb->setValue(k1()*1000);
        horizontalLayout_10->addWidget(K1Spb);
        QSlider *K1Slider = new QSlider();
        K1Slider->setObjectName(QStringLiteral("K1Slider"));
        K1Slider->setMinimum(5);
        K1Slider->setMaximum(800);
        K1Slider->setValue(k1()*1000);
        K1Slider->setOrientation(Qt::Horizontal);
        QObject::connect(K1Spb,SIGNAL(valueChanged(int)),K1Slider,SLOT(setValue(int)));
        QObject::connect(K1Slider,SIGNAL(valueChanged(int)),K1Spb,SLOT(setValue(int)));

        horizontalLayout_10->addWidget(K1Slider);
        horizontalLayout_13->addLayout(horizontalLayout_10);

        QHBoxLayout * horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        QLabel* label_10 = new QLabel("<small><i> Half Plateau </i></small>");
        label_10->setObjectName(QStringLiteral("label_10"));
        horizontalLayout_11->addWidget(label_10);

        QSpinBox *K2Spb = new QSpinBox();
        K2Spb->setObjectName(QStringLiteral("K2Spb"));
        K2Spb->setMinimum(5);
        K2Spb->setMaximum(60);
        K2Spb->setValue(k2());

        horizontalLayout_11->addWidget(K2Spb);

        QSlider * K2Slider = new QSlider( );
        K2Slider->setObjectName(QStringLiteral("K2Slider"));
        K2Slider->setMinimum(5);
        K2Slider->setMaximum(60);
        K2Slider->setPageStep(10);
        K2Slider->setValue(k2());
        K2Slider->setOrientation(Qt::Horizontal);
        QObject::connect(K2Spb,SIGNAL(valueChanged(int)),K2Slider,SLOT(setValue(int)));
        QObject::connect(K2Slider,SIGNAL(valueChanged(int)),K2Spb,SLOT(setValue(int)));

        horizontalLayout_11->addWidget(K2Slider);
        horizontalLayout_13->addLayout(horizontalLayout_11);

        logisticLayout->addLayout(horizontalLayout_13);

        QHBoxLayout *horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        QLabel *label_7 = new QLabel("<small><i> Plateau </i></small>");
        horizontalLayout_9->addWidget(label_7);

        QSpinBox* GSpb = new QSpinBox();
        GSpb->setObjectName(QStringLiteral("GSpb"));
        GSpb->setMinimum(-500);
        GSpb->setMaximum(500);
        GSpb->setValue(G());

        horizontalLayout_9->addWidget(GSpb);

        QSlider *GSlider = new QSlider();
        GSlider->setObjectName(QStringLiteral("GSlider"));
        GSlider->setMinimum(-500);
        GSlider->setMaximum(500);
        GSlider->setValue(G());
        GSlider->setOrientation(Qt::Horizontal);
        QObject::connect(GSpb,SIGNAL(valueChanged(int)),GSlider,SLOT(setValue(int)));
        QObject::connect(GSlider,SIGNAL(valueChanged(int)),GSpb,SLOT(setValue(int)));

        horizontalLayout_9->addWidget(GSlider);
        logisticLayout->addLayout(horizontalLayout_9);

        QFrame* line_4 = new QFrame();
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        logisticLayout->addWidget(line_4);

        return ui = std::make_tuple(logisticLayout,GSlider,K1Slider,K2Slider);
    }


};



static QVector<QXYSeries*> records;

QCompressor::QCompressor(QWidget *parent) : QWidget(parent)
{
    static MyLogistic *logistic;
    QChart *chart = new QChart();
    chart->legend()->hide();
    //chart->setAnimationOptions(QChart::AllAnimations);

    QSplineSeries *series;
    QScatterSeries *dots;

    logistic = new MyLogistic( records = { series = new QSplineSeries(this), dots = new QScatterSeries(this) }, 230, 0.236, 30, chart );

    series->setName("Logistic-Splined");
    dots->setName("Logistic-Sampled");

    QScatterSeries *marker= new QScatterSeries(this);
    marker->setColor(Qt::blue);
    marker->setName("Marker");
    marker->setMarkerSize( marker->markerSize()* 1.50 );

    for( auto s : records )
        chart->addSeries(s);
    chart->addSeries(marker);

    chart->createDefaultAxes();
    chart->axisX()->setTitleText("Effort Units");
    chart->axisY()->setTitleText("U.A");

    chart->axisX()->setGridLineVisible(true);
    chart->axisY()->setGridLineVisible(true);
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

    QLogisticChartView *chartView = new QLogisticChartView( chart, dots, marker, logistic, this );
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);

    for( auto i : { std::get<1>(logistic->ui),  std::get<2>(logistic->ui),  std::get<3>(logistic->ui)  })
        connect( i, & QSlider::sliderReleased, [=](){
            logistic->populate( records );
        });


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget( chartView );
    layout->addItem( std::get<0>(logistic->ui) );

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
        y_pivote->setMaximum( logistic->G() * 1.2 );
        x_pivote->setMaximum( logistic->k2() * 2 * 1.2 );

        {
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/pixmaps/generator.png"), QSize(), QIcon::Normal);
            generator->setIcon(icon);
            generator->setToolTip("<small><b>Splined</b> curve generator through tweaked points</small>");
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
                chartView->adjustRanges();
            });
            zoom->setToolTip("<small><b>AutoZoom</b> operation</small>");
        }

        {
            pivote_box->addWidget( reset );
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/pixmaps/reset.png"), QSize(), QIcon::Normal);
            reset->setIcon(icon);
            reset->setToolTip("<small><b>Initialise</b> curves</small>");
        }

        pivote_box->setSpacing(2);
        layout->addItem(pivote_box);
    }

    setLayout(layout);

}

QLogisticChartView::QLogisticChartView(QChart *chart, QXYSeries *_series, QXYSeries *_marker, MyLogistic *_logistic, QWidget *parent)
    : QChartView(chart, parent), series( _series ), marker( _marker ), logistic( _logistic ), persistence( false )
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

        setToolTip( QString("<small>(%1,%2)</small>").arg(point.x()).arg(point.y()) );
        cached = false;
        marker->clear();
        marker->append( point.x(), point.y() );
        pivote = point;
        pivote_x->setValue( pivote.x() );
        pivote_y->setValue( pivote.y() );
        cached = true;
    });
    generator = parent->findChild<QPushButton*>("Generator");
    auto f = [=]( int value ) {
        (void)value;
        if( cached == false || ( inhibit == true ) )
            return;
        QPointF replacement( pivote_x->value(), pivote_y->value() );
        series->replace( pivote, replacement );
        marker->replace( pivote, replacement );
        pivote = replacement;
        generator->click();
    };
    pivote_x->connect( pivote_x, QOverload<int>::of(& QSlider::valueChanged), f );
    pivote_y->connect( pivote_y, QOverload<int>::of(& QSlider::valueChanged), f );

    QObject::connect(generator,&QPushButton::clicked,[=](){
        adjustRanges();
        chart->removeSeries(g_series);
        g_series->clear();
        for( auto p : series->pointsVector() )
            g_series->append( p );
        chart->addSeries( g_series );
    });

    QObject::connect(parent->findChild<QPushButton*>("Reset"),&QPushButton::clicked,[=](){
        inhibit = true;
        adjustRanges();

        if( persistence )
        {
            chart->removeSeries( control );
            control->clear();
            for( auto p : series->pointsVector() )
                control->append( p );
            chart->addSeries( control );
        }

        logistic->populate(records);
        inhibit = false;
    });


}

void QLogisticChartView::adjustRanges()
{
    float x_min = 0,x_max = 0,y_min = 0, y_max = 0;
    for( auto p : series->pointsVector() )
    {
        if( p.x() < x_min )
            x_min = p.x();
        else if ( p.x() > x_max )
            x_max = p.x();

        if( p.y() < y_min )
            y_min = p.y();
        else if ( p.y() > y_max )
            y_max = p.y();
    }

    chart()->axisX()->setRange(x_min,x_max);
    chart()->axisY()->setRange(y_min,y_max);

}


void QLogisticChartView::wheelEvent(QWheelEvent *event)
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

void QLogisticChartView::mouseMoveEvent(QMouseEvent *event)
{
    QChartView::mouseMoveEvent(event);
}
