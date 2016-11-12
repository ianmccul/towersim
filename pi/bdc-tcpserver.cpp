// -*- C++ -*- $Id: sensortcpserver.cpp 1888 2016-10-03 03:16:06Z uqimccul $

#include <boost/date_time/posix_time/posix_time.hpp>
#include "bdc-tcpserver.h"
#include <sstream>
#include <iostream>
#include <iomanip>

BDC_TCPServer::BDC_TCPServer(boost::asio::io_service& io_, std::string const& Address, std::string const& Port)
   : io(io_),
     Acceptor(io)
{

// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io);
  boost::asio::ip::tcp::resolver::query query(Address, Port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  Acceptor.open(endpoint.protocol());
  Acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  Acceptor.bind(endpoint);
  Acceptor.listen();

  this->StartAccept();
}

void BDC_TCPServer::Stop()
{
   // The server is stopped by cancelling all outstanding asynchronous
   // operations. Once all operations have finished the io_service::run() call
   // will exit.
   Acceptor.close();
   while (!Connections.empty())
   {
      boost::system::error_code error;
      (*Connections.begin())->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
      (*Connections.begin())->close(error);
      Connections.erase(Connections.begin());
   }
}

void BDC_TCPServer::StartAccept()
{
   SocketPtr Socket = SocketPtr(new boost::asio::ip::tcp::socket(io));
   Acceptor.async_accept(*Socket,
                         boost::bind(&BDC_TCPServer::HandleAccept, this, Socket,
                                     boost::asio::placeholders::error));
}

void BDC_TCPServer::HandleAccept(SocketPtr Socket, boost::system::error_code const& e)
{
  // Check whether the server was stopped by a signal before this completion
  // handler had a chance to run.
  if (!Acceptor.is_open())
  {
    return;
  }

  if (!e)
  {
     std::cerr << "Accepted connection from " << Socket->remote_endpoint() << '\n';
     Connections.insert(Socket);
  }

  this->StartAccept();
}

void
BDC_TCPServer::HandleWrite(SocketPtr Socket, boost::system::error_code const& e, std::size_t Bytes)
{
   if (e)
   {
      // graceful shutdown - avoid calling remote_endpoint(), it will throw if the
      // connection doesn't exist!
      boost::system::error_code error;
      Socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
      Connections.erase(Socket);
   }
}

void
BDC_TCPServer::TriggerBDC(int Bell, boost::posix_time::ptime At, double Velocity)
{
   std::ostringstream Msg;
   Msg << "BDC " << Bell << " "
       << to_iso_string(At) << " "
       << std::setprecision(6) << Velocity
       << '\n';
   std::string Str = Msg.str();
   for (std::set<SocketPtr>::const_iterator I = Connections.begin(); I != Connections.end(); ++I)
   {
      boost::asio::async_write(**I, boost::asio::buffer(Str.data(), Str.size()),
			       boost::bind(&BDC_TCPServer::HandleWrite, this, *I,
					   boost::asio::placeholders::error,
					   boost::asio::placeholders::bytes_transferred));
   }
}
