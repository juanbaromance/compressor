#include "QCompressor.h"
#include "QCompressor.private.h"
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QVector>
#include <QValueAxis>
#include "common_cpp/CLogistic.h"
#include <QToolTip>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QSpacerItem>
#include <QPushButton>
#include <QSlider>
#include <QVariant>

#include <QFileDialog>
#include <QDateTime>
#include <QTextStream>
#include <QFileInfo>

#include <tuple>
#include <iostream>
#include <iomanip>
using namespace std;
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "boost/date_time/posix_time/posix_time.hpp"
typedef boost::posix_time::ptime Time;
typedef boost::posix_time::time_duration TimeDuration;

#include <cstring>

static void MyHandler( const char * reason, const char * file, int line, int gsl_errno )
{

}

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
        status = QString("<small><b>Logistic(%1)</b> Plateau.G(<b>%2</b>)/Slope.k1(<b>%3</b>)/HalfPlateau.k2(<b>%4</b>)<small>")
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
    chart->setAnimationOptions(QChart::SeriesAnimations);

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

    QPushButton *reset;
    ( reset = new QPushButton(this) )->setObjectName("Reset");
    QPushButton *generator;
    ( generator = new QPushButton(this) )->setObjectName("Generator");
    QPushButton *save;
    ( save = new QPushButton(this) )->setObjectName("MapSaver");
    QPushButton *zoom = new QPushButton(this);
    zoom->setObjectName("zoom");

    QLogisticChartView *chartView = new QLogisticChartView( chart, dots, marker, logistic, this );
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);

    QObject::connect(zoom,&QPushButton::clicked,[=](){ chartView->adjustRanges(); });

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
        pivote_box->addWidget( new QLabel("<small><i>Control point tweak<i></small>") );

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

        auto buttonSetUp = [=]( QPushButton *b, QString icon_reference, QString tooltip ){
            pivote_box->addWidget( b );
            QIcon icon;
            icon.addFile(icon_reference, QSize(), QIcon::Normal);
            b->setIcon(icon);
            b->setToolTip(tooltip);
        };

        buttonSetUp( generator,":/pixmaps/generator.png","<small><b>Splined</b> curve generation through tweaking points</small>");
        buttonSetUp( zoom     ,":/pixmaps/zoom.png","<small><b>AutoZoom</b> operation</small>");
        buttonSetUp( reset    ,":/pixmaps/reset.png","<small><b>Initialise</b> curves</small>");
        buttonSetUp( save     ,":/pixmaps/save.png","<small><b>Save</b> tweaked curve on ASCII file</small>");

        pivote_box->setSpacing(2);
        layout->addItem(pivote_box);
    }

    setLayout(layout);

}
QLogisticChartView::~QLogisticChartView()
{
    gsl_spline_free ( gsl.spline );
    gsl_interp_accel_free (gsl.acc);

}

