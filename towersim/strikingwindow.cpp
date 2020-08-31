// -*- C++ -*- $Id: strikingwindow.cpp 987 2012-07-15 01:58:28Z uqimccul $

#include "strikingwindow.h"
#include "outputparser.h"
#include "change/change.h"  // for bell_name
#include <QResizeEvent>
#include <Qt>

int const TimerMS = 100;

StrikingWindow::StrikingWindow(QWidget* Parent)
   : QGraphicsView(Parent),
     Scene(new QGraphicsScene()),
     RealTime(false),
     NumBells(0),
     Finished(false),
     WaitForFinish(false),
     MyTimer(-1)
{
   this->setScene(Scene);
   this->show();

   // The fixed layout specifications
   PixelsPerRow = 18;

   LeftMargin = 30;  // leave enough room for the 'H' and 'B' symbols
   RightMargin = 20;

   LineMarginTop = 3;
   LineMarginBottom = 3;

   BackgroundLineSize = 2;

   // a sensible default, but we'll reset it on the resize anyway
   PixelsPerPlace = 20;

   DivisionOffset = 0;

   this->Resize(this->size().width());
}

void
StrikingWindow::ShowRealTime(bool RealTime_)
{
   RealTime = RealTime_;

   if (!RealTime)
   {
      // kill the timer
      if (MyTimer != -1)
      {
         this->killTimer(MyTimer);
         MyTimer = -1;
      }

      // show any pending bells immediately
      while (!PendingBells.empty())
      {
         this->operator()(*PendingBells.begin());
         PendingBells.erase(PendingBells.begin());
      }
   }
}

void
StrikingWindow::timerEvent(QTimerEvent*)
{
   if (PendingBells.empty())
   {
      if (WaitForFinish)
      {
         WaitForFinish = false;
         Finished = true;
         this->killTimer(MyTimer);
         MyTimer = -1;
      }
      return;
   }

   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   while (!PendingBells.empty() && 
          Now >= PendingBells.begin()->Time - boost::posix_time::milliseconds(TimerMS/2))
   {
      this->DoRingBell(*PendingBells.begin());
      PendingBells.erase(PendingBells.begin());
   }
}

void StrikingWindow::Reset()
{
   Finished = false;
   WaitForFinish = false;
   // clear the graphics scene
   Scene->clear();
   SensorColors.clear();
   BellRecords.clear();
   BellSymbols.clear();
   DivisionRows.clear();
   DivisionLines.clear();
   this->Resize(this->viewport()->size().width());
}

QColor
StrikingWindow::ColorOfSensor(std::string const& Sensor)
{
   std::map<std::string, QColor>::const_iterator I = SensorColors.find(Sensor);
   if (I != SensorColors.end())
      return I->second;

   QColor NextColor;
   switch (SensorColors.size() % 6)
   {
      case 0 : NextColor = Qt::blue; break;
      case 1 : NextColor = Qt::red; break;
      case 2 : NextColor = Qt::green; break;
      case 3 : NextColor = Qt::magenta; break;
      case 4 : NextColor = Qt::cyan; break;
      case 5 : NextColor = Qt::yellow; break;
   }
   SensorColors[Sensor] = NextColor;
   return NextColor;
}

void
StrikingWindow::operator()(OutCom::NumberOfBells const& n)
{
   NumBells = n.Number;
}

void
StrikingWindow::operator()(OutCom::RingBell const& r)
{
   if (RealTime)
   {
      PendingBells.insert(r);
      if (MyTimer == -1)
         MyTimer = this->startTimer(TimerMS);
   }
   else
   {
      this->DoRingBell(r);
   }
}

