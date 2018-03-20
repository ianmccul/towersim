#include "chartview.h"
#include <QtGui/QMouseEvent>

ChartView::ChartView(Chart* chart, QWidget *parent) :
    QChartView(chart, parent),
    m_chart(chart),
    m_isTouching(false)
{
    setRubberBand(QChartView::RectangleRubberBand);
}

bool ChartView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        // By default touch events are converted to mouse events. So
        // after this event we will get a mouse event also but we want
        // to handle touch events as gestures only. So we need this safeguard
        // to block mouse events that are actually generated from touch.
        m_isTouching = true;

        // Turn off animations when handling gestures they
        // will only slow us down.
        m_chart->DisableAnimations();

        chart()->setAnimationOptions(QChart::NoAnimation);

    }
    return QChartView::viewportEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isTouching)
        m_isTouching = false;

    // Because we disabled animations when touch event was detected
    // we must put them back on.
    chart()->setAnimationOptions(QChart::SeriesAnimations);

    QChartView::mouseReleaseEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
   m_chart->DisableAnimations();
   switch (event->key())
   {
   case Qt::Key_Plus:
      m_chart->zoomIn();
      break;
   case Qt::Key_Minus:
      m_chart->zoomOut();
      break;
   case Qt::Key_Left:
      m_chart->scroll(-10, 0);
      break;
   case Qt::Key_Right:
      m_chart->scroll(10, 0);
      break;
   case Qt::Key_Up:
      m_chart->scroll(0, 10);
      break;
   case Qt::Key_Down:
      m_chart->scroll(0, -10);
      break;
   case Qt::Key_H:
     m_chart->SetHandstroke();
      break;
   case Qt::Key_B:
     m_chart->SetBackstroke();
      break;
   case Qt::Key_R:
      m_chart->Reset();
      break;
   case Qt::Key_Z:
      m_chart->ZeroView();
      break;
   case Qt::Key_N:
      m_chart->NarrowView();
      break;
   default:
      QGraphicsView::keyPressEvent(event);
      break;
   }
   //m_chart->EnableAnimations();
}
