// -*- C++ -*-

#if !defined(TOWERSIM_ENERGYMETER_CHARTVIEW_H)
#define TOWERSIM_ENERGYMETER_CHARTVIEW_H

#include "chart.h"
#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
   public:
      ChartView(Chart *chart, QWidget *parent = 0);

   protected:
      bool viewportEvent(QEvent *event);
      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void keyPressEvent(QKeyEvent *event);

   private:
      Chart* m_chart;
      bool m_isTouching;
};

#endif
