// -*- C++ -*- $Id: sensortcpserver.h 1795 2016-09-14 16:34:25Z uqimccul $
//
// TCP server for sensors
//

#if !defined(SENSORTCPSERVER_H_HCDSJEWYR89)
#define SENSORTCPSERVER_H_HCDSJEWYR89

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <set>

class SensorTCPServer
{
   public:
      SensorTCPServer(boost::asio::io_service& io_, std::string const& Address, std::string const& Port);

      void Attach(std::string const& Sensor);

      // attach a sensor, and specify the handstroke and backstroke corrections in milliseconds
      void Attach(std::string const& Sensor, 
                  int HandCorrection_,
                  int BackCorrection_);

      void TriggerSensor(std::string const& Sensor, boost::posix_time::ptime At);

      // Handle a request to stop the server.
      void Stop();

   private:
      typedef boost::asio::ip::tcp::socket SocketType;
      typedef boost::shared_ptr<SocketType> SocketPtr;

      SensorTCPServer(SensorTCPServer const&); // not implemented

      // initiate an async_accept for a new connection
      void StartAccept();

      // initiate an async accept operation
      void HandleAccept(SocketPtr Socket, const boost::system::error_code& e);

      // handle an async write request
      void HandleWrite(SocketPtr Socket, boost::system::error_code const& e, std::size_t Bytes);

      boost::asio::io_service& io;

      // Acceptor used to listen for incoming connections
      boost::asio::ip::tcp::acceptor Acceptor;

      // the list of connected sockets
      std::set<SocketPtr> Connections;

      // The list of attached sensors (keep track of these so we can notify new connections)
      std::set<std::string> AttachedSensors;
      std::map<std::string, int> HandCorrection;
      std::map<std::string, int> BackCorrection;
};

#endif
