// -*- C++ -*- $Id: methodcontroller.cpp 1778 2016-08-29 08:34:22Z uqimccul $

#include "methodcontroller.h"
#include "ui_ui-towersim-mainwindow.h"
#include "ringingscheduler.h"
#include "select-method.h"
#include <QSignalMapper>
#include <cctype>

// We define a 'Call Window', which is the region of time in which a call
// can be made that affects a given row.  This is specified by the
// start and end points of the window, measured in fractional numbers of rows.
// A particular change is committed (and dispatched to the sound client)
// at the end of the call window.  Calls can be made any time from the 
// start of the call window.  Scheduled calls are actually made at
// 'middle' of the call window.
// Note that the width of the call window must be strictly less than 1 row,
// otherwise the call window would start before the controller knows
// which calls are available for the proper row.
double CallWindowStart = 2.5;
double CallWindowMiddle = 2.3;
double CallWindowFinish = 1.6;

MethodController::MethodController(boost::shared_ptr<RingingScheduler> Scheduler_, 
                             Ui::MainWindow* MainWindow)
   : ui(MainWindow),
     Scheduler(Scheduler_),
     CCLabel(MaxBells),
     CCButton(MaxBells-1),
     MyTimer(-1),
     Ringing(false),
     CurrentChange(change(8)),
     StandRow(-1),
     CommittedStand(false),
     CallStand(false),
     RoundsCalled(None),
     CurrentCallNum(0),
     MethodCallButton(MaxCalls, NULL),
     Dodge(MaxBells-1, None),
     RightPlaces(MaxBells-1, None),
     WrongPlaces(MaxBells-1, None),
     CallChange(MaxBells-1, None),
     RingingMethod(false),
     GoCalled(None),
     CurrentMethod(NULL),
     NextMethod(NULL),
     NumCourses(0),
     SelectMethod(NULL)
{
   // setup the call change labelling and button
   CCLabel[0] = ui->CC1;
   CCLabel[1] = ui->CC2;
   CCLabel[2] = ui->CC3;
   CCLabel[3] = ui->CC4;
   CCLabel[4] = ui->CC5;
   CCLabel[5] = ui->CC6;
   CCLabel[6] = ui->CC7;
   CCLabel[7] = ui->CC8;
   CCLabel[8] = ui->CC9;
   CCLabel[9] = ui->CC10;
   CCLabel[10] = ui->CC11;
   CCLabel[11] = ui->CC12;
   CCLabel[12] = ui->CC13;
   CCLabel[13] = ui->CC14;
   CCLabel[14] = ui->CC15;
   CCLabel[15] = ui->CC16;

   CCButton[0] = ui->Call12;
   CCButton[1] = ui->Call23;
   CCButton[2] = ui->Call34;
   CCButton[3] = ui->Call45;
   CCButton[4] = ui->Call56;
   CCButton[5] = ui->Call67;
   CCButton[6] = ui->Call78;
   CCButton[7] = ui->Call89;
   CCButton[8] = ui->Call90;
   CCButton[9] = ui->Call0E;
   CCButton[10] = ui->CallET;
   CCButton[11] = ui->CallTA;
   CCButton[12] = ui->CallAB;
   CCButton[13] = ui->CallBC;
   CCButton[14] = ui->CallCD;

   connect(ui->NumberOfBells, SIGNAL(valueChanged(int)),
           this, SLOT(NumberOfBells_valueChanged(int)));

   connect(ui->RingingStart, SIGNAL(clicked()), this, SLOT(RingingStart()));
   connect(ui->RingingAuto, SIGNAL(clicked()), this, SLOT(RingingAuto()));
   connect(ui->RingingStop, SIGNAL(clicked()), this, SLOT(RingingStop()));
   connect(ui->RingingRounds, SIGNAL(clicked()), this, SLOT(RingingRounds()));
   connect(ui->RingingStand, SIGNAL(clicked()), this, SLOT(RingingStand()));
   connect(ui->SelectCallChange, SIGNAL(clicked()), this, SLOT(CallModeChanged()));
   connect(ui->SelectDodge, SIGNAL(clicked()), this, SLOT(CallModeChanged()));
   connect(ui->SelectRightPlaces, SIGNAL(clicked()), this, SLOT(CallModeChanged()));
   connect(ui->SelectWrongPlaces, SIGNAL(clicked()), this, SLOT(CallModeChanged()));

   connect(ui->SelectMethod, SIGNAL(clicked()), this, SLOT(SelectMethodPushed()));
   connect(ui->MethodGo, SIGNAL(clicked()), this, SLOT(MethodGoPushed()));
   connect(ui->MethodGoDelay, SIGNAL(toggled(bool)), this, SLOT(MethodGoDelayToggled(bool)));

   QSignalMapper* CallMapper = new QSignalMapper(this);
   for (int i = 0; i < MaxBells-1; ++i)
   {
      connect(CCButton[i], SIGNAL(clicked()), CallMapper, SLOT(map()));
      CallMapper->setMapping(CCButton[i], i);
   }
   connect(CallMapper, SIGNAL(mapped(int)), this, SLOT(CCButtonClicked(int)));

   // set up the buttons for the calls (bob, single etc)
   MethodCallButton[0] = ui->MethodCall0;
   MethodCallButton[1] = ui->MethodCall1;
   MethodCallButton[2] = ui->MethodCall2;
   MethodCallButton[3] = ui->MethodCall3;
   QSignalMapper* CallBobMapper = new QSignalMapper(this);
   for (int i = 0; i < MaxCalls; ++i)
   {
      connect(MethodCallButton[i], SIGNAL(clicked()), CallBobMapper, SLOT(map()));
      CallBobMapper->setMapping(MethodCallButton[i], i);
   }
   connect(CallBobMapper, SIGNAL(mapped(int)), this, SLOT(CallClicked(int)));

   // initialize the number of bells
   this->NumberOfBells_valueChanged(ui->NumberOfBells->value());

   // Initialize other UI elements
   this->UiStanding();
   this->UpdateButtons();
   this->SetCurrentMethod(NULL);

   SelectMethod = new SelectMethodDialog();
}

