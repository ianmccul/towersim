// -*- C++ -*-

#include "chart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpSocket>
#include <QtDBus/QtDBus>
#include "common/trace.h"

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_series(0),
    m_axis(new QValueAxis),
    m_axis_thresholds(new QCategoryAxis),
    m_step(0),
    m_x(3),
    m_y(1),
    ConnectionMapper(new QSignalMapper(this)),
    ConnectionCompletedMapper(new QSignalMapper(this)),
    Server(new QTcpServer(this)),
    yMax(0.0),
    yMin(-1.0)
{
    qsrand((uint) QTime::currentTime().msec());

    // Timer to prevent screen saver
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    m_timer.setInterval(10000);

    m_series = new QSplineSeries(this);
    QPen green(Qt::red);
    green.setWidth(10);
    m_series->setPen(green);
    m_series->append(m_x, m_y);

    addSeries(m_series);
    createDefaultAxes();
    QPen GridLine(Qt::black);
    GridLine.setWidth(4);
    m_axis->setGridLinePen(GridLine);
    setAxisX(m_axis, m_series);
    m_axis->setTickCount(5);
    axisX()->setRange(0, 5);

    // bell 9
    //    axisY()->setRange(400*400, 410*410);
    //axisY()->setRange(0, 410*410);

    // bell 5
    axisY()->setRange(460*460, 475*475);

    axisY()->setGridLinePen(GridLine);

    QPen Marker(Qt::blue);
    Marker.setWidth(8);
    m_axis_thresholds->setGridLinePen(Marker);
    m_axis_thresholds->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    // bell 9
    //    m_axis_thresholds->append("Handstroke", 164050);
    // m_axis_thresholds->append("Backstroke", 165500);

    // bell 5
    m_axis_thresholds->append("Handstroke", 465*465);
    m_axis_thresholds->append("Backstroke", 469*469);

    this->addAxis(m_axis_thresholds, Qt::AlignRight);
    m_series->attachAxis(m_axis_thresholds);

    m_timer.start();
}

Chart::~Chart()
{
}

void
Chart::handleTimeout()
{
   // Stop the screen saver from kicking in
   // system("qdbus org.freedesktop.ScreenSaver /ScreenSaver SimulateUserActivity");
   QDBusInterface dbus_iface("org.freedesktop.ScreenSaver", "/ScreenSaver",
                             "org.freedesktop.ScreenSaver");
   dbus_iface.call("SimulateUserActivity");
}

void
Chart::SetThreshold(QString const& Str)
{
   m_axis_thresholds->remove(Str);
   m_axis_thresholds->append(Str, m_y);
}

void
Chart::ResetView()
{
   this->DisableAnimations();
   this->axisY()->setRange(0.0, yMax+2000);
}

void
Chart::NarrowView()
{
   this->DisableAnimations();
   this->axisY()->setRange(std::max(0.0, yMin-2000), yMax+2000);
}

void
Chart::DisableAnimations()
{
   this->setAnimationOptions(QChart::NoAnimation);
}

void
Chart::EnableAnimations()
{
   this->setAnimationOptions(QChart::SeriesAnimations);
}

void
Chart::PlotPoint(double v)
{
   yMax = std::max(v, yMax);
   if (yMin == -1.0)
      yMin = v;
   else
      yMin = std::min(v, yMin);

   this->setAnimationOptions(QChart::AllAnimations);

   qreal x = plotArea().width() / m_axis->tickCount();
   TRACE(x);
   qreal y = (m_axis->max() - m_axis->min()) / m_axis->tickCount();
   TRACE(y);
   m_x += y;
   m_y = v;
   m_series->append(m_x, m_y);
   scroll(x, 0);
}

void
Chart::ConnectTo(QString const& Host, int Port)
{
   QTcpSocket* Connection = new QTcpSocket(this);
   connect(Connection, SIGNAL(error(QAbstractSocket::SocketError)), Connection, SLOT(deleteLater()));
   connect(Connection, SIGNAL(disconnected()), Connection, SLOT(deleteLater()));

   Connection->connectToHost(Host, Port);

   ConnectionCompletedMapper->setMapping(Connection, Connection);
   connect(Connection, SIGNAL(connected()), ConnectionCompletedMapper, SLOT(map()));
   connect(ConnectionCompletedMapper, SIGNAL(mapped(QObject*)), this, SLOT(CompleteConnection(QObject*)));
}

void
Chart::CompleteConnection(QObject* SocketObj)
{
   QAbstractSocket* Connection = dynamic_cast<QAbstractSocket*>(SocketObj);
   if (!Connection)
   {
      PANIC("Unexpected mapped socket");
   }

   ConnectionMapper->setMapping(Connection, Connection);
   connect(Connection, SIGNAL(readyRead()), ConnectionMapper, SLOT(map()));
   connect(ConnectionMapper, SIGNAL(mapped(QObject*)), this, SLOT(ReadCommand(QObject*)));
}

void
Chart::ReadCommand(QObject* SocketObj)
{
   QIODevice* Socket = dynamic_cast<QIODevice*>(SocketObj);
   if (!Socket)
   {
      PANIC("Unexpected mapped socket");
   }

   // make sure that we have a newline character available
   int Avail = Socket->bytesAvailable();
   while (Avail > 0)
   {
      std::vector<char> Buf(Avail);
      Socket->peek(&Buf[0], Avail);
      if (std::find(Buf.begin(), Buf.end(), '\n') == Buf.end())
         return;

      QByteArray Line = Socket->readLine();
      std::string LineStr(Line.data(), Line.data()+Line.size());
      TRACE(LineStr);

      std::istringstream Str(LineStr);
      std::string BDC, At;
      int Bell;
      double V;
      Str >> BDC >> Bell >> At >> V;
      TRACE(Bell)(V);

      if (Bell == 5)
	this->PlotPoint(V*V);


      Avail = Socket->bytesAvailable();
   }
}
