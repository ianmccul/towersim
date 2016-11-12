// -*- C++ -*-

#include "chart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpSocket>
#include "common/trace.h"

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    m_series(0),
    m_axis(new QValueAxis),
    m_step(0),
    m_x(3),
    m_y(1),
    ConnectionMapper(new QSignalMapper(this)),
    ConnectionCompletedMapper(new QSignalMapper(this)),
    Server(new QTcpServer(this))
{
    qsrand((uint) QTime::currentTime().msec());

    //QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    //m_timer.setInterval(1000);

    m_series = new QSplineSeries(this);
    QPen green(Qt::red);
    green.setWidth(3);
    m_series->setPen(green);
    m_series->append(m_x, m_y);

    addSeries(m_series);
    createDefaultAxes();
    setAxisX(m_axis, m_series);
    m_axis->setTickCount(5);
    axisX()->setRange(0, 5);
    axisY()->setRange(0, 500*500);

    m_timer.start();
}

Chart::~Chart()
{

}

void Chart::handleTimeout()
{
   qreal x = 1; // plotArea().width() / m_axis->tickCount();
   qreal y = (m_axis->max() - m_axis->min()) / m_axis->tickCount();
   m_x += y;
   m_y = qrand() % 5 - 2.5;
   m_series->append(m_x, m_y);
   scroll(x, 0);
   if (m_x == 100)
      m_timer.stop();
}

void Chart::PlotPoint(double v)
{
   qreal x = plotArea().width() / m_axis->tickCount();
   TRACE(x);
   qreal y = (m_axis->max() - m_axis->min()) / m_axis->tickCount();
   TRACE(y);
   m_x += y;
   m_y = v;
   m_series->append(m_x, m_y);
   scroll(x, 0);
}

void Chart::ConnectTo(QString const& Host, int Port)
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
      TRACE(V);

      this->PlotPoint(V*V);


      Avail = Socket->bytesAvailable();
   }
}