void
MethodController::NumberOfBells_valueChanged(int NumBells)
{
   // update the change window
   for (int i = 0; i < MaxBells; ++i)
   {
      CCLabel[i]->setVisible(i < NumBells);
   }
   for (int i = 0; i < MaxBells-1; ++i)
   {
      CCButton[i]->setVisible(i < NumBells-1);
   }
   CurrentChange = change(NumBells);
   this->DisplayChange();
   Scheduler->SetNumberOfBells(NumBells);
}

void
MethodController::DisplayChange()
{
   bool UpdateLabels = true;
   // Set the change window
   for (int i = 0; i < CurrentChange.size(); ++i)
   {
      CCLabel[i]->setText(QString(bell_name_z(CurrentChange[i])));
   }
   if (UpdateLabels)
   {
      // update the button labels
      for (int i = 0; i < CurrentChange.size()-1; ++i)
      {
	 CCButton[i]->setText(bell_name_z(CurrentChange[i])
			      +QString('-')
			      +bell_name_z(CurrentChange[i+1]));
      }
   }
}

void
MethodController::RingingStart()
{
   this->Reset(false);
   Ringing = true;
   StandRow = -1;
   DisplayedRow = 0;
   Scheduler->Begin(boost::posix_time::microsec_clock::universal_time(), CurrentChange);
   MyTimer = this->startTimer(50);
   this->UiRinging();
}

void
MethodController::RingingAuto()
{
   this->Reset(false);
   Ringing = true;
   StandRow = -1;
   CallStand = false;
   DisplayedRow = 0;
   Scheduler->BeginAuto(CurrentChange);
   MyTimer = this->startTimer(50);
   this->UiRinging();
}

