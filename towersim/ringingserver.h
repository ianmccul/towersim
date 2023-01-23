// -*- C++ -*- $Id: ringingserver.h 987 2012-07-15 01:58:28Z uqimccul $
//
// The RingingServer class emits OutputCommand's
// and maintains some state information about
// the number of bells being rung and the
// mappings between sensors and bells.
//
// Users of this class register one or more 'sinks' with the
// RegisterCommandVisitor method.  The supplied pointer is
// to a class that has an operator() defined for each
// possible output command.

#if !defined(RINGINGSERVER_H_SDHEW75Y8479JDLIS)
#define RINGINGSERVER_H_SDHEW75Y8479JDLIS

#include "outputparser.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "posix_time_operators.h"
#include <set>
#include <map>
#include <list>
#include <ostream>

class RingingServer
{
   public:
      RingingServer();

      // Functions for the mapping between sensors and bells

      void AssociateSensor(std::string const& Sensor, int Bell);

      // removes the named sensor
      void DisassociateSensor(std::string const& Sensor);

      // removes the sensor associated to the specified bell
      void DisassociateSensor(int Bell);

      // returns the bell number associated with the sensor,
      // or 0 if not associated
      int Lookup(std::string const& Sensor) const
      {
         std::map<std::string, int>::const_iterator I = AssociatedSensors.find(Sensor);
         if (I == AssociatedSensors.end())
            return 0;
         return I->second;
      }

      // returns the sensor attached to the given bell, or empty string if no sensor attached
      std::string GetAssociatedSensor(int Bell) const;


      // Register a sink for OutputCommand's
      // The class must have an operator() for each possible OutputCommand
      // When a new sink is registered, the necessary state information is written
      // immediately.  Currently, this is only the NumberOfBells command.
      template <typename T>
      void RegisterCommandVisitor(T* Class);

      // The following members are front ends to emit OutputCommand's

      // tell the client how many bells are ringing (for choosing the sound set)
      void SetNumberOfBells(int n);

      // schedules a computer-generated bell at the given time
      void RingBell(int Bell,
                    boost::posix_time::ptime At,
                    boost::posix_time::time_duration Error);

      void RingBell(int Bell,
                    boost::posix_time::ptime At,
                    boost::posix_time::time_duration Error,
                    double FracError,
                    int Row, int Place);

      // Cancels (rings 'SILENT') an already scheduled bell (eg, because a sensor has triggered)
      void CancelBell(int Bell,
                      boost::posix_time::ptime At,
                      boost::posix_time::time_duration Error,
                      double FracError,
                      int Row, int Place);

      void RingSensor(int Bell,
                      boost::posix_time::ptime At,
                      boost::posix_time::time_duration Error,
                      double FracError,
                      std::string const& Sensor);

      void RingSensor(int Bell,
                      boost::posix_time::ptime At,
                      std::string const& Sensor);

      void RingSensor(int Bell,
                      boost::posix_time::ptime At,
                      boost::posix_time::time_duration Error,
                      double FracError,
                      std::string const& Sensor,
                      int Row, int Place);

      void Call(boost::posix_time::ptime At, std::string const& str);
      void Call(boost::posix_time::ptime At, std::string const& str1,
                std::string const& str2);
      void Call(boost::posix_time::ptime At, std::string const& str1,
                std::string const& str2, std::string const& str3);
      void Call(boost::posix_time::ptime At, std::string const& str1,
                std::string const& str2, std::string const& str3,
		std::string const& str4);

      void Call(boost::posix_time::ptime At, std::vector<std::string> const& str);

      void CallGoMethod(boost::posix_time::ptime At, std::string const& Name);

      void CallSpliceMethod(boost::posix_time::ptime At, std::string const& Name);

      void LoadCall(std::string const& str);

      void Division(int Row);

      void Finish();

      void Abort();

   private:
      class CommandDispatcherBase;

      template <typename T> friend class CommandDispatcher;

      typedef std::list<CommandDispatcherBase*> DispatcherListType;

      template <typename T>
      void Emit(T const& command);

      int NumBells;
      std::set<int> AssociatedBells;
      std::map<std::string, int> AssociatedSensors;
      std::map<int, bool> BellSensorActive; // true if the sensor is actively used
      DispatcherListType CommandSinks;
};

#include "ringingserver.cc"

#endif
