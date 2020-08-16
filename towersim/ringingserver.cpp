// -*- C++ -*- $Id: ringingserver.cpp 987 2012-07-15 01:58:28Z uqimccul $

#include "ringingserver.h"
#include "outputparser.h"

RingingServer::RingingServer()
   : NumBells(8)
{
}

void 
RingingServer::AssociateSensor(std::string const& Sensor, int Bell)
{
   this->DisassociateSensor(Bell);
   this->DisassociateSensor(Sensor);
   AssociatedBells.insert(Bell);
   AssociatedSensors[Sensor] = Bell;
}

void
RingingServer::DisassociateSensor(std::string const& Sensor)
{
   int Bell = AssociatedSensors[Sensor];
   AssociatedBells.erase(Bell);
   AssociatedSensors.erase(Sensor);
}

void
RingingServer::DisassociateSensor(int Bell)
{
   std::map<std::string, int>::iterator I = AssociatedSensors.begin();
   while (I != AssociatedSensors.end() && I->second != Bell)
      ++I;

   if (I != AssociatedSensors.end())
   {
      AssociatedSensors.erase(I);
   }
   AssociatedBells.erase(Bell);
}

std::string
RingingServer::GetAssociatedSensor(int Bell) const
{
   for (std::map<std::string, int>::const_iterator I = AssociatedSensors.begin(); 
        I != AssociatedSensors.end(); ++I)
   {
      if (I->second == Bell)
         return I->first;
   }
   return std::string();
}

// This helper is only called from this file so no need to include it in the header
template <typename T>
void
RingingServer::Emit(T const& command)
{
   for (DispatcherListType::const_iterator I = CommandSinks.begin(); I != CommandSinks.end(); ++I)
   {
      (*I)->operator()(command);
   }
}


void
RingingServer::LoadCall(std::string const& str)
{
   this->Emit(OutCom::LoadCall(str));
}

void
RingingServer::Call(boost::posix_time::ptime At, std::string const& str)
{
   this->Emit(OutCom::Call(At, str));
}

void
RingingServer::Call(boost::posix_time::ptime At, std::string const& str1,
                std::string const& str2)
{
   this->Emit(OutCom::Call(At, str1, str2));
}

void
RingingServer::Call(boost::posix_time::ptime At, std::string const& str1,
                std::string const& str2, std::string const& str3)
{
   this->Emit(OutCom::Call(At, str1, str2, str3));
}

void
RingingServer::Call(boost::posix_time::ptime At, std::string const& str1,
                std::string const& str2, std::string const& str3, std::string const& str4)
{
   this->Emit(OutCom::Call(At, str1, str2, str3, str4));
}

void
RingingServer::Call(boost::posix_time::ptime At, std::vector<std::string> const& str)
{
   this->Emit(OutCom::Call(At, str));
}

void 
RingingServer::CallGoMethod(boost::posix_time::ptime At, std::string const& Name)
{
   this->Emit(OutCom::CallGoMethod(At, Name));
}

void 
RingingServer::CallSpliceMethod(boost::posix_time::ptime At, std::string const& Name)
{
   this->Emit(OutCom::CallSpliceMethod(At, Name));
}

void
RingingServer::Division(int Row)
{
   this->Emit(OutCom::Division(Row));
}

void
RingingServer::SetNumberOfBells(int n)
{
   this->Emit(OutCom::NumberOfBells(n));
}


void
RingingServer::Finish()
{
   this->Emit(OutCom::Finish());
}

void
RingingServer::Abort()
{
   this->Emit(OutCom::Abort());
}

void
RingingServer::RingBell(int Bell, 
                        boost::posix_time::ptime At, 
                        boost::posix_time::time_duration Error,
                        double FracError,
                        int Row, int Place)
{
   this->Emit(OutCom::RingBell(Bell, At, Error, FracError, Row, Place));
}

void
RingingServer::CancelBell(int Bell, 
                          boost::posix_time::ptime At, 
                          boost::posix_time::time_duration Error,
                          double FracError,
                          int Row, int Place)
{
   this->Emit(OutCom::RingBell(Bell, At, Error, FracError, Row, Place, true));
}

void
RingingServer::RingSensor(int Bell, 
                          boost::posix_time::ptime At, 
                          std::string const& Sensor)
{
   this->Emit(OutCom::RingBell(Bell, At, Sensor));
}

void
RingingServer::RingSensor(int Bell, 
                          boost::posix_time::ptime At, 
                          boost::posix_time::time_duration Error,
                          double FracError,
                          std::string const& Sensor)
{
   this->Emit(OutCom::RingBell(Bell, At, Error, FracError, Sensor));
}

void
RingingServer::RingSensor(int Bell, 
                          boost::posix_time::ptime At, 
                          boost::posix_time::time_duration Error,
                          double FracError,
                          std::string const& Sensor,
                          int Row, int Place)
{
   this->Emit(OutCom::RingBell(Bell, At, Error, FracError, Row, Place, Sensor));
}