void
MethodController::UpdateButtons()
{
   // igf we're not ringing then everything is disabled except for call changes
   if (!Ringing)
   {
      for (int i = 0; i < MaxBells-1; ++i)
      {
	 if (ui->SelectCallChange->isChecked())
	 {
	    CCButton[i]->setCheckable(false);
	    CCButton[i]->setEnabled(true);
	 }
	 else
	 {
	    CCButton[i]->setCheckable(false);
	    CCButton[i]->setEnabled(false);
	 }
      }
      return;
   }
	    	    
   // else we are ringing
   for (int i = 0; i < MaxBells-1; ++i)
   {
      // Disable the button if we have a committed or current swap in either a call or a dodge
      // in the current or neighboring channels.
      bool Disable = (CallChange[i] != None)
	 || (Dodge[i] != None)
	 || (RightPlaces[i] != None)
	 || (WrongPlaces[i] != None);
      if (i > 0)
	 Disable = Disable || (CallChange[i-1] != None)
	 || (Dodge[i-1] != None)
	 || (RightPlaces[i-1] != None)
	 || (WrongPlaces[i-1] != None);
      if (i < MaxBells-2)
	 Disable = Disable || (CallChange[i+1] != None)
	 || (Dodge[i+1] != None)
	 || (RightPlaces[i+1] != None)
	 || (WrongPlaces[i+1] != None);

      // Also disable if any nearby bell is participating in the method
      if (RingingMethod || GoCalled == Pending)
      {
	 Disable = Disable || i < CurrentMethod->stage();
      }

      if (ui->SelectCallChange->isChecked())
      {
	 CCButton[i]->setCheckable(false);
	 CCButton[i]->setEnabled(!Disable);
      }
      else if (ui->SelectDodge->isChecked())
      {
	 CCButton[i]->setCheckable(true);
	 CCButton[i]->setEnabled(Ringing && (!Disable || Dodge[i] == Current));
	 CCButton[i]->setChecked(Dodge[i] == Pending || Dodge[i] == Committed 
				 || Dodge[i] == Current);
      }
      else if (ui->SelectRightPlaces->isChecked())
      {
	 CCButton[i]->setCheckable(true);
	 CCButton[i]->setEnabled(Ringing && (!Disable || RightPlaces[i] == Current));
	 CCButton[i]->setChecked(RightPlaces[i] == Pending || RightPlaces[i] == Committed 
				 || RightPlaces[i] == Current);
      }
      else if (ui->SelectWrongPlaces->isChecked())
      {
	 CCButton[i]->setCheckable(true);
	 CCButton[i]->setEnabled(Ringing && (!Disable || WrongPlaces[i] == Current));
	 CCButton[i]->setChecked(WrongPlaces[i] == Pending || WrongPlaces[i] == Committed 
				 || WrongPlaces[i] == Current);
      }
   }
   // Update the available go/splice buttons
   this->UpdateNextMethod();
}

void
MethodController::Reset(bool ResetCurrentChange)
{
   Ringing = false;
   CommittedStand = false;
   CallStand = false;
   RoundsCalled = None;
   Dodge = std::vector<CallState>(MaxBells-1, None);
   RightPlaces = std::vector<CallState>(MaxBells-1, None);
   WrongPlaces = std::vector<CallState>(MaxBells-1, None);
   CallChange = std::vector<CallState>(MaxBells-1, None);
   GoCalled = None;
   RingingMethod = false;
   this->SetCurrentMethod(NULL);
   NumCourses = 0;
   if (ResetCurrentChange)
      CurrentChange = change(CurrentChange.size());
   this->DisplayChange();
   this->UpdateButtons();
   this->UiStanding();
}