void
StrikingWindow::DoRingBell(OutCom::RingBell const& r)
{
   if (!r.Place)
      return;

   int Row = r.Place->get<0>();
   int Place = r.Place->get<1>();

   if (Finished)
      this->Reset();

   bool BottomOfScreenVisible = this->mapFromScene(Scene->sceneRect().bottomLeft()).y() 
      <= this->viewport()->height();

   //   TRACE(this->mapFromScene(Scene->sceneRect().bottomLeft()).y())(this->height());

   // make sure that we have enough rows to fit this record
   while (r.Place->get<0>()+1 > int(BellRecords.size()))
   {
      // add new record
      // draw the handstroke/backstroke symbols
      QGraphicsTextItem* Item = new QGraphicsTextItem(QString(BellRecords.size() % 2 == 0 ? 'H' : 'B'));
      Item->setPos(LeftMargin - Item->boundingRect().width(), PixelsPerRow * BellRecords.size());
      Scene->addItem(Item);

      BellRecords.push_back(BellRecordRowType(NumBells));
      BellSymbols.push_back(Item);

      // check to see if we have any division lines not yet displayed
      for (int i = 0; i < int(DivisionRows.size()); ++i)
      {
	 if (DivisionLines[i] == NULL && DivisionRows[i] < int(BellRecords.size()))
	 {
	    DivisionLines[i] = new QGraphicsLineItem(LeftMargin,
						     DivisionRows[i]*PixelsPerRow + DivisionOffset,
						     LeftMargin+PixelsPerPlace*(NumBells+1), 
						     DivisionRows[i]*PixelsPerRow + DivisionOffset);
	    Scene->addItem(DivisionLines[i]);
	 }
      }
   }

   // is this a duplicate?
   if (BellRecords[Row][Place-1].Exists)
   {
      // we've had this place bell before; possibly it was originally a computer bell
      // but now its been rung by a sensor (or vice-versa).
      // If a sensor bell has already rung, and this isn't a sensor bell,
      // then ignore this.  Otherwise we update with the new record.
      if (!r.Sensor && !BellRecords[Row][Place-1].Sensor.empty())
         return;
   }

   BellRecords[Row][Place-1].Exists = true; 
   BellRecords[Row][Place-1].Bell = r.Bell;
   BellRecords[Row][Place-1].Row = Row;
   BellRecords[Row][Place-1].Place = Place;
   BellRecords[Row][Place-1].Time = r.Time;
   BellRecords[Row][Place-1].Error = boost::posix_time::time_duration(0,0,0,0);
   BellRecords[Row][Place-1].FracError = 0.0;
   if (r.Error)
   {
      BellRecords[Row][Place-1].Error = r.Error->get<0>();
      BellRecords[Row][Place-1].FracError = r.Error->get<1>();
   }
   BellRecords[Row][Place-1].Sensor = r.Sensor ? (*r.Sensor) : std::string();
   BellRecords[Row][Place-1].Silent = r.Silent;

   double FracError = BellRecords[Row][Place-1].FracError;
   //   TRACE(FracError);

   double x = PixelsPerPlace * (Place + FracError) + LeftMargin;

   // cutoff at the margin boundaries
   if (x < LeftMargin)
      x = LeftMargin;
   if (x > PixelsPerPlace*(NumBells+1) + LeftMargin)
      x = PixelsPerPlace*(NumBells+1) + LeftMargin;

   double xTrue = PixelsPerPlace*Place + LeftMargin;
   double y = PixelsPerRow * Row;

   // no matter what we do, we need to redraw or remove the item
   if (BellRecords[Row][Place-1].Item)
   {
      Scene->removeItem(BellRecords[Row][Place-1].Item);
      delete BellRecords[Row][Place-1].Item;
      BellRecords[Row][Place-1].Item = NULL;
   }

   // make an item whether we draw it or not; we need the bounding box anyway
   QGraphicsTextItem* Item = new QGraphicsTextItem(QString(bell_name(r.Bell)));
   double ItemWidth = Item->boundingRect().width();
   double ItemHeight = Item->boundingRect().height();

   // Add a line underneath our bell if this is a sensor bell (whether we're silent or not)
   // or if its a computer bell with an error
   if (BellRecords[Row][Place-1].Silent || !BellRecords[Row][Place-1].Sensor.empty()
       || BellRecords[Row][Place-1].FracError != 0.0)
   {
      if (BellRecords[Row][Place-1].Line)
      {
         Scene->removeItem(BellRecords[Row][Place-1].Line);
         delete BellRecords[Row][Place-1].Line;
      }

      QGraphicsLineItem* Line = new 
         QGraphicsLineItem(xTrue, y+LineMarginTop, xTrue, y+ItemHeight-LineMarginBottom);
      Line->setPen(QPen(Item->defaultTextColor()));
      Line->setZValue(-2.0);  // draw behind everything
      BellRecords[Row][Place-1].Line = Line;
      Scene->addItem(Line);
   }

   // draw in the symbol if we're not silent
   if (!BellRecords[Row][Place-1].Silent)
   {
      // draw in dots behind, if we've drawn a line
      if (BellRecords[Row][Place-1].Line)
      {
         // if we're a sensor bell then set the color
         if (!BellRecords[Row][Place-1].Sensor.empty())
            Item->setDefaultTextColor(this->ColorOfSensor(BellRecords[Row][Place-1].Sensor));
         // and add a line to the symbol, this is in coords relative to the symbol

         QGraphicsLineItem* ItemLine = new QGraphicsLineItem(Item);
         ItemLine->setPen(QPen(Item->defaultTextColor()));
         ItemLine->setLine(ItemWidth*0.5, LineMarginTop, ItemWidth*0.5, LineMarginTop+BackgroundLineSize);
         ItemLine->setZValue(-1.0);

         ItemLine = new QGraphicsLineItem(Item);
         ItemLine->setPen(QPen(Item->defaultTextColor()));
         ItemLine->setLine(ItemWidth*0.5, ItemHeight-LineMarginBottom-BackgroundLineSize, 
                           ItemWidth*0.5, ItemHeight-LineMarginBottom);
         ItemLine->setZValue(-1.0);

      }
      Item->setPos(x-ItemWidth*0.5,y);
      BellRecords[Row][Place-1].Item = Item;
      Scene->addItem(Item);
   }
   else
   {
      
      BellRecords[Row][Place-1].Line = NULL;
   }
   if (BottomOfScreenVisible)
      this->ensureVisible(50, Scene->sceneRect().height(), this->size().width()-100, 0);
}

