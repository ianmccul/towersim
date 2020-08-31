#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "posix_time_operators.h"
#include "ringingserver.h"
#include "ringingscheduler.h"
//#include "readsensor.h"

double const DefaultChangesPerMinute = 28;

int const RestartAfterSkip = 4;  // restart sounding the bell after this many changes of no sensor

std::vector<int> LastRowOfSensorInput(16,-1000);

RingingScheduler::RingingScheduler(boost::shared_ptr<RingingServer> Server_)
   : Server(Server_),
     NumBells(8),
     HandstrokeGap(1.0),
     HandstrokeError(0.0),
     BackstrokeError(0.0),
     ErrorFromSensorMultiplier(0.0),
     Running(false),
     GaussianGen(Rng, GaussianDistributionType(0.0, 1.0)),
     UniformGen(Rng, UniformDistributionType(-1.0, 1.0))
{
}

boost::posix_time::time_duration
RingingScheduler::Gap() const
{
   double gap = 60.0 / ((NumBells + 0.5*HandstrokeGap) * ChangesPerMinute);
   return boost::posix_time::microseconds(long(gap*1e6));
}

boost::posix_time::time_duration
RingingScheduler::Gap(int NBells) const
{
   double gap = 60.0 / ((NBells + 0.5*HandstrokeGap) * ChangesPerMinute);
   return boost::posix_time::microseconds(long(gap*1E6));
}

boost::posix_time::time_duration
RingingScheduler::RowDuration() const
{
   double RowGap = 60.0 / ChangesPerMinute;
   return boost::posix_time::microseconds(long(RowGap*1E6));
}

boost::posix_time::time_duration
RingingScheduler::GapOfRow(int Row) const
{
   CHECK(Row >= 0 && Row < int(RowGap.size()))(Row);
   return RowGap[Row];
}


void
RingingScheduler::SetChangesPerMinute(double cpm)
{
   ChangesPerMinute = cpm;
}

void
RingingScheduler::Begin(boost::posix_time::ptime At, change FirstRow,
                        bool UseTenorBehind)
{
   TenorBehind = (FirstRow.size() % 2 == 1) && UseTenorBehind;
   NumBells = FirstRow.size() + TenorBehind;
   Server->SetNumberOfBells(NumBells);
   Running = true;
   AutoStartTriggered = false;
   AutoStartTimes.clear();
   Rows = changelist();
   RowTime.clear();
   RowError.clear();
   RowFracError.clear();
   RowGap.clear();
   SensorRecords.clear();
   NextRowStart = At;
   LastRowOfSensorInput = std::vector<int>(16,-1000);
   this->NextRow(FirstRow);
}

void
RingingScheduler::BeginAuto(change FirstRow, bool UseTenorBehind)
{
   TenorBehind = (FirstRow.size() % 2 == 1) && UseTenorBehind;
   NumBells = FirstRow.size() + TenorBehind;
   Server->SetNumberOfBells(NumBells);
   AutoStartRow = FirstRow;
   AutoStartTriggered = true;
   Running = false;
   AutoStartTimes.clear();
   Rows = changelist();
   RowTime.clear();
   RowError.clear();
   RowFracError.clear();
   RowGap.clear();
   SensorRecords.clear();
   LastRowOfSensorInput = std::vector<int>(16,-1000);

   TRACE("BeginAuto");
}

void
RingingScheduler::SetNumberOfBells(int n)
{
   NumBells = n;
   Server->SetNumberOfBells(n);
}

void
RingingScheduler::SetNumberOfBells(int n, bool UseTenorBehind)
{
   TenorBehind = (n % 2 == 1) && UseTenorBehind;
   NumBells = n + TenorBehind;
   Server->SetNumberOfBells(NumBells);
}

void
RingingScheduler::Abort()
{
   Running = false;
   Server->Abort();
}

void
RingingScheduler::Finish()
{
   Running = false;
   Server->Finish();
}

inline
boost::posix_time::time_duration
abs(boost::posix_time::time_duration td)
{
   if (td.is_negative())
      return td *= -1;
   return td;
}

