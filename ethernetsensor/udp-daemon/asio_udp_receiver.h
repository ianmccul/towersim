// -*- C++-*- $Id: asio_handler.h 1820 2016-09-16 13:03:37Z uqimccul $

#if !defined(ASIO_UDP_RECEIVER_H)
#define ASIO_UDP_RECEIVER_H

#include <boost/asio.hpp>
#include <boost/asio/generic/seq_packet_protocol.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <cstddef>

typedef boost::function<void(std::vector<char> const&)> TriggerFunc;

class UDPReceiver
{
   public:
      explicit UDPReceiver(boost::asio::io_context& io, int Port);

      ~UDPReceiver();

      // Calls f_(buffer)
      void AsyncRead(TriggerFunc f_);

   private:
      static int const bufsize = 100;
      char buf[100];

      boost::asio::socket_base::message_flags Flags;

      void ReadPacket(const boost::system::error_code& e,
                      int BytesTransferred,
                      TriggerFunc Trigger);

      boost::asio::ip::udp::socket Socket;
};

UDPReceiver::UDPReceiver(boost::asio::io_context& io, int Port)
   : Socket(io)
{
   boost::asio::ip::udp::endpoint Receiver(boost::asio::ip::udp::v4(), Port);
   Socket.open(Receiver.protocol());
   Socket.bind(Receiver);
}

UDPReceiver::~UDPReceiver()
{
}

void UDPReceiver::AsyncRead(TriggerFunc f_)
{

   Socket.async_receive(boost::asio::buffer(buf, bufsize), Flags,
                        boost::bind(&UDPReceiver::ReadPacket,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred,
                                    f_));
}

void UDPReceiver::ReadPacket(const boost::system::error_code& e,
                               int BytesTransferred,
                               TriggerFunc Trigger)
{
   // Copy the buffer
   std::vector<char> Buf(buf, buf+BytesTransferred);
   // Process the data (if we got any)
   if (BytesTransferred > 0)
      Trigger(Buf);

   // Schedule another async receive
   Socket.async_receive(boost::asio::buffer(buf, bufsize), Flags,
                        boost::bind(&UDPReceiver::ReadPacket,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred,
                                    Trigger));
}

#endif
