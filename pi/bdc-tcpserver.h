// -*- C++ -*- $Id: sensortcpserver.h 1795 2016-09-14 16:34:25Z uqimccul $
//
// TCP server for sensors
//

#if !defined(BDC_TCPSERVER_H_HCDSJEWYR89)
#define BDC_TCPSERVER_H_HCDSJEWYR89

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <set>

class BDC_TCPServer
{
   public:
      BDC_TCPServer(boost::asio::io_service& io_, std::string const& Address, std::string const& Port);

      void TriggerBDC(int Bell, boost::posix_time::ptime At, double Velocity);

      // Handle a request to stop the server.
      void Stop();

   private:
      typedef boost::asio::ip::tcp::socket SocketType;
      typedef boost::shared_ptr<SocketType> SocketPtr;

      BDC_TCPServer(BDC_TCPServer const&); // not implemented

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
};

#endif