void 
MethodController::timerEvent(QTimerEvent*)
{
   if (!Scheduler->IsRunning())
      return;

   // current time
   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   // time that the next row (yet to be scheduled) will start
   boost::posix_time::ptime NextRowStarts = Scheduler->NextRowStarts();
   int NextRow = Scheduler->NumRows();

   // See if we should update the display
   if (DisplayedRow+1 < Scheduler->NumRows() && Now > Scheduler->GetTimeOfRow(DisplayedRow+1))
   {
      ++DisplayedRow;
      CurrentChange = Scheduler->Row(DisplayedRow);
      this->DisplayChange();
      // if the displayed row is a handstroke, then clear any current swaps
      if (DisplayedRow % 2 == 0)
      {
	 for (int i = 0; i < MaxBells-1; ++i)
	 {
	    if (CallChange[i] == Current)
	       CallChange[i] = None;
	 }
	 this->UpdateButtons();
      }
   }

   // Pending calls come at the middle of the call window
   if (Now > NextRowStarts - Scheduler->RowDuration() * CallWindowMiddle)
   {
      // We are inside the call window.  Any pending calls?

      // Firstly, has 'stand' been called?
      if (NextRow == StandRow && CallStand)
      {
	 Scheduler->Call(NextRow - CallWindowMiddle, "stand");
	 CommittedStand = true;
	 CallStand = false;
      }

      // Has 'rounds' been called?
      if (RoundsCalled == Pending)
      {
	 Scheduler->Call(NextRow - CallWindowMiddle, "rounds");
	 RoundsCalled = Committed;
      }

      // Has 'thats all' been called?
      if (GoCalled == PendingStop)
      {
	 Scheduler->Call(NextRow - CallWindowMiddle, "thatsall");
	 GoCalled = CommittedStop;
	 RingingMethod = false;
	 // Auto-stand?
	 if (ui->MethodStandAfterBox->isChecked())
	 {
	    StandRow = NextRow + 2*ui->MethodStandAfter->value();
	    if (StandRow % 2 == 1)  // only stand at handstroke
	       ++StandRow;
	    CallStand = true;
	 }
      }

      // Any calls?
      if (RingingMethod && NextCall.is_null() && MethodCallButton[CurrentCallNum]->isChecked())
      {
	 // See if the currently selected call is available right now
	 std::vector<call> CallsNow = Cursor.calls_available_now();
	 if (std::find(CallsNow.begin(), CallsNow.end(), AvailableCalls[CurrentCallNum])
	     != CallsNow.end())
	 {
	    // convert the call name to lowercase and announce it
	    NextCall = AvailableCalls[CurrentCallNum];
	    std::string CallName = NextCall.Name;
	    std::transform(CallName.begin(), CallName.end(), CallName.begin(), (int(*)(int))std::tolower);
	    Scheduler->Call(NextRow - CallWindowMiddle, CallName);
	    if (!ui->MethodCallRepeat->isChecked())
	       MethodCallButton[CurrentCallNum]->setChecked(false);
	    // disable the buttons until the end of the call window
	    for (unsigned i = 0; i < AvailableCalls.size(); ++i)
	    {
	       MethodCallButton[i]->setEnabled(false);
	    }
	 }
      }

      // Are we starting a method?
      if (GoCalled == Pending && NextRow == GoRow)
      {
	 // verify that Method is valid
	 if (CurrentMethod && this->MaxChangeRingingBells() >= CurrentMethod->stage())
	 {
	    Scheduler->CallGoMethod(NextRow - CallWindowMiddle, CurrentMethod->full_name_stage());
	    this->CommitGoMethod();
	 }
	 else
	 {
	    // we attempted to start a method but it was invalid (or the method changed
	    // after posting the call).  This probably shouldn't happen, but if it did
	    // then the best we can do is just ignore the request
	    GoCalled = None;
	    ui->MethodGoDelay->setChecked(false);
	    this->UiRinging();
	 }
      }

      // For call changes, dodges, we only go at handstroke
      if (NextRow % 2 == 0)
      {
	 // Next row is a handstroke, see if we have any pending call changes etc
	 for (int i = 0; i < MaxBells-1; ++i)
	 {
	    if (CallChange[i] == Pending)
	    {
	       CallChange[i] = Committed;
	       Scheduler->CallChange(NextRow - CallWindowMiddle, 
				     Scheduler->Row(NextRow-1)[i],
				     Scheduler->Row(NextRow-1)[i+1]);
	    }
	    if (Dodge[i] == Pending)
	    {
	       Dodge[i] = Committed;
	       Scheduler->CallDodging(NextRow - CallWindowMiddle,
				      Scheduler->Row(NextRow-1)[i],
				      Scheduler->Row(NextRow-1)[i+1]);
	    }
	    if (Dodge[i] == PendingStop)
	    {
	       Dodge[i] = CommittedStop;
	       Scheduler->StopDodging(NextRow - CallWindowMiddle,
				      Scheduler->Row(NextRow-1)[i],
				      Scheduler->Row(NextRow-1)[i+1]);
	    }
	    if (RightPlaces[i] == Pending)
	    {
	       RightPlaces[i] = Committed;
	       Scheduler->CallRightPlaces(NextRow - CallWindowMiddle,
					  Scheduler->Row(NextRow-1)[i],
					  Scheduler->Row(NextRow-1)[i+1]);
	    }
	    if (RightPlaces[i] == PendingStop)
	    {
	       RightPlaces[i] = CommittedStop;
	       Scheduler->StopPlaces(NextRow - CallWindowMiddle,
				     Scheduler->Row(NextRow-1)[i],
				     Scheduler->Row(NextRow-1)[i+1]);
	    }
	 }
      }
      else
      {
	 // wrong places start at backstroke
	 for (int i = 0; i < MaxBells-1; ++i)
	 {
	    if (WrongPlaces[i] == Pending)
	    {
	       WrongPlaces[i] = Committed;
	       Scheduler->CallWrongPlaces(NextRow - CallWindowMiddle,
					  Scheduler->Row(NextRow-1)[i],
					  Scheduler->Row(NextRow-1)[i+1]);
	    }
	    if (WrongPlaces[i] == PendingStop)
	    {
	       WrongPlaces[i] = CommittedStop;
	       Scheduler->StopPlaces(NextRow - CallWindowMiddle,
				     Scheduler->Row(NextRow-1)[i],
				     Scheduler->Row(NextRow-1)[i+1]);
	    }
	 }
      }
   }

   // see if we've reached the end of the window
   // which means that it is time to schedule the next change
   if (Now > NextRowStarts - Scheduler->RowDuration() * CallWindowFinish)
   {
      // Check to see if we're standing
      if (NextRow == StandRow)
      {
	 // we're committed to standing.  Wait plenty of time to stop.
	 if (Now > Scheduler->NextRowStarts() + Scheduler->Gap() * 2)
	 {
	    CHECK(CommittedStand);
	    Scheduler->Finish();
	    this->killTimer(MyTimer);
	    this->Reset();
	 }
	 return;  // return without scheduling more changes
      }

      // Get the current row
      change ThisChange = Scheduler->Row(NextRow-1);
      change NextChange = ThisChange;

      // has rounds been called?
      if (RoundsCalled == Committed)
      {
	 RoundsCalled = None;
	 NextChange = change(ThisChange.size());
	 Dodge = std::vector<CallState>(MaxBells-1, None);
	 RightPlaces = std::vector<CallState>(MaxBells-1, None);
	 WrongPlaces = std::vector<CallState>(MaxBells-1, None);
	 CallChange = std::vector<CallState>(MaxBells-1, None);
	 GoCalled = None;
	 ui->MethodGoDelay->setChecked(false);
	 RingingMethod = false;
	 ui->CurrentMethod->setText("");
	 this->UiRinging();
      }

      // Manipulate it by whatever calls have been committed
      for (int i = 0; i < MaxBells-1; ++i)
      {
	 if (CallChange[i] == Committed)
	 {
	    std::swap(NextChange[i], NextChange[i+1]);
	    CallChange[i] = Current;
	 }
      }
      for (int i = 0; i < MaxBells-1; ++i)
      {
	 if (NextRow % 2 == 0)
	 {
	    // are we starting dodging?  Do this only at handstroke
	    if (Dodge[i] == Committed)
	       Dodge[i] = Current;
	    if (Dodge[i] == CommittedStop)
	       Dodge[i] = None;
	    if (Dodge[i] == Current || Dodge[i] == PendingStop)
	       std::swap(NextChange[i], NextChange[i+1]);
	    // We stop dodging only at backstroke

	    // making right places?
	    if (RightPlaces[i] == Committed)
	       RightPlaces[i] = Current;
	    if (RightPlaces[i] == CommittedStop)
	       RightPlaces[i] = None;
	    if (RightPlaces[i] == Current || RightPlaces[i] == PendingStop)
	       std::swap(NextChange[i], NextChange[i+1]);
	 }
	 else
	 {
	    // backstroke
	    if (WrongPlaces[i] == Committed)
	       WrongPlaces[i] = Current;
	    if (WrongPlaces[i] == CommittedStop)
	       WrongPlaces[i] = None;
	    if (WrongPlaces[i] == Current || WrongPlaces[i] == PendingStop)
	       std::swap(NextChange[i], NextChange[i+1]);

	    // Dodge (but we don't start or stop dodging at backstroke)
	    if (Dodge[i] == Current || Dodge[i] == PendingStop)
	       std::swap(NextChange[i], NextChange[i+1]);
	 }
      }

      // If we've started ringing a method, then set the starting change
      // and other housekeeping
      if (GoCalled == Committed)
      {
	 GoCalled = Current;
	 RingingMethod = true;
	 // record the starting change
	 StartChange = NextChange;
	 this->UiRingingMethod();
	 // Insert a division prior to the last row of rounds, unless we're going
	 // to have a division at the next row anyway
	 if (!Cursor.is_division())
	    Scheduler->Division(NextRow-1);
      }

      // are we ringing a method?
      if (RingingMethod)
      {
	 if (Cursor.is_division())
	 {
	    Scheduler->Division(NextRow);
	 }

	 // any pending call?
	 if (!NextCall.is_null())
	 {
	    // Do the call
	    Cursor.set_call(NextCall);
	    NextCall = call();
	    // re-enable the call buttons
	    for (unsigned i = 0; i < AvailableCalls.size(); ++i)
	    {
	       MethodCallButton[i]->setEnabled(true);
	    }
	 }

	 ++Cursor;
	 // merge the method into the current row.  Firstly take the start row
	 // and modify it by the current row at the cursor
	 change c = StartChange * extend(*Cursor, NextChange.size() - Cursor->size());
	 // and modify the first Cursor.stage() bells into the NextChange
	 bool TryRounds = true;
	 for (int i = 0; i < Cursor.stage(); ++i)
	 {
	    NextChange[i] = c[i];
	    TryRounds = TryRounds && (NextChange[i] == i);
	 }

	 // That's all?  We could also check for Cursor.is_rounds()
	 if (TryRounds)
	 {
	    ++NumCourses;
	    if (ui->MethodThatsAll->isChecked() 
		|| (ui->MethodKeepGoing->value() > 0 && NumCourses >= ui->MethodKeepGoing->value()))
	    {
	       GoCalled = PendingStop;
	    }
	 }
      }
      Scheduler->NextRow(NextChange);
      this->UpdateButtons();
   }
}

