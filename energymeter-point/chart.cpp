// -*- C++ -*-

#include "chart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpSocket>
#include <QtDBus/QtDBus>
#include "common/trace.h"

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    handPoints(0),
    backPoints(0),
    hthresh(0),
    bthresh(0),
    HEnergy(465*465),
    BEnergy(469*469),
    m_axis(new QValueAxis),
    m_step(0),
    m_x(3.5),
    m_y(1),
    ConnectionMapper(new QSignalMapper(this)),
    ConnectionCompletedMapper(new QSignalMapper(this)),
    Server(new QTcpServer(this)),
    yMax(475.0*475.0),
    yMin(-1.0),
    WatchBell(5)
{
    qsrand((uint) QTime::currentTime().msec());

    // Timer to prevent screen saver
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleScreensaver()));
    screenTimer.setInterval(10000);

    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    m_timer.setInterval(10);

    handPoints = new QScatterSeries(this);
    handPoints->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    handPoints->setMarkerSize(30.0);
    QBrush b(Qt::red);
    QPen p(Qt::red);
    handPoints->setPen(p);
    handPoints->setBrush(b);
    this->addSeries(handPoints);

    backPoints = new QScatterSeries(this);
    p = QPen(Qt::blue);
    b = QBrush(Qt::blue);
    backPoints->setPen(p);
    backPoints->setBrush(b);
    backPoints->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    backPoints->setMarkerSize(30.0);
    this->addSeries(backPoints);

    QPen HPen(Qt::red);
    HPen.setWidth(6);
    hthresh = new QLineSeries();
    hthresh->setPen(HPen);
    hthresh->append(0, HEnergy);
    hthresh->append(5, HEnergy);
    this->addSeries(hthresh);

    QPen BPen(Qt::blue);
    BPen.setWidth(6);
    bthresh = new QLineSeries();
    bthresh->setPen(BPen);
    bthresh->append(0, BEnergy);
    bthresh->append(5, BEnergy);
    this->addSeries(bthresh);

    createDefaultAxes();
    QPen GridLine(Qt::black);
    GridLine.setWidth(4);
    m_axis->setGridLinePen(GridLine);
    setAxisX(m_axis);
    m_axis->setTickCount(11);
    axisX()->setRange(0, 10);
    handPoints->attachAxis(m_axis);
    backPoints->attachAxis(m_axis);

    // bell 5
    this->SetBell(5);
    axisY()->setRange(460*460, 475*475);
    axisY()->setGridLinePen(GridLine);

    m_timer.start();

}

Chart::~Chart()
{
}

void
Chart::handleTimeout()
{
   double const f = 0.85;
   qreal xMiddle = f*m_axis->max() + (1-f)*m_axis->min();
   if (m_x > xMiddle)
   {
     double Scale = (this->plotArea().right() - this->plotArea().left()) /
       (m_axis->max() - m_axis->min());
     this->scroll(std::min(10.0, Scale*(m_x-xMiddle)), 0);
   }
}

void
Chart::handleScreensaver()
{
   // Stop the screen saver from kicking in
   // system("qdbus org.freedesktop.ScreenSaver /ScreenSaver SimulateUserActivity");
   QDBusInterface dbus_iface("org.freedesktop.ScreenSaver", "/ScreenSaver",
                             "org.freedesktop.ScreenSaver");
   dbus_iface.call("SimulateUserActivity");
}

void
Chart::SetHandstroke()
{
  HEnergy = m_y;
  hthresh->clear();
  hthresh->append(0, HEnergy);
  hthresh->append(this->plotArea().right(), HEnergy);
}

void
Chart::SetBackstroke()
{
  BEnergy = m_y;
  bthresh->clear();
  bthresh->append(0, BEnergy);
  bthresh->append(this->plotArea().right(), BEnergy);
}

void
Chart::SetBell(int B)
{
   WatchBell = B;
   this->setTitle(("Energy Meter Bell " + std::to_string(B)).c_str());
}

void
Chart::Reset()
{
   handPoints->clear();
   backPoints->clear();
   m_x = 3;
   axisX()->setRange(0, 10);

   hthresh->clear();
   hthresh->append(0, HEnergy);
   hthresh->append(5, HEnergy);

   bthresh->clear();
   bthresh->append(0, BEnergy);
   bthresh->append(5, BEnergy);

   yMin = m_y;
   yMax = m_y;
   this->NarrowView();
}

void
Chart::ZeroView()
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
   this->setAnimationOptions(QChart::NoAnimation);
}

void
Chart::PlotPoint(double v)
{
   double E = v*v;
   yMax = std::max(E, yMax);
   if (yMin == -1.0)
      yMin = E;
   else
      yMin = std::min(E, yMin);

   m_x += 1;
   m_y = E;

   if (v < 0)
   {
     backPoints->append(m_x, m_y);
   }
   else
   {
     handPoints->append(m_x, m_y);
   }

   hthresh->append(plotArea().right(), HEnergy);
   bthresh->append(plotArea().right(), BEnergy);
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

      if (Bell == WatchBell)
	this->PlotPoint(V);


      Avail = Socket->bytesAvailable();
   }
}
