// -*- C++ -*- $Id: sensorinputserver.h 974 2012-07-04 22:20:50Z uqimccul $

#if !defined(SENSORINPUTSERVER_H_KDJASHFCWEHUIH)
#define SENSORINPUTSERVER_H_KDJASHFCWEHUIH

#include "towersim-mainwindow.h"
#include <QObject>
#include <QTcpServer>
#include <QSignalMapper>

namespace Ui
{
   class MainWindow;
} // namespace Ui

class SensorInputServer : public QObject
{
   Q_OBJECT

   public:
      static int const DefaultSensorPort = 5700;

      SensorInputServer(TowerSimMainWindow* Window_);

      // start listening for connections on the given TCP port
      void ListenForConnections(int Port = DefaultSensorPort);

      // stop listening for connections
      void StopListening();

      // Initiates a connection to the given host:port
      // if Retry then attempt to reconnect on error
      void ConnectTo(QString const& Host, int Port = DefaultSensorPort, bool Retry = false);

      // if we call ConnectTo with Retry, then we always retry to connect.
      // this function stops attempting to connect.
      void ClearConnectionRetryList();

   private slots:
      void ReadCommand(QObject* Socket);
      void NewConnection();
      void RetryConnection(QObject* Socket);
      void Retry();
      void CompleteConnection(QObject* Socket);

   private:
      TowerSimMainWindow* Window;
      Ui::MainWindow* ui;
      QSignalMapper* ConnectionMapper;
      QSignalMapper* ConnectionCompletedMapper;
      QTcpServer* Server;

      // keep track of the sockets that we want to retry connecting
      QSignalMapper* ConnectionRetryMapper;
      std::map<QTcpSocket*, QString> RetryHostMap;
      std::map<QTcpSocket*, int> RetryPortMap;

      QTcpSocket* RetrySocket;
};

#endif
