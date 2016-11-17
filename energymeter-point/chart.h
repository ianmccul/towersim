// -*- C++ -*-

#ifndef CHART_H
#define CHART_H

#include <QtCharts/QChart>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>
#include <QtCore/QSignalMapper>

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Chart: public QChart
{
      Q_OBJECT
   public:
      Chart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
      virtual ~Chart();

      static int const DefaultSensorPort = 5701;

      // Initiates a connection to the given host:port
      // if Retry then attempt to reconnect on error
      void ConnectTo(QString const& Host, int Port = DefaultSensorPort);

      void PlotPoint(double v);

      void SetHandstroke();
      void SetBackstroke();

      void SetBell(int Bell);

      void Reset();

      void ZeroView();

      void NarrowView();

      void DisableAnimations();
      void EnableAnimations();

   public slots:
      void handleScreensaver();
      void handleTimeout();

   private slots:
      void CompleteConnection(QObject* Socket);
      void ReadCommand(QObject* Socket);

   private:
      QTimer screenTimer;
      QTimer m_timer;
      QScatterSeries* handPoints;
      QScatterSeries* backPoints;
      QLineSeries* hthresh;
      QLineSeries* bthresh;
      double HEnergy, BEnergy;
      QStringList m_titles;
      QValueAxis *m_axis;
      qreal m_step;
      qreal m_x;
      qreal m_y;

      QLinearGradient gradient;

      QSignalMapper* ConnectionMapper;
      QSignalMapper* ConnectionCompletedMapper;
      QTcpServer* Server;

      double yMax;
      double yMin;

      int WatchBell;
};

#endif /* CHART_H */
