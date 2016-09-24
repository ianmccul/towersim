
#include "nRF24L01P_PTX.h"

// status
#define STATUS_UNDEFINED         0
#define STATUS_READY_INIT        1
#define STATUS_POWER_DOWN        2
#define STATUS_STARTUP_STANDBY   3
#define STATUS_STANDBY           4
#define STATUS_TRANSMITTING      5
#define STATUS_PACKET_OK         6
#define STATUS_PACKET_MAX_RT     7
#define STATUS_FINISHED_TRANSMIT 8

nRF24L01P_PTX::nRF24L01P_PTX(nRF24L01P& Device_, PinName CE_, PinName Int_)
  : Device(Device_),
    CE(CE_),
    Int(Int_),
    IntFlag(0)
{
   CE = 0;
   Status = STATUS_UNDEFINED;
   Int.mode(PullNone);
   Int.fall(NULL);
   InitializeTimer.attach_us(this, &nRF24L01P_PTX::ReadyInitialize, Tundef2pd_us);
}

void
nRF24L01P_PTX::Initialize()
{
   while (Status == STATUS_UNDEFINED)
   {
      wait_us(1);
   }
   PowerOnTimer.detach();
   CE = 0;
   Device.reset();
   Device.set_ptx_mode();
   Status = STATUS_POWER_DOWN;
   Int.fall(NULL);
}

void
nRF24L01P_PTX::SetChannel(int Channel)
{
   Device.set_channel(Channel);
}

void
nRF24L01P_PTX::SetDataRate(int Rate)
{
   Device.set_air_data_rate(Rate);
}

void
nRF24L01P_PTX::SetTransmitPower(int Power)
{
   Device.set_tx_power(Power);
}

void
nRF24L01P_PTX::SetDestinationAddress(uint64_t Address)
{
   Device.set_tx_address(Address);
}

void
nRF24L01P_PTX::PowerUp()
{
   if (Status != STATUS_POWER_DOWN)
   {
      error("nRF24L01P_PTX::PowerUp(): can only be called when device is powered down");
   }
   Status = STATUS_STARTUP_STANDBY;
   Device.set_power_up();
   PowerOnTimer.attach_us(this, &nRF24L01P_PTX::ReadyStandby, Tpd2stby_us);
}

void
nRF24L01P_PTX::PowerDown()
{
   if (Status == STATUS_UNDEFINED || Status == STATUS_READY_INIT)
   {
      error("nRF24L01P_PTX::PowerDown(): error: device is not initialized!");
   }
   // Although it is technically possible to turn the power off at any time,
   // if we're currently processing a packet, wait until we've finished
   while (Status == STATUS_TRANSMITTING || Status == STATUS_PACKET_OK || Status == STATUS_PACKET_MAX_RT)
   {
      wait_us(1);
   }
   PowerOnTimer.detach();
   Device.set_power_down();
   Status = STATUS_POWER_DOWN;
}

void
nRF24L01P_PTX::EnableDynamicPayload(int Pipe, bool Enable)
{
   Device.enable_dynamic_payload(Pipe, Enable);
}

bool
nRF24L01P_PTX::IsReadyTransmit()
{
   return (Status == STATUS_STANDBY);
}

int
nRF24L01P_PTX::TransmitPacket(char* Buf, int Size)
{
   // If the device isn't powered up then there is a logic error
   if (Status == STATUS_UNDEFINED || Status == STATUS_READY_INIT || Status == STATUS_POWER_DOWN)
   {
      error("nRF24L01P_PTX::TransmitPacket(): error: device power must be on!");
   }

   // if we're not ready to send, then bug out
   if (Status != STATUS_STANDBY)
   {
      return -1;
   }

   Device.write_tx_payload(Buf, Size);
   Status = STATUS_TRANSMITTING;
   CE = 1;
   wait_us(Thce_us);
   //CE = 0;
   // wait until the packet is transmitted (or some error)
   while (Status == STATUS_TRANSMITTING)
   {
      wait_us(1);
      if (Device.is_tx_sent())
      {
         // we successfully sent a packet
         Status = STATUS_PACKET_OK;
         Device.clear_tx_sent();
      }
      else if (Device.is_max_rt())
      {
         // we failed to send the packet
         Status = STATUS_PACKET_MAX_RT;
         Device.clear_max_rt();
      }
   }
   CE=0;
   if (Status == STATUS_PACKET_OK)
   {
      Status = STATUS_STANDBY;
      return 0;
   }
   else if (Status == STATUS_PACKET_MAX_RT)
   {
      Status = STATUS_STANDBY;
      Device.flush_tx_fifo();
      return -1;
   }
   // else the Status isn't what we expected, which
   // shouldn't happen in blocking mode.  Some interrupt must have
   // reset Status.  Bug out with an error.
   return -1;
}