void
RingingScheduler::NextRow(change c)
{
   c = extend(c, TenorBehind);
   Rows.push_back(c);
   RowTime.push_back(RowTimeType());
   RowError.push_back(RowErrorType());
   RowFracError.push_back(RowFracErrorType());
   RowGap.push_back(this->Gap());
   double ErrorFromSensor = ErrorFromSensorMultiplier * this->MeanSensorErrorMS(4) / this->Gap().total_milliseconds();
   for (int i = 0; i < c.size(); ++i)
   {
      int Bell = Rows.back()[i]+1;
      double ErrorFrac = 0.0;
      double DelayFrac = 0.0;
      if (Rows.size() % 2 == 0)
      {
         // backstroke
         DelayFrac = BackstrokeBellDelay[Bell];
         ErrorFrac = BackstrokeError + ErrorFromSensor + BackstrokeBellError[Bell];
      }
      else
      {
         // handstroke
         DelayFrac = HandstrokeBellDelay[Bell];
         ErrorFrac = HandstrokeError + ErrorFromSensor + HandstrokeBellError[Bell];
      }
      // Generate the random component of the error
      double DelayFracRandom = this->RandomGen() * ErrorFrac;
      boost::posix_time::time_duration NextError = this->Gap() * (DelayFrac+DelayFracRandom);

      if (RestartAfterSkip > 0 && (Rows.size() - LastRowOfSensorInput[Bell-1]) <= RestartAfterSkip)
      {
         Server->CancelBell(Bell, NextRowStart+NextError, NextError,
                            DelayFracRandom, Rows.size()-1, i+1);
      }
      else
      {
         Server->RingBell(Bell, NextRowStart+NextError, NextError,
                          DelayFracRandom, Rows.size()-1, i+1);
      }
      RowTime.back().push_back(NextRowStart);
      RowError.back().push_back(boost::posix_time::milliseconds(0));
      RowFracError.back().push_back(0.0);
      NextRowStart += this->Gap();
   }

   // handstroke gap
   if (Rows.size() % 2 == 0)
      NextRowStart += this->Gap() * HandstrokeGap;
}

boost::posix_time::ptime
RingingScheduler::NextRowStarts() const
{
   return NextRowStart;
}


int
RingingScheduler::NumRows()
{
   return Rows.size();
}

void
RingingScheduler::RingSensor(std::string const& Sensor, boost::posix_time::ptime At)
{
   int Bell = Server->Lookup(Sensor);
   if (Bell == 0)
      return;

   if (!Running)
   {
      // if we're not running, we've got no way of knowing whether this is
      // a handstroke or a backstroke.  So no way we can make an adjustment
      Server->RingSensor(Bell, At, Sensor);
      if (AutoStartTriggered)
      {
         if (AutoStartTimes.empty())
            AutoStartSensor = Sensor;

         if (Sensor == AutoStartSensor)
         {
            AutoStartTimes.push_back(At);
            if (AutoStartTimes.size() == 4)
            {
               // we've got 2 whole pulls, start on the next
               double Gap1 = (AutoStartTimes[1] - AutoStartTimes[0]).total_milliseconds() * 0.001;
               double Gap2 = (AutoStartTimes[2] - AutoStartTimes[1]).total_milliseconds() * 0.001;
               double Gap3 = (AutoStartTimes[3] - AutoStartTimes[2]).total_milliseconds() * 0.001;
               // get the bell gap, which includes 2 handstrokes
               double BellGap = (Gap1+Gap2+Gap3) / (AutoStartRow.size() * 3 + 2*HandstrokeGap);
               // calculate changes per minute from this
               double cpm = 60 / (BellGap*AutoStartRow.size() + 0.5*HandstrokeGap);
               this->SetChangesPerMinute(cpm);
               // now guess when the bell will sound again
               boost::posix_time::ptime NextRing = At + boost::posix_time::milliseconds(long(Gap2*1000));
               // and which place bell are we?
               int Place = AutoStartRow.place_of(Bell);
               this->Begin(NextRing - (Place-1)*this->Gap(AutoStartRow.size()), AutoStartRow);
            }
         }
      }
      return;
   }


   // find the row with the closest time for this bell


   // we dont know yet whether this is a handstroke or a backstroke, so be prepared for either adjustment
   std::vector<boost::posix_time::time_duration> AdjVec(2);
   AdjVec[0] = boost::posix_time::milliseconds(HandAdjust[Sensor]);
   AdjVec[1] = boost::posix_time::milliseconds(BackAdjust[Sensor]);

   TRACE("Running")(AdjVec[0])(AdjVec[1]);


   int RowOfBell = 0;
   int PlaceOfBell = Rows[0].place_of(Bell);
   boost::posix_time::ptime ExpectedTime = RowTime[0][PlaceOfBell-1];
   boost::posix_time::time_duration Error = abs(At + AdjVec[0] - ExpectedTime);
   for (unsigned i = 1; i < Rows.size(); ++i)
   {
      int TrialPlace = Rows[i].place_of(Bell);
      boost::posix_time::ptime TrialTime = RowTime[i][TrialPlace-1];
      boost::posix_time::time_duration TrialError = abs(At + AdjVec[i%2] - TrialTime);
      if (TrialError < Error)
      {
         RowOfBell = i;
         PlaceOfBell = TrialPlace;
         ExpectedTime = TrialTime;
         Error = TrialError;
      }
   }

   // adjust the handstroke/backstroke
   At += AdjVec[RowOfBell%2];

   // Get the actual error (not just the absolute value)
   Error = At - ExpectedTime;
   double FracError = Error / this->GapOfRow(RowOfBell);
   TRACE(Error)(FracError);
   SensorRecords[Sensor].push_back(SensorRecord(RowOfBell, PlaceOfBell, Error, FracError));

   // cancel any pending bells that were not silent
   for (int Row = RowOfBell; Row < int(Rows.size()); ++Row)
   {
      if (Row >= LastRowOfSensorInput[Bell-1]+RestartAfterSkip)
      {
         int Place = Rows[Row].place_of(Bell);
         Server->CancelBell(Bell, RowTime[Row][Place-1], RowError[Row][Place-1],
                            RowFracError[Row][Place-1],
                            Row, Place);
      }
   }
   Server->RingSensor(Bell, At, Error, FracError, Sensor, RowOfBell, PlaceOfBell);
   // the most recent row where the bell has rung
   LastRowOfSensorInput[Bell-1] = RowOfBell;
}