void
StrikingWindow::operator()(OutCom::Finish const&)
{
   WaitForFinish = true;
}

void
StrikingWindow::operator()(OutCom::Abort const&)
{
   Finished = true;
   PendingBells.clear();
}

void
StrikingWindow::operator()(OutCom::Division const& d)
{
   DivisionRows.push_back(d.Row);
   DivisionLines.push_back(NULL);

   // only display the line if we've already put at least one bell on the following row
   if (d.Row < int(BellRecords.size()))
   {
      DivisionLines.back() = new QGraphicsLineItem(LeftMargin,
						   d.Row*PixelsPerRow + DivisionOffset,
						   LeftMargin+PixelsPerPlace*(NumBells+1),
						   d.Row*PixelsPerRow + DivisionOffset);
      Scene->addItem(DivisionLines.back());
   }
}

void
StrikingWindow::operator()(OutCom::Call const&)
{
}

void
StrikingWindow::operator()(OutCom::CallGoMethod const&)
{
}

void
StrikingWindow::operator()(OutCom::CallSpliceMethod const&)
{
}

void
StrikingWindow::operator()(OutCom::LoadCall const&)
{
}

void
StrikingWindow::operator()(OutCom::OutputError const& e)
{
   std::cerr << "Ignorning OutputError emitted to StrikingWindow: " << e.What << std::endl;
}

void
StrikingWindow::Resize(int Width)
{
   bool BottomOfScreenVisible = this->mapFromScene(Scene->sceneRect().bottomLeft()).y() 
      <= this->viewport()->height();

   Scene = new QGraphicsScene();
   this->setScene(Scene);
   // recalculate the pixels per place
   PixelsPerPlace = floor((Width-LeftMargin-RightMargin) / (NumBells+1));
   for (unsigned r = 0; r < BellRecords.size(); ++r)
   {
      Scene->addItem(BellSymbols[r]);

      for (unsigned p = 0; p < BellRecords[r].size(); ++p)
      {
         if (BellRecords[r][p].Item)
         {
            int Place = BellRecords[r][p].Place;
            int Row = BellRecords[r][p].Row;
            double FracError = BellRecords[r][p].FracError;
            double x = PixelsPerPlace * (Place + FracError) + LeftMargin;

            // cutoff at the margin
            if (x < LeftMargin)
               x = LeftMargin;
            if (x > PixelsPerPlace*(NumBells+1) + LeftMargin)
               x = PixelsPerPlace*(NumBells+1) + LeftMargin;
            
            double xTrue = PixelsPerPlace*Place + LeftMargin;
            double y = PixelsPerRow * Row;
            double ItemWidth = BellRecords[r][p].Item->boundingRect().width();
            double ItemHeight = BellRecords[r][p].Item->boundingRect().height();
            BellRecords[r][p].Item->setPos(x-ItemWidth*0.5,y);
            Scene->addItem(BellRecords[r][p].Item);
            if (BellRecords[r][p].Line)
            {
               BellRecords[r][p].Line->setLine(xTrue,
                                               y+LineMarginTop,
                                               xTrue,
                                               y+ItemHeight-LineMarginBottom);
               Scene->addItem(BellRecords[r][p].Line);
            }
         }
      }
   }
   for (unsigned r = 0; r < DivisionRows.size(); ++r)
   {
      if (DivisionLines[r] == NULL)
      {
	 if (DivisionRows[r] < int(BellRecords.size()))
	    DivisionLines[r] = new QGraphicsLineItem(LeftMargin,
						     DivisionRows[r]*PixelsPerRow + DivisionOffset,
						     LeftMargin+PixelsPerPlace*(NumBells+1), 
						     DivisionRows[r]*PixelsPerRow + DivisionOffset);
	 Scene->addItem(DivisionLines[r]);
      }
      else
      {
	 DivisionLines[r]->setLine(LeftMargin,
				   DivisionRows[r]*PixelsPerRow + DivisionOffset,
				   LeftMargin+PixelsPerPlace*(NumBells+1), 
				   DivisionRows[r]*PixelsPerRow + DivisionOffset);
	 Scene->addItem(DivisionLines[r]);
      }
   }
   double y = this->sceneRect().y();
   double Height = this->size().height();
   this->ensureVisible(0, y, Width, Height);
   this->show();

   if (BottomOfScreenVisible)
      this->ensureVisible(50, Scene->sceneRect().height(), this->size().width()-100, 0);
}

void
StrikingWindow::resizeEvent(QResizeEvent* Event)
{
   this->Resize(Event->size().width());
   QGraphicsView::resizeEvent(Event);
}
