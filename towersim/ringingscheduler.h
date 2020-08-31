// -*- C++ -*- $Id: ringingscheduler.h 974 2012-07-04 22:20:50Z uqimccul $

#if !defined(RINGINGSCHEDULER_H_SJDHCWUEIYR894Y89)
#define RINGINGSCHEDULER_H_SJDHCWUEIYR894Y89

#include "change/change.h"
#include "change/touch.h"
#include "ringingserver.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

// for keeping a record of sensors
struct SensorRecord
{
   int Row;
   int Place;
   boost::posix_time::time_duration Error;
   double FracError;

   SensorRecord() {}
   SensorRecord(int Row_, int Place_, boost::posix_time::time_duration Error_, double FracError_)
      : Row(Row_), Place(Place_), Error(Error_), FracError(FracError_) {}
};

class RingingScheduler
{
   public:
      RingingScheduler(boost::shared_ptr<RingingServer> Server_);

      void SetChangesPerMinute(double cpm);
      void SetHandstrokeGap(double gap);

      // returns the gap of one place; this depends on knowing the number of bells.
      // I'd like to depreciate this method.  Or at least, make it a function of the number of bells.
      boost::posix_time::time_duration Gap() const;

      boost::posix_time::time_duration Gap(int NBells) const;

      // The duration of a row.  This includes 0.5 of a handstroke gap.
      boost::posix_time::time_duration RowDuration() const;

      // returns the gap at the particular row as it was rung
      boost::posix_time::time_duration GapOfRow(int Row) const;

      // this is used to indicate to the sound client how many bells we are ringing.
      // it is used to set up sounds when the bells are not ringing changes.
      // It isn't necessary to use this; the number of bells are set by calling Begin() anyway,
      // but this does set the number of bells for choosing the bell sounds
      void SetNumberOfBells(int n);

      // if UseTenorBehind is true and the number of bells is odd, then
      // use n+1 bells
      void SetNumberOfBells(int n, bool UseTenorBehind);

      // return the number of bells
      int GetNumberOfBells() const { return NumBells; }
 
      // beging ringing the touch at this time
      // if UseTenorBehind is true and the number of bells is odd, then
      // use n+1 bells and add the tenor behind.
      void Begin(boost::posix_time::ptime At, change FirstRow, bool UseTenorBehind = false);

      // Auto-start based on the first 2 whole-pulls of sensor input
      void BeginAuto(change FirstChange, bool UseTenorBehind = false);

      // returns the number of rows that have been scheduled
      int NumRows();

      change Row(int r) const { return Rows[r]; }

      // returns the time that the next change will start.
      // The NextRow() function should be called before this time
      boost::posix_time::ptime NextRowStarts() const;

      // set the handstroke and backstroke adjustment factors for the specified sensor
      void SetSensorAdjust(std::string const& Sensor, int HandAdjust_, int BackAdjust_);

      // rings the given sensor
      void RingSensor(std::string const& Sensor, boost::posix_time::ptime At);

      // sets the next row
      void NextRow(change c);

      // Indicate that there is a division (line between method blocks)
      // PRIOR to the given row number
      void Division(int Row);

      // prepares the ringing server by pre-loading the call
      void LoadCall(std::string const& str);

      // emits an audible call at the fractional row number.
      // Recommend that floor(AtRow) is not greater than this->NumRows(),
      // otherwise the call is too far in the future and may not be placed accurately
      // eg if the ringing speed changes or some other state change occurs.
      void Call(double AtRow, std::string const& str);

      void Call(double AtRow, std::string const& str1, std::string const& str2);

      void Call(double AtRow, std::string const& str1, std::string const& str2,
                std::string const& str3);

      void CallGoMethod(double AtRow, std::string const& Name);

      void CallSpliceMethod(double AtRow, std::string const& Name);

      // Short cut to call a call change with the specified zero-based bells
      void CallChange(double AtRow, int Bell1, int Bell2);

      // Short cut to call two bells to start dodging
      void CallDodging(double AtRow, int Bell1, int Bell2);

      // Short cut to call two bells to stop dodging
      void StopDodging(double AtRow, int Bell1, int Bell2);

      // Short cut to call two bells to start making places right
      void CallRightPlaces(double AtRow, int Bell1, int Bell2);

      // Short cut to call two bells to start making places wrong
      void CallWrongPlaces(double AtRow, int Bell1, int Bell2);

      // Short cut to call two bells to stop making places
      void StopPlaces(double AtRow, int Bell1, int Bell2);

      void Abort();

      void Finish();

      bool IsRunning() const { return Running; }

      boost::shared_ptr<RingingServer> GetServer() const { return Server; }

