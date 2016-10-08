// -*- C++ -*- $Id: sensortcpserver.cpp 1888 2016-10-03 03:16:06Z uqimccul $

#include <boost/date_time/posix_time/posix_time.hpp>
#include "sensortcpserver.h"
#include <sstream>
#include <iostream>
#include <iomanip>

SensorTCPServer::SensorTCPServer(boost::asio::io_service& io_, std::string const& Address, std::string const& Port)
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

void SensorTCPServer::Stop()
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

void SensorTCPServer::StartAccept()
{
   SocketPtr Socket = SocketPtr(new boost::asio::ip::tcp::socket(io));
   Acceptor.async_accept(*Socket,
                         boost::bind(&SensorTCPServer::HandleAccept, this, Socket,
                                     boost::asio::placeholders::error));
}

void SensorTCPServer::HandleAccept(SocketPtr Socket, boost::system::error_code const& e)
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
     // send the attached sensors
     for (std::set<std::string>::const_iterator I = AttachedSensors.begin(); I != AttachedSensors.end(); ++I)
     {
        std::ostringstream Msg;
        Msg << "ATTACH \""
            << (*I)
            << "\"";
        if (HandCorrection.find(*I) != HandCorrection.end())
        {
           Msg << " ADJUST HAND " << HandCorrection[*I] << " BACK " << BackCorrection[*I];
        }
        Msg << '\n';
        std::string Str = Msg.str();
        boost::asio::async_write(*Socket, boost::asio::buffer(Str.data(), Str.size()),
                                 boost::bind(&SensorTCPServer::HandleWrite, this, Socket,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
     }
  }

  this->StartAccept();
}

void
SensorTCPServer::HandleWrite(SocketPtr Socket, boost::system::error_code const& e, std::size_t Bytes)
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
SensorTCPServer::Attach(std::string const& Sensor)
{
   // ignore the request if the sensor is already attached
   if (AttachedSensors.find(Sensor) != AttachedSensors.end())
      return;

   AttachedSensors.insert(Sensor);

   std::ostringstream Msg;
   Msg << "ATTACH \""
       << Sensor
       << "\"\n";
   std::string Str = Msg.str();
   for (std::set<SocketPtr>::const_iterator I = Connections.begin(); I != Connections.end(); ++I)
   {
      boost::asio::async_write(**I, boost::asio::buffer(Str.data(), Str.size()),
                               boost::bind(&SensorTCPServer::HandleWrite, this, *I,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
   }
}

void
SensorTCPServer::Attach(std::string const& Sensor,
               int HandCorrection_,
               int BackCorrection_)
{
   // ignore the request if the sensor is already attached
   if (AttachedSensors.find(Sensor) != AttachedSensors.end() &&
       HandCorrection.find(Sensor) != HandCorrection.end() &&
       HandCorrection[Sensor] == HandCorrection_ &&
       BackCorrection.find(Sensor) != BackCorrection.end() &&
       BackCorrection[Sensor] == BackCorrection_)
      return;

   AttachedSensors.insert(Sensor);
   HandCorrection[Sensor] = HandCorrection_;
   BackCorrection[Sensor] = BackCorrection_;

   std::ostringstream Msg;
   Msg << "ATTACH \""
       << Sensor << '\"'
       << " ADJUST HAND "
       << HandCorrection_
       << " BACK "
       << BackCorrection_
       << "\n";
   std::string Str = Msg.str();
   for (std::set<SocketPtr>::const_iterator I = Connections.begin(); I != Connections.end(); ++I)
   {
      boost::asio::async_write(**I, boost::asio::buffer(Str.data(), Str.size()),
                               boost::bind(&SensorTCPServer::HandleWrite, this, *I,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
   }
}

void
SensorTCPServer::TriggerSensor(std::string const& Sensor,
                        boost::posix_time::ptime At)
{
   std::ostringstream Msg;
   Msg << "SENSOR \""
       << Sensor
       << "\" "
       << to_iso_string(At)
       << '\n';
   std::string Str = Msg.str();
   for (std::set<SocketPtr>::const_iterator I = Connections.begin(); I != Connections.end(); ++I)
   {
      boost::asio::async_write(**I, boost::asio::buffer(Str.data(), Str.size()),
			       boost::bind(&SensorTCPServer::HandleWrite, this, *I,
					   boost::asio::placeholders::error,
					   boost::asio::placeholders::bytes_transferred));
   }
}
