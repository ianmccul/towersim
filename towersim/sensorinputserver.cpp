// -*- C++ -*- $Id: sensorinputserver.cpp 987 2012-07-15 01:58:28Z uqimccul $

// TODO: keep track of which connections have attached a sensor,
// so that we can detach them cleanly if the connection is closed.
// or do we want to deliberately avoid that?  The connection may be reestablished later...

#include "sensorinputserver.h"
#include "sensorparser.h"
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>

SensorInputServer::SensorInputServer(TowerSimMainWindow* Window_)
   : QObject(Window_),
     Window(Window_),
     ConnectionMapper(new QSignalMapper(this)),
     ConnectionCompletedMapper(new QSignalMapper(this)),
     Server(new QTcpServer(this)),
     ConnectionRetryMapper(new QSignalMapper(this))
{
}

void
SensorInputServer::ListenForConnections(int Port)
{
   // if we're already listening, then
   if (Server->isListening())
   { 
      // if we're listening on the same port then we don't need to do anything.
      if (Server->serverPort() == Port)
         return;

      // otherwise, close the server so we can listen on a different port
      disconnect(Server, SIGNAL(newConnection()),  this, SLOT(NewConnection()));

      Server->close();
   }

   if (!Server->listen(QHostAddress::Any, Port))
   {
      QMessageBox::critical(Window, tr("Bellringing Simulator"),
                               tr("Unable to listen for sensor connections: %1.")
                            .arg(Server->errorString()));
      return;
   }
   connect(Server, SIGNAL(newConnection()), this, SLOT(NewConnection()));
}

void
SensorInputServer::StopListening()
{
   if (Server->isListening())
   { 
      disconnect(Server, SIGNAL(newConnection()),  this, SLOT(NewConnection()));
      Server->close();
   }
}  

void
SensorInputServer::NewConnection()
{
   QTcpSocket* Connection = Server->nextPendingConnection();
   connect(Connection, SIGNAL(disconnected()), Connection, SLOT(deleteLater()));

   ConnectionMapper->setMapping(Connection, Connection);
   connect(Connection, SIGNAL(readyRead()), ConnectionMapper, SLOT(map()));
   connect(ConnectionMapper, SIGNAL(mapped(QObject*)), this, SLOT(ReadCommand(QObject*)));
   connect(Connection, SIGNAL(disconnected()), Connection, SLOT(deleteLater()));
}

void
SensorInputServer::ConnectTo(QString const& Host, int Port, bool Retry)
{
   QTcpSocket* Connection = new QTcpSocket(this);
   if (Retry)
   {
      ConnectionRetryMapper->setMapping(Connection, Connection);
      connect(Connection, SIGNAL(error(QAbstractSocket::SocketError)), ConnectionRetryMapper, SLOT(map()));
      connect(Connection, SIGNAL(disconnected()), ConnectionRetryMapper, SLOT(map()));
      connect(ConnectionRetryMapper, SIGNAL(mapped(QObject*)), this, SLOT(RetryConnection(QObject*)));
      RetryHostMap[Connection] = Host;
      RetryPortMap[Connection] = Port;
   }
   else
   {
      connect(Connection, SIGNAL(error(QAbstractSocket::SocketError)), Connection, SLOT(deleteLater()));
      connect(Connection, SIGNAL(disconnected()), Connection, SLOT(deleteLater()));
   }

   Connection->connectToHost(Host, Port);

   ConnectionCompletedMapper->setMapping(Connection, Connection);
   connect(Connection, SIGNAL(connected()), ConnectionCompletedMapper, SLOT(map()));
   connect(ConnectionCompletedMapper, SIGNAL(mapped(QObject*)), this, SLOT(CompleteConnection(QObject*)));
}

void
SensorInputServer::RetryConnection(QObject* Socket)
{
   RetrySocket = dynamic_cast<QTcpSocket*>(Socket);
   // Wait some seconds before retrying
   QTimer::singleShot(1000, this, SLOT(Retry()));
}

void
SensorInputServer::Retry()
{
   RetrySocket->connectToHost(RetryHostMap[RetrySocket], RetryPortMap[RetrySocket]);
}

void
SensorInputServer::ClearConnectionRetryList()
{
   for (std::map<QTcpSocket*, QString>::const_iterator I = RetryHostMap.begin(); I != RetryHostMap.end(); ++I)
   {
      QTcpSocket* Connection = I->first;
      disconnect(Connection, SIGNAL(error(QAbstractSocket::SocketError)), ConnectionRetryMapper, SLOT(map()));
      disconnect(ConnectionRetryMapper, SIGNAL(mapped(QObject*)), this, SLOT(RetryConnection(QObject*)));
      connect(Connection, SIGNAL(error(QAbstractSocket::SocketError)), Connection, SLOT(deleteLater()));
   }
   RetryHostMap.clear();
   RetryPortMap.clear();
}

void
SensorInputServer::CompleteConnection(QObject* SocketObj)
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

class ApplyCommand : public boost::static_visitor<>
{
   public:
      explicit ApplyCommand(TowerSimMainWindow* Window_)
         : Window(Window_) {}

      void operator()(AttachSensor const& a) const
      {
         TRACE("Attach")(a.Sensor)(a.HandAdjust)(a.BackAdjust);
         Window->AttachSensor(a);
      }

      void operator()(DetachSensor const& a) const
      {
         Window->DetachSensor(a.Sensor);
      }

      void operator()(TriggerSensor const& t) const
      {
	 TRACE("Trigger")(t.Sensor);
         Window->GetScheduler()->RingSensor(t.Sensor, t.At);
         Window->UpdateSensorStatistics();
      }

      void operator()(CommandError const& e) const
      {
         std::cerr << "error: " << e.What << '\n';
      }

   private:
      TowerSimMainWindow* Window;
};

void
SensorInputServer::ReadCommand(QObject* SocketObj)
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
      SensorInputCommand Command = ParseSensorCommand(LineStr);
      boost::apply_visitor(ApplyCommand(Window), Command);
      Avail = Socket->bytesAvailable();
   }
}