void MethodController::CommitGoMethod()
{
   NumCourses = 0;
   this->UiRinging();  // to disable the go buttons
   GoCalled = Committed;
   // Don't set the start change - there may still be time in the call window for it to change???
   // Initialize the cursor
   Cursor.reset(*CurrentMethod);
   ui->MethodGoDelay->setChecked(false);
}

void
MethodController::RingingStop()
{
   this->killTimer(MyTimer);
   Scheduler->Abort();
   this->Reset();
}

void
MethodController::RingingRounds()
{
   // if we're not ringing, flip to rounds immediately
   if (!Ringing)
   {
      CurrentChange = change(CurrentChange.size());
      this->DisplayChange();
   }
   else
   {
      // We can call rounds at any time, and it affects the next scheduled row
      boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
      if (Now > Scheduler->NextRowStarts() - Scheduler->RowDuration() * CallWindowStart)
      {
	 // we can emit the call immediately
	 Scheduler->Call(StandRow - CallWindowStart, "rounds");
	 RoundsCalled = Committed;
      }
      else
      {
	 // we're not inside the call window yet, schedule the call for later
	 RoundsCalled = Pending;
      }
   }
}

void
MethodController::RingingStand()
{
   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   int NextRow = Scheduler->NumRows();
   // stand at handstroke
   if (NextRow % 2 == 0)
   {
      // see if we're inside the call window - we must be before the finish otherwise the
      // row would have been scheduled
      if (Now > Scheduler->NextRowStarts() - Scheduler->RowDuration() * CallWindowStart)
      {
	 // we can emit the call immediately
	 StandRow = NextRow;
	 Scheduler->Call(StandRow - CallWindowStart, "stand");
	 CallStand = false;
	 CommittedStand = true;
      }
      else
      {
	 // we're not inside the call window yet, schedule the call for later
	 StandRow = NextRow;
	 CallStand = true;
	 CommittedStand = false;
      }
   }
   else
   {
      // make sure we stand at handstroke
      ++NextRow;
      StandRow = NextRow;
      CallStand = true;
      CommittedStand = false;
   }
}