      // sets the overall striking error to be Gaussian random, 
      // with a standard devation of Fraction of a gap.
      // Equivalent to SetHandstrokeStrikingError(Fraction); SetBackstrokeStrikingError(Fraction)
      void SetStrikingError(double Fraction);

      // sets the overall striking error for handstrokes only to be Gaussian random, 
      // with a standard devation of Fraction of a gap.
      void SetHandstrokeStrikingError(double Fraction);

      // sets the overall striking error for backstrokes only to be Gaussian random, 
      // with a standard devation of Fraction of a gap.
      void SetBackstrokeStrikingError(double Fraction);

      // Sets the fixed delay of this bell (in fractions of a gap)
      // for both strokes.  The bell number is 1-based.
      void SetBellStrikingDelay(int Bell, double Fraction);

      // Sets the fixed delay of this bell (in fractions of a gap)
      // Handstroke only.  The bell number is 1-based.
      void SetBellHandstrokeStrikingDelay(int Bell, double Fraction);

      // Sets the fixed delay of this bell (in fractions of a gap)
      // Backstroke only.  The bell number is 1-based.
      void SetBellBackstrokeStrikingDelay(int Bell, double Fraction);

      // Sets the striking error of the given bell
      // to be Gaussian random with a standard devation
      // of the specified fraction of a bell gap.  The bell number is 1-based.
      void SetBellStrikingError(int Bell, double Fraction);
      void SetBellHandstrokeStrikingError(int Bell, double Fraction);
      void SetBellBackstrokeStrikingError(int Bell, double Fraction);

      // use a uniform distribution for the random numbers
      void SetUniformDistribution();

      // use a gaussian distribution for the random numbers
      void SetGaussianDistribution();

      typedef std::list<SensorRecord> SensorRecordListType;

      // returns the striking records of the given sensor
      SensorRecordListType const& GetSensorRecord(std::string const& Sensor);

      // returns the set of sensors that have a non-empty sensor record
      std::set<std::string> GetRecordedSensors() const;

      // returns the mean error duration (RMS error) in milliseconds of each
      // sensor inputs over the last n rows.  If there are fewer than n rows 
      // of sensor input, then treat the remaining
      // rows has having 0 error.  n=0 means take all rows of sensor input.
      // Errors of more than one full place are ignored.
      double MeanSensorErrorMS(int n = 0) const;

      // Set the sensor contribution to the total error to be MeanSensorErrorMS(4) * x
      void SetSensorErrorMultiplier(double x);

      // returns the time at which the given row started (or a guess, if it hasn't been scheduled yet)
      boost::posix_time::ptime GetTimeOfRow(double Row) const;

   private:
      double RandomGen();

      boost::shared_ptr<RingingServer> Server;
      int NumBells;
      double ChangesPerMinute;
      double HandstrokeGap;            // 1.0 for conventional handstrokes

      double HandstrokeError;  // handstroke error in fractions of a gap
      double BackstrokeError;  // backstroke error in fractions of a gap

      double ErrorFromSensorMultiplier;

      // individual bell fixed delay
      std::map<int, double> HandstrokeBellDelay;
      std::map<int, double> BackstrokeBellDelay;

      // individual bell errors, cumulative on the overall error
      std::map<int, double> HandstrokeBellError;
      std::map<int, double> BackstrokeBellError;

      bool Running;
      int TenorBehind; // number of tensors behind

      // the sensor to use for autostart
      bool AutoStartTriggered;
      change AutoStartRow;
      std::string AutoStartSensor;
      std::vector<boost::posix_time::ptime> AutoStartTimes;
 
      boost::posix_time::ptime NextRowStart;

      changelist Rows;

      // the time that each place bell rings at
      typedef std::vector<boost::posix_time::ptime> RowTimeType;
      std::vector<RowTimeType> RowTime;

      // the handstroke/backstroke adjustments
      std::map<std::string, int> HandAdjust;
      std::map<std::string, int> BackAdjust;

      std::map<std::string, SensorRecordListType> SensorRecords;

      // the error of each place bell
      typedef std::vector<boost::posix_time::time_duration> RowErrorType;
      std::vector<RowErrorType> RowError;

      // the fractional error of each place bell
      typedef std::vector<double> RowFracErrorType;
      std::vector<RowFracErrorType> RowFracError;

      // the ringing speed (row duration) at each row
      std::vector<boost::posix_time::time_duration> RowGap;

      boost::posix_time::ptime FinishTime;

      bool UseGaussian; // true if we should use gaussian numbers, false for uniform

      typedef boost::normal_distribution<> GaussianDistributionType;
      typedef boost::uniform_real<> UniformDistributionType;
      boost::mt19937 Rng;
      boost::variate_generator<boost::mt19937&, GaussianDistributionType> GaussianGen;
      boost::variate_generator<boost::mt19937&, UniformDistributionType> UniformGen;
};

#endif