boost::posix_time::ptime
RingingScheduler::GetTimeOfRow(double AtRow) const
{
   DEBUG_CHECK(!Rows.empty());
   int Row = int(floor(AtRow));
   double Frac = AtRow - Row;

   boost::posix_time::ptime Time;
   if (Row < 0)
   {
      Time = RowTime[0][0] + this->RowDuration()*(Frac + Row);
   }
   else if (Row < int(Rows.size()))
   {
      Time = RowTime[Row][0] + this->RowDuration()*Frac;
   }
   else
   {
      Time = NextRowStart + this->RowDuration()*(Row - Rows.size() + Frac);
   }
   return Time;
}

void
RingingScheduler::LoadCall(std::string const& str)
{
   Server->LoadCall(str);
}

void
RingingScheduler::Division(int Row)
{
   Server->Division(Row);
}

void
RingingScheduler::Call(double AtRow, std::string const& str)
{
   Server->Call(this->GetTimeOfRow(AtRow), str);
}

void
RingingScheduler::Call(double AtRow, std::string const& str1, std::string const& str2)
{
   Server->Call(this->GetTimeOfRow(AtRow), str1, str2);
}

void
RingingScheduler::Call(double AtRow, std::string const& str1, std::string const& str2,
                       std::string const& str3)
{
   Server->Call(this->GetTimeOfRow(AtRow), str1, str2, str3);
}

void
RingingScheduler::CallGoMethod(double AtRow, std::string const& Name)
{
   Server->CallGoMethod(this->GetTimeOfRow(AtRow), Name);
}

void
RingingScheduler::CallSpliceMethod(double AtRow, std::string const& Name)
{
   Server->CallSpliceMethod(this->GetTimeOfRow(AtRow), Name);
}

char BellVocalNames[][9] = {"one", "two", "three", "four", "five", "six",
			    "seven", "eight", "nine", "ten", "eleven",
			    "twelve", "thirteen", "fourteen", "fifteen", "sixteen"};

void
RingingScheduler::CallChange(double AtRow, int Bell1, int Bell2)
{
   Server->Call(this->GetTimeOfRow(AtRow), BellVocalNames[Bell1], "follow", BellVocalNames[Bell2]);
}

void
RingingScheduler::CallDodging(double AtRow, int Bell1, int Bell2)
{
   Server->Call(this->GetTimeOfRow(AtRow), BellVocalNames[Bell1], "and", BellVocalNames[Bell2],
		"startdodging");
}

void
RingingScheduler::StopDodging(double AtRow, int Bell1, int Bell2)
{
   Server->Call(this->GetTimeOfRow(AtRow), BellVocalNames[Bell1], "and", BellVocalNames[Bell2],
		"stopdodging");
}