void
MethodController::CCButtonClicked(int n)
{
   if (ui->SelectCallChange->isChecked())
   {
      if (!Ringing)
      {
	 // if we're not ringing, do the swap immediately
	 std::swap(CurrentChange[n], CurrentChange[n+1]);
	 this->DisplayChange();
      }
      else
      {
	 CallChange[n] = Pending;
	 this->UpdateButtons();
      }
   }
   else if (ui->SelectDodge->isChecked())
   {
      if (!Ringing)
	 return;
      if (Dodge[n] == None)
	 Dodge[n] = Pending;
      else if (Dodge[n] == Current)
	 Dodge[n] = PendingStop;
      this->UpdateButtons();
   }
   else if (ui->SelectRightPlaces->isChecked())
   {
      if (!Ringing)
	 return;
      if (RightPlaces[n] == None)
	 RightPlaces[n] = Pending;
      else if (RightPlaces[n] == Current)
	 RightPlaces[n] = PendingStop;
      this->UpdateButtons();
   }
   else if (ui->SelectWrongPlaces->isChecked())
   {
      if (!Ringing)
	 return;
      if (WrongPlaces[n] == None)
	 WrongPlaces[n] = Pending;
      else if (WrongPlaces[n] == Current)
	 WrongPlaces[n] = PendingStop;
      this->UpdateButtons();
   }
}

void
MethodController::CallClicked(int n)
{
   int OldCall = CurrentCallNum;
   CurrentCallNum = n;
   if (CurrentCallNum != OldCall)
   {
      MethodCallButton[OldCall]->setChecked(false);
   }
   CurrentCallNum = n;
}

void MethodController::CallModeChanged()
{
   this->UpdateButtons();
}