int
nRF24L01P_PTX::TransmitPacketNB(char* Buf, int Size)
{
   // If the device isn't powered up then there is a logic error
   if (Status == STATUS_UNDEFINED || Status == STATUS_READY_INIT || Status == STATUS_POWER_DOWN)
   {
      error("nRF24L01P_PTX::TransmitPacketNonBlocking(): error: device power must be on!");
   }

   // if we're not ready to send, then bug out
   if (Status != STATUS_STANDBY)
   {
      return -1;
   }

   // Set up the interrupt handler
   Status = STATUS_TRANSMITTING;
   Int.fall(this, &nRF24L01P_PTX::IntHandler);

   // write the payload
   Device.write_tx_payload(Buf, Size);
   CE = 1;
   wait_us(Thce_us);
   CE = 0;

   return 0;
}

void nRF24L01P_PTX::EnableStreamMode()
{
   Device.set_interrupt_tx(false);
   Int.fall(this, &nRF24L01P_PTX::IntHandlerStream);
   Status = STATUS_TRANSMITTING;
   CE = 1;
}

int
nRF24L01P_PTX::StreamPacket(char* Buf, int Size)
{
   // If the device isn't powered up then there is a logic error
   if (Status == STATUS_UNDEFINED || Status == STATUS_READY_INIT || Status == STATUS_POWER_DOWN)
   {
      error("nRF24L01P_PTX::StreamPacket(): error: device power must be on!");
   }

   while (IntFlag)
   {
      Int.disable_irq();
      IntFlag = 0;
      if (Device.is_max_rt())
      {
         // we failed to send the packet
         Device.clear_max_rt();
         Device.flush_tx_fifo();
      }
      Int.enable_irq();
   }

   // quit if we can't queue the packet immediately
   if (Device.is_tx_full())
      return -1;

   // write the payload
   Device.write_tx_payload(Buf, Size);

   return 0;
}

bool
nRF24L01P_PTX::IsTransmitFinished()
{
   return Status == STATUS_FINISHED_TRANSMIT;
}

int
nRF24L01P_PTX::TransmitWait()
{
   // wait until the packet is transmitted (or some error)
   while (Status == STATUS_TRANSMITTING)
   {
      wait_us(1);
   }

   if (Status != STATUS_FINISHED_TRANSMIT)
   {
      // we weren't waiting on a packet
      return 1;
   }

   Int.fall(NULL);

   if (Device.is_tx_sent())
   {
      // we successfully sent a packet
      Device.clear_tx_sent();
      Status = STATUS_STANDBY;
      return 0;
   }
   else if (Device.is_max_rt())
   {
      // we failed to send the packet
      Device.clear_max_rt();
      Device.flush_tx_fifo();
      Status = STATUS_STANDBY;
      return -1;
   }

   // this shouldn't happen
   return -2;
}

void
nRF24L01P_PTX::IntHandler()
{
   //   Int.fall(NULL);
   Status = STATUS_FINISHED_TRANSMIT;
}

void
nRF24L01P_PTX::IntHandlerStream()
{
   //   Int.fall(NULL);
   IntFlag = 1;
}


void
nRF24L01P_PTX::ReadyInitialize()
{
   Status = STATUS_READY_INIT;
}

void
nRF24L01P_PTX::ReadyStandby()
{
   if (Status == STATUS_STARTUP_STANDBY)
      Status = STATUS_STANDBY;
}