QLogisticChartView::QLogisticChartView(QChart *chart, QXYSeries *_series, QXYSeries *_marker, MyLogistic *_logistic, QWidget *parent)
    : QChartView(chart, parent),
    operation_mask(0),
    series( _series ), marker( _marker ),  persistence( false ), logistic( _logistic )
{
    ( spline_series = new QSplineSeries(this) )->setName("QT.Splines");
    ( nurbs_series = new QLineSeries(this) )->setName("Nurbs");
    ( gsl_series = new QLineSeries(this) )->setName("GSL.Splines");



    control = new QScatterSeries(this);
    cached = false;

    //operation_mask.set(GSLSplines);
    size_t so_interpolation = 100;
    {
        gsl.soInterpolation = so_interpolation;
        gsl.acc = gsl_interp_accel_alloc ();
        if( series->pointsVector().size() > 20 )
            assert(0);
        gsl_set_error_handler( MyHandler );

        gsl.spline = gsl_spline_alloc( gsl_interp_steffen, series->pointsVector().size());

    }

    operation_mask.set(Nurbs);
    {
        nurbs.interface.soInterpolation = so_interpolation;
        nurbs.interface.noKnots = series->pointsVector().size();
        nurbs.interface.order = 7;
        nurbs.generator = nurbs_generator;
    }


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
    for( auto p : { pivote_x, pivote_y })
        QObject::connect( p, QOverload<int>::of(& QSlider::valueChanged), f );

    auto interpolator = [this]()
    {

        QString tmp = "./" + QDateTime::currentDateTime().toString("hhmmddMMyyyy") + ".compressor.txt";
        QString fileName = QFileDialog::getSaveFileName(0,tr("Save buffer"), tmp, tr("(*.txt)"));
        QFile file(fileName);
        if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text ) )
            return;

        {
            QString buffer;
            for ( size_t j = 0; j < gsl.soInterpolation; j++)
            {
                if( operation_mask.test(GSLSplines) )
                    buffer += QString("%1 %2")
                                  .arg( std::get<0>( gsl.interpolator )[j])
                                  .arg( std::get<1>( gsl.interpolator )[j]);
                if( operation_mask.test(Nurbs) )
                    buffer += QString("%1 %2")
                                  .arg( nurbs.interface.data.x[j] )
                                  .arg( nurbs.interface.data.y[j] );
                buffer += "\n";
            }

            QTextStream out( & file );
            out << QDateTime::currentDateTime().toString("## ddMMyyyy hh:mm:ss.zzz:: ");
            out << QString("## file=\"%1.compressor.txt\";plot file u 1:2, file u 3:4 w p\n").arg( QFileInfo(file).baseName() );
            out << buffer;

            auditory(fileName);
        }

        file.close();
    };

    QObject::connect( saver = parent->findChild<QPushButton*>("MapSaver"), & QPushButton::clicked, interpolator );

    generator = parent->findChild<QPushButton*>("Generator");

    QObject::connect(generator,&QPushButton::clicked,[=](){
        adjustRanges();
        size_t elapsed = 0;

        auto series_setup = [=](QXYSeries *series )
        {
            chart->removeSeries(series);
            chart->addSeries( series );
            series->attachAxis(chart->axisX());
            series->attachAxis(chart->axisY());
        };

        if( operation_mask.test(QTSplines) )
         {
            spline_series->clear();
            for( auto p : series->pointsVector() )
                spline_series->append( p );
            series_setup( spline_series );
        }

        if( operation_mask.test(GSLSplines) )
         {

            size_t i(0);
            for( auto p : series->pointsVector() )
            {
                gsl.x_knot[i] = p.x();
                gsl.y_knot[i] = p.y();
                i++ ;
            }

            Time offset_start = boost::posix_time::microsec_clock::local_time();
            gsl_spline_init(gsl.spline, gsl.x_knot, gsl.y_knot, series->pointsVector().size());
            float scaler = gsl.spline->interp->xmax/gsl.soInterpolation;
            for( size_t j = 0; j < gsl.soInterpolation; j++ )
            {
                double x_map = j * scaler;
                std::get<0>( gsl.interpolator )[j] = x_map;
                std::get<1>( gsl.interpolator )[j]= gsl_spline_eval (gsl.spline, x_map, gsl.acc);
            }
            Time now = boost::posix_time::microsec_clock::local_time();
            elapsed = ( now - offset_start ).total_microseconds();
            auditory( QString("GSL latency <b>%1</b>(microsec)").arg(elapsed) );

            gsl_series->clear();
            for( size_t j = 0; j < gsl.soInterpolation; j++ )
                gsl_series->append( std::get<0>( gsl.interpolator )[j], std::get<1>( gsl.interpolator )[j] );
            series_setup(gsl_series);
        }

        if( operation_mask.test(Nurbs) )
        {
            size_t i(0);
            memset( & nurbs.interface.data, 0, sizeof( nurbs.interface.data ));
            for( auto p : series->pointsVector() )
            {
                nurbs.interface.data.x[i] = p.x();
                nurbs.interface.data.y[i] = p.y();
                i++ ;
            }

            Time offset_start = boost::posix_time::microsec_clock::local_time();
            nurbs.generator( & nurbs.interface );
            Time now = boost::posix_time::microsec_clock::local_time();
            elapsed = ( now - offset_start ).total_microseconds();
            auditory( QString("NURBS latency <b>%1</b>(microsec)").arg(elapsed) );

            nurbs_series->clear();
            for ( size_t i = 0; i < nurbs.interface.soInterpolation; i++ )
                nurbs_series->append( nurbs.interface.data.x[i], nurbs.interface.data.y[i] );
            series_setup(nurbs_series);
        }

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

void QLogisticChartView::auditory(QString report)
{
    QString tooltip = saver->toolTip();
    static QString header;
    if ( header.size() == 0 )
        header = tooltip;
    if( tooltip.size() > 256 )
        tooltip = QString("<b>%1</b>").arg( header.size() ? header + "<br/>" : header );
    saver->setToolTip( tooltip + "<br/>" + QString("<small>%1</small>").arg(report) );
}