void MethodController::SelectMethodPushed()
{
   if (!SelectMethod)
      SelectMethod = new SelectMethodDialog();

   int r = SelectMethod->exec();
   if (r != 1)
      return;

   NextMethod = SelectMethod->SelectedMethod();

   // if we're not currently ringing then set the number of bells
   if (NextMethod && !Ringing)
   {
      int NumBells = NextMethod->stage();
      if (NumBells % 2 == 1 && ui->MethodTenorBehind->isChecked())
	 ++NumBells;
      ui->NumberOfBells->setValue(NumBells);
   }

   this->UpdateNextMethod();
   this->UpdateAvailableCalls();
}

void
MethodController::SetCurrentMethod(method* m)
{
   CurrentMethod = m;
   if (CurrentMethod)
   {
      ui->CurrentMethod->setText(QString::fromStdString(CurrentMethod->full_name_stage()));
   }
   else
   {
      ui->CurrentMethod->setText("");
   }
   this->UpdateAvailableCalls();
}

void MethodController::UpdateAvailableCalls()
{
   // make sure we preserve the currently selected call (if there is one)
   call OldCall;
   if (MethodCallButton[CurrentCallNum]->isChecked() && CurrentCallNum < int(AvailableCalls.size()))
   {
      OldCall = AvailableCalls[CurrentCallNum];
   }

   // get the new list of available calls, from the current method plus the next method
   AvailableCalls = std::vector<call>();
   if (CurrentMethod)
      AvailableCalls = CurrentMethod->all_calls();
   if (NextMethod)
   {
      // add non-duplicate calls
      std::vector<call> Temp(NextMethod->all_calls());
      for (unsigned i = 0; i < Temp.size(); ++i)
      {
	 if (std::find(AvailableCalls.begin(), AvailableCalls.end(), Temp[i]) == AvailableCalls.end())
	    AvailableCalls.push_back(Temp[i]);
      }
   }

   // Update the buttons for the available calls
   if (AvailableCalls.empty())
   {
      // If there are no available calls, default to 'bob' and 'single', but don't enable them
      MethodCallButton[0]->setText("Bob");
      MethodCallButton[0]->setChecked(false);
      MethodCallButton[0]->setEnabled(false);
      MethodCallButton[0]->setVisible(true);
      MethodCallButton[1]->setText("Single");
      MethodCallButton[1]->setChecked(false);
      MethodCallButton[1]->setEnabled(false);
      MethodCallButton[1]->setVisible(true);

      for (int i = 2; i < MaxCalls; ++i)
      {
	 MethodCallButton[i]->setChecked(false);
	 MethodCallButton[i]->setEnabled(false);
	 MethodCallButton[i]->setVisible(false);
      }
   }
   else
   {
      for (unsigned i = 0; i < std::min(AvailableCalls.size(), std::size_t(MaxCalls)); ++i)
      {
	 MethodCallButton[i]->setText(QString::fromStdString(AvailableCalls[i].Name));
	 MethodCallButton[i]->setChecked(AvailableCalls[i] == OldCall);
	 MethodCallButton[i]->setEnabled(true);
	 MethodCallButton[i]->setVisible(true);

      }
      for (int i = AvailableCalls.size(); i < MaxCalls; ++i)
      {
	 MethodCallButton[i]->setChecked(false);
	 MethodCallButton[i]->setEnabled(false);
	 MethodCallButton[i]->setVisible(false);
      }
   }   
}

void MethodController::MethodGoPushed()
{
   // Make sure that it really is valid to start this method
   if (!Ringing || RingingMethod || !NextMethod || NextMethod->stage() > this->MaxChangeRingingBells()
       || CommittedStand)
      return;

   CallStand = false;  // if we've got an auto-stand then cancel it

   this->SetCurrentMethod(NextMethod);

   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   int NextRow = Scheduler->NumRows();
   // go at handstroke
   if (NextRow % 2 == 0)
   {
      // see if we're inside the call window - we must be before the finish otherwise the
      // row would have been scheduled
      if (Now > Scheduler->NextRowStarts() - Scheduler->RowDuration() * CallWindowStart)
      {
	 // we can emit the call immediately
	 GoRow = NextRow;
	 Scheduler->CallGoMethod(NextRow - CallWindowMiddle, CurrentMethod->full_name_stage());
	 this->CommitGoMethod();
      }
      else
      {
	 // we're not inside the call window yet, schedule the call for later
	 GoRow = NextRow;
	 GoCalled = Pending;
      }
   }
   else
   {
      // make sure we stand at handstroke
      ++NextRow;
      GoRow = NextRow;
      GoCalled = Pending;
   }

   this->UiRinging(); // to update the go buttons
}

