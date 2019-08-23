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

using LogisticParams = logistic_parameters_t;
class MyLogistic : public CLogistic
{

public :

    LogisticUI ui;

    MyLogistic( QVector<QXYSeries*> records, float plateau, float slope, float half_plateau, QChart *chart, size_t sampling = 15 )
        : CLogistic ( plateau , slope , half_plateau )
    {
       ui = makeUI();
       populate( records, sampling );

    }
    QString report(){ return QString("Plateau(%1)/Slope(%2)/HalfPlateau(%3)").arg(G()).arg(k1()).arg(k2()); }
    void populate( QVector<QXYSeries*> & records,  size_t sampling, LogisticParams *params  = nullptr)
    {
        G ( params == nullptr ? std::get<1>(ui)->value() : params->G);
        k1( params == nullptr ? std::get<2>(ui)->value()/1000. : params->k1);
        k2( params == nullptr ? std::get<3>(ui)->value() : params->k2);

        float scaler = k2()*2/sampling;
        for( auto s : records )
        {
            s->clear();
            for( size_t j = 0; j < sampling; j++ )
            {
                float x_map = j * scaler;
                s->append( round(x_map), round(eval( x_map )));
            }
        }
    }

private:
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


QCompressor::QCompressor(QWidget *parent) : QWidget(parent)
{
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);

    QSplineSeries *series;
    QScatterSeries *dots;
    QVector<QXYSeries*> records;
    MyLogistic logistic( records = { series = new QSplineSeries(this), dots = new QScatterSeries(this) }, 230, 0.236, 30, chart );
    series->setName("Logistic-Splined");
    dots->setName("Logistic-Sampled");

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

    QChartView *chartView = new QLogisticChartView( chart, dots, marker, logistic.ui, this );
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget( chartView );
    layout->addItem( std::get<0>(logistic.ui) );

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

QLogisticChartView::QLogisticChartView(QChart *chart, QXYSeries *_series, QXYSeries *_marker, LogisticUI & logistic_ui, QWidget *parent)
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

        setToolTip( QString("<small>(%1,%2)</small>").arg(point.x()).arg(point.y()) );
        cached = false;
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
        MyLogistic logistic( { series }, 230, 0.236, 30, chart );
        inhibit = false;
    });

    for( auto i : { std::get<1>(logistic_ui),  std::get<2>(logistic_ui),  std::get<3>(logistic_ui)  })
        connect( i, & QSlider::sliderReleased, [=](){
            chart->removeAllSeries();
            for( auto s : { series, g_series, control })
                s->clear();

        });
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

void QLogisticChartView::populate(int value)
{
    chart()->removeAllSeries();
}
