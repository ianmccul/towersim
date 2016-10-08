// -*- C++-*- $Id: asio_handler.h 1820 2016-09-16 13:03:37Z uqimccul $

#if !defined(ASIO_HANDLER_H)
#define ASIO_HANDLER_H

#include <boost/asio.hpp>
#include <boost/asio/generic/seq_packet_protocol.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <cstddef>

typedef boost::function<void(std::vector<char> const&)> TriggerFunc;

class PacketHandler
{
   public:
      explicit PacketHandler(boost::asio::io_service& io);

      ~PacketHandler();

      void Connect(std::string const& Addr);

      void Stop() {}

      // Calls f_(buffer)
      void AsyncRead(TriggerFunc f_);

   private:
      static int const bufsize = 100;
      char buf[100];

      boost::asio::socket_base::message_flags Flags;

      void ReadPacket(const boost::system::error_code& e,
                      int BytesTransferred,
                      TriggerFunc Trigger);

      boost::asio::generic::seq_packet_protocol::socket Socket;
};

PacketHandler::PacketHandler(boost::asio::io_service& io)
   : Socket(io)
{
}

PacketHandler::~PacketHandler()
{
}

void
PacketHandler::Connect(std::string const& Addr)
{
   struct sockaddr_un addr;
   memset(&addr, 0, sizeof(struct sockaddr_un));
   addr.sun_family = AF_LOCAL;
   std::copy(Addr.begin(), Addr.end(), addr.sun_path);
   int Len = Addr.size() + offsetof(sockaddr_un, sun_path);
   Socket.connect(boost::asio::generic::seq_packet_protocol::endpoint(static_cast<void*>(&addr), Len));
}

void PacketHandler::AsyncRead(TriggerFunc f_)
{

   Socket.async_receive(boost::asio::buffer(buf, bufsize), Flags,
                        boost::bind(&PacketHandler::ReadPacket,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred,
                                    f_));
}

void PacketHandler::ReadPacket(const boost::system::error_code& e,
                               int BytesTransferred,
                               TriggerFunc Trigger)
{
   std::vector<char> Buf(buf, buf+BytesTransferred);
   Socket.async_receive(boost::asio::buffer(buf, bufsize), Flags,
                        boost::bind(&PacketHandler::ReadPacket,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred,
                                    Trigger));
   if (BytesTransferred > 0)
      Trigger(Buf);
}


#endif
