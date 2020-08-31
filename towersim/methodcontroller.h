// -*- C++ -*- $Id: methodcontroller.h 987 2012-07-15 01:58:28Z uqimccul $
//
// methodcontroller.h
// Manages call changes, dodges, and ringing
// methods and touches.  This sits between the user interface
// and the RingingScheduler.

#if !defined(METHODCONTROLLER_H_DASHFSKJHY894Y89HPO)
#define METHODCONTROLLER_H_DASHFSKJHY894Y89HPO

#include "ringingscheduler.h"
#include "change/method.h"
#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <vector>

int const MaxBells = 16;
int const MaxCalls = 4;    // maximum number of different calls supported by the ui

namespace Ui
{
   class MainWindow;
} // namespace Ui

class SelectMethodDialog;

// Start and end of the "call window".  This is the time
// at which it is valid to make a call that would affect
// a given row.  The times are measured in units of the
// number of rows prior to the start of the change.
// Scheduled calls are made at CallWindowMiddle.
extern double CallWindowStart;
extern double CallWindowFinish;
extern double CallWindowMiddle;

// The state that a call can be in
enum CallState {None, Pending, Committed, Current, PendingStop, CommittedStop};

class MethodController : public QObject
{
   Q_OBJECT

   public:
      MethodController(boost::shared_ptr<RingingScheduler> Scheduler_, 
		       Ui::MainWindow* MainWindow);

   protected:
      void timerEvent(QTimerEvent *event);

   private slots:
      void NumberOfBells_valueChanged(int Value);
      void RingingStart();
      void RingingAuto();
      void RingingStop();
      void RingingRounds();
      void RingingStand();
      void CCButtonClicked(int n);
      void CallModeChanged();

      void SelectMethodPushed();
      void MethodGoPushed();
      void MethodGoDelayToggled(bool Checked);
      void CallClicked(int n);

   private:
      void UiStanding(); // set enabled/disabled for 'standing' bells
      void UiRinging(); // set enabled/disabled for 'ringing' call changes
      void UiRingingMethod(); // set enabled/disabled for 'ringing' a method
      void DisplayChange(); // displays CurrentChange in the window,
                            // doesn't update the visibility
      void UpdateButtons(); // update the enabled state on the buttons
      // Reset the state back to doing nothing, eg at the end of ringing
      void Reset(bool ResetCurrentChange = true); 
      void UpdateNextMethod(); // update the display of the next method
      void SetCurrentMethod(method* m); // set the CurrentMethod, and update the UI
      void UpdateAvailableCalls(); // set the tiles on the call buttons based on CurrentMethod and NextMethod
      void CommitGoMethod(); // used to set the GoMethod = Committed state, and resets the cursor

      // if we are not ringing a method, then return the current number of bells.
      // if we are ringing, but not ringing a method, then return
      // the maximum stage of bells that can fit in amongst the non-dodging bells.
      // If we are ringing a method, then return the number of bells participating
      // in the change-ringing plus the number of fixed tenors behind.
      // This is the maximum stage of ringing that we could start (or splice).
      int MaxChangeRingingBells() const;

      Ui::MainWindow* ui;
      boost::shared_ptr<RingingScheduler> Scheduler;

      // the labels to indicate the current change
      std::vector<QLabel*> CCLabel;
      // the labels to indicate 
      std::vector<QPushButton*> CCButton;

      int MyTimer;

      // true if we are currently ringing changes
      bool Ringing;

      // the change that we are currently displaying
      change CurrentChange;

      // The row number of the currently displayed row
      int DisplayedRow;

      // the final row which won't get scheduled because we've called stand
      int StandRow;
 
      // if true, "stand" has been called
      bool CommittedStand;

      // if true, then we should call "stand" at an appropriate time for the StandRow
      bool CallStand;

      // if Rounds has been called
      CallState RoundsCalled;

      // The number of the current call (index into the AvailableCalls array)
      int CurrentCallNum;
      std::vector<call> AvailableCalls;
      std::vector<QPushButton*> MethodCallButton;     // convenience array of call pushbuttons

      // for each pair 0 - 14, indicate if it is currently participating in dodging
      std::vector<CallState> Dodge;

      // for each pair 0 - 14, indicate if it is currently participating in right-way places
      std::vector<CallState> RightPlaces;

      // for each pair 0 - 14, indicate if it is currently participating in wrong-way places
      std::vector<CallState> WrongPlaces;

      // Current means call changes that have been scheduled but haven't sounded yet.
      // This usage is slightly different to the CurrentDodge flags
      std::vector<CallState> CallChange;

      bool RingingMethod;     // true if we are currently ringing a method
      CallState GoCalled;     // keep track of "Go" and "That's All" calls
      int GoRow;              // the row that will be affected by the "Go" call

      method* CurrentMethod;
      method_cursor Cursor;
      change StartChange;     // we might start the method not from rounds and on more bells, so keep track of it
      call NextCall;          // the call that will affect the next change, once its committed

      method* NextMethod;

      int NumCourses;  // number of courses rung of the method

      SelectMethodDialog* SelectMethod;
};

#endif