void MethodController::MethodGoDelayToggled(bool Checked)
{
   // Are we cancelling an existing go request?
   if (!Checked)
   {
      if (GoCalled == Pending)
      {
	 GoCalled = None;
	 this->SetCurrentMethod(NULL);
	 this->UiRinging(); // to update the go buttons
      }
      return;
   }
   
   // else this is a new request

   // Make sure that it really is valid to start this method
   if (!NextMethod || NextMethod->stage() > this->MaxChangeRingingBells())
      return;

   // It is possible to press this button when we are not yet ringing, in which 
   // case start ringing now
   if (!Ringing)
   {
      this->RingingStart();
      GoRow = ui->MethodGoAfter->value()*2;
   }
   else
      GoRow = Scheduler->NumRows() + ui->MethodGoAfter->value()*2;

   // go at handstroke only
   if (GoRow % 2 == 1)
      ++GoRow;

   GoCalled = Pending;
   this->SetCurrentMethod(NextMethod);
   this->UiRinging(); // to update the go buttons
}

void MethodController::UpdateNextMethod()
{
   if (NextMethod)
   {
      ui->NextMethod->setText(QString::fromStdString(NextMethod->full_name_stage()));
      if (Ringing)
      {
	 if (RingingMethod)
	 {
	    // See if the splice is valid
	    ui->MethodSplice->setEnabled(this->MaxChangeRingingBells() >= NextMethod->stage());
	 }
	 else
	 {
	    // we're ringing call changes.  See if we can 'go' into the method
	    int MaxAvail = this->MaxChangeRingingBells();
	    ui->MethodGo->setEnabled(MaxAvail >= NextMethod->stage());
	    ui->MethodGoDelay->setEnabled(MaxAvail >= NextMethod->stage());
	 }
      }
      else
      {
	 // we're not ringing.  If the number of bells is compatible then we could 'go with delay'.
	 ui->MethodGoDelay->setEnabled(CurrentChange.size() >= NextMethod->stage());
      }
   }
   else
   {
      ui->NextMethod->setText("");
      ui->MethodGo->setEnabled(false);
      ui->MethodGoDelay->setEnabled(false);
      ui->MethodSplice->setEnabled(false);
   }
}

void
MethodController::UiStanding()
{
   ui->RingingStand->setEnabled(false);
   ui->RingingStop->setEnabled(false);

   ui->RingingRounds->setEnabled(true);

   ui->NumberOfBells->setEnabled(true);
   ui->RingingStart->setEnabled(true);

   ui->MethodGo->setEnabled(false);
   ui->MethodGoDelay->setEnabled(NextMethod && this->MaxChangeRingingBells() >= NextMethod->stage());
   ui->MethodGoDelay->setChecked(false);
   ui->MethodSplice->setEnabled(false);
   ui->MethodThatsAll->setEnabled(false);
   ui->MethodThatsAll->setChecked(false);
}

void
MethodController::UiRinging()
{
   ui->RingingStand->setEnabled(true);
   ui->RingingStop->setEnabled(true);

   ui->RingingRounds->setEnabled(true);

   ui->NumberOfBells->setEnabled(false);
   ui->RingingStart->setEnabled(false);

   ui->MethodGo->setEnabled(GoCalled == None && 
			    NextMethod && this->MaxChangeRingingBells() >= NextMethod->stage());
   ui->MethodGoDelay->setEnabled((GoCalled == None || GoCalled == Pending) &&
				 NextMethod && this->MaxChangeRingingBells() >= NextMethod->stage());
   ui->MethodSplice->setEnabled(false);
   ui->MethodThatsAll->setEnabled(false);
   ui->MethodThatsAll->setChecked(false);
}

void
MethodController::UiRingingMethod()
{
   CHECK(CurrentMethod);

   ui->RingingStand->setEnabled(true);
   ui->RingingStop->setEnabled(true);

   ui->RingingRounds->setEnabled(true);

   ui->NumberOfBells->setEnabled(false);
   ui->RingingStart->setEnabled(false);

   ui->MethodGo->setEnabled(false);
   ui->MethodGoDelay->setEnabled(false);
   ui->MethodGoDelay->setChecked(false);
   ui->MethodSplice->setEnabled(NextMethod && this->MaxChangeRingingBells() >= NextMethod->stage());
   ui->MethodThatsAll->setEnabled(true);
}

int
MethodController::MaxChangeRingingBells() const
{
   int n = 0;
   while (n < CurrentChange.size() && Dodge[n] == None && RightPlaces[n] == None && WrongPlaces[n] == None
	  && CallChange[n] == None)
      ++n;
   return n;
}