void
RingingScheduler::CallRightPlaces(double AtRow, int Bell1, int Bell2)
{
   Server->Call(this->GetTimeOfRow(AtRow), BellVocalNames[Bell1], "and", BellVocalNames[Bell2],
		"startmakingplaces");
}

void
RingingScheduler::CallWrongPlaces(double AtRow, int Bell1, int Bell2)
{
   Server->Call(this->GetTimeOfRow(AtRow), BellVocalNames[Bell1], "and", BellVocalNames[Bell2],
		"startmakingwrongplaces");
}

void
RingingScheduler::StopPlaces(double AtRow, int Bell1, int Bell2)
{
   Server->Call(this->GetTimeOfRow(AtRow), BellVocalNames[Bell1], "and", BellVocalNames[Bell2],
		"stopmakingplaces");
}

void
RingingScheduler::SetSensorAdjust(std::string const& Sensor, int HandAdjust_, int BackAdjust_)
{
   TRACE("SetSensorAdjust")(HandAdjust_)(BackAdjust_);
   HandAdjust[Sensor] = HandAdjust_;
   BackAdjust[Sensor] = BackAdjust_;
}

void
RingingScheduler::SetStrikingError(double Fraction)
{
   HandstrokeError = Fraction;
   BackstrokeError = Fraction;
}

void
RingingScheduler::SetHandstrokeStrikingError(double Fraction)
{
   HandstrokeError = Fraction;
}

void
RingingScheduler::SetBackstrokeStrikingError(double Fraction)
{
   BackstrokeError = Fraction;
}

void
RingingScheduler::SetBellStrikingDelay(int Bell, double Fraction)
{
   HandstrokeBellDelay[Bell] = Fraction;
   BackstrokeBellDelay[Bell] = Fraction;
}

void
RingingScheduler::SetBellHandstrokeStrikingDelay(int Bell, double Fraction)
{
   HandstrokeBellDelay[Bell] = Fraction;
}

void
RingingScheduler::SetBellBackstrokeStrikingDelay(int Bell, double Fraction)
{
   BackstrokeBellDelay[Bell] = Fraction;
}

void
RingingScheduler::SetBellStrikingError(int Bell, double Fraction)
{
   HandstrokeBellError[Bell] = Fraction;
   BackstrokeBellError[Bell] = Fraction;
}

void
RingingScheduler::SetBellHandstrokeStrikingError(int Bell, double Fraction)
{
   HandstrokeBellError[Bell] = Fraction;
}

void
RingingScheduler::SetBellBackstrokeStrikingError(int Bell, double Fraction)
{
   BackstrokeBellError[Bell] = Fraction;
}

void
RingingScheduler::SetUniformDistribution()
{
   UseGaussian = false;
}

void
RingingScheduler::SetGaussianDistribution()
{
   UseGaussian = true;
}

RingingScheduler::SensorRecordListType const&
RingingScheduler::GetSensorRecord(std::string const& Sensor)
{
   return SensorRecords[Sensor];
}

std::set<std::string>
RingingScheduler::GetRecordedSensors() const
{
   std::set<std::string> Result;
   for (std::map<std::string, SensorRecordListType>::const_iterator I = SensorRecords.begin(); I != SensorRecords.end(); ++I)
   {
      Result.insert(I->first);
   }
   return Result;
}

double
RingingScheduler::MeanSensorErrorMS(int n) const
{
   int TotalCount = 0;
   double TotalTimeSq = 0;

   // iterate over all sensors
   for (std::map<std::string, SensorRecordListType>::const_iterator I = SensorRecords.begin();
        I != SensorRecords.end();
        ++I)
   {
      // iterate over at most n records, in reverse order from the end
      int Count = 0;
      SensorRecordListType::const_iterator J = I->second.end();
      while (J != I->second.begin() && (n == 0 || Count < n))
      {
         --J;
         double ms = J->Error.total_milliseconds();
         if (ms <= this->GapOfRow(J->Row).total_milliseconds()) // ignore errors larger than 1 gap
         {
            TotalTimeSq += ms*ms;
            ++TotalCount;
            ++Count;
         }
      }
   }
   if (n != 0 && TotalCount < n)     // treat missing samples as zero
      TotalCount = n;
   return TotalCount == 0 ? 0.0 : std::sqrt(TotalTimeSq / TotalCount);
}

void
RingingScheduler::SetSensorErrorMultiplier(double x)
{
   ErrorFromSensorMultiplier = x;
}

double
RingingScheduler::RandomGen()
{
   return UseGaussian ? GaussianGen() : UniformGen();
}



