// -*- C++ -*- $Id: strikingwindow.h 987 2012-07-15 01:58:28Z uqimccul $
//

#if !defined(STRIKINGWINDOW_H_JKSDHWREUIYT3894Y89)
#define STRIKINGWINDOW_H_JKSDHWREUIYT3894Y89

#include "outputparser.h"
#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <vector>
#include <set>
#include <boost/date_time/posix_time/posix_time.hpp>

// structure to represent a bell as viewed in a graphics scene
struct BellRecord
{
   bool Exists;
   int Bell;
   int Row;
   int Place;
   boost::posix_time::ptime Time;
   boost::posix_time::time_duration Error;
   double FracError;
   std::string Sensor;
   bool Silent;
   QGraphicsTextItem* Item;
   QGraphicsLineItem* Line;

   BellRecord()
      : Exists(false), Item(NULL), Line(NULL) {}
};

typedef std::vector<BellRecord> BellRecordRowType;
typedef std::vector<BellRecordRowType> BellRecordType;

class StrikingWindow : public QGraphicsView
{
   public:
      StrikingWindow(QWidget* Parent);

      // if true, use a timer event to update the display in real time.
      // Otherwise update events immedately upon a ProcessLine command.
      void ShowRealTime(bool RealTime_);

      // resets the display
      void Reset();

      // processes a line of simulator output
      void ProcessLine(std::string const& Line);

      // The StrikingWindow acts as a visitor sink for OutputCommand's
 
      void operator()(OutCom::RingBell const& r);
      void operator()(OutCom::NumberOfBells const& n);
      void operator()(OutCom::Call const& c);
      void operator()(OutCom::CallGoMethod const& c);
      void operator()(OutCom::CallSpliceMethod const& c);
      void operator()(OutCom::LoadCall const& c);
      void operator()(OutCom::Division const& c);
      void operator()(OutCom::Finish const& c);
      void operator()(OutCom::Abort const& c);
      void operator()(OutCom::OutputError const& c);
 
      // returns a color to use for the output of the given sensor
      QColor ColorOfSensor(std::string const& Sensor);

   protected:
      virtual void resizeEvent(QResizeEvent* Event);
      virtual void timerEvent(QTimerEvent *event);

   private:
      void DoRingBell(OutCom::RingBell const& r);

      void Resize(int Width);

      QGraphicsScene* Scene;
      bool RealTime;
      int NumBells;
      bool Finished;
      bool WaitForFinish;
      //boost::posix_time::ptime BaseTime;
      int MyTimer;

      // parameters of the view
      double LeftMargin;
      double RightMargin;

      double PixelsPerRow;
      double PixelsPerPlace;

      double DivisionOffset;  // vertical offset in pixels for the location of a division line

      double LineMarginTop;      // margin at the top of a text item before we draw a line
      double LineMarginBottom;
      double BackgroundLineSize;  // number of pixels if the line underneath the bell number

      // for real-time output, keep a priority queue of pending bells to ring
      std::set<OutCom::RingBell> PendingBells;

      std::map<std::string, QColor> SensorColors;

      // the bell records are indexed by row and place
      BellRecordType BellRecords;
      std::vector<QGraphicsTextItem*> BellSymbols;  // the 'H' or 'B' symbols on the left hand side

      std::vector<int> DivisionRows;                  // the rows where we have division lines
      std::vector<QGraphicsLineItem*> DivisionLines;  // the rows where we have division lines

}; // class StrikingWindow

#endif
