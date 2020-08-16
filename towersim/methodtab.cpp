// -*- C++ -*- $Id: methodtab.cpp 967 2012-06-24 12:26:14Z uqimccul $

#include "methodtab.h"
#include "ui_ui-bellsim-mainwindow.h"
#include "ringingscheduler.h"
#include <fstream>
//#include "../loadmethod.h"

std::string const Directory = "/home/ian/svn/work/ringing/src/sim/bellsim/";

double const CallWindowStart = 2.2;   // number of rows prior to the start of a change that we can put in a call
double const CallWindowFinish = 1.3;   // number of rows prior to the start of a change that we can put in a call


void LoadMethods(QStandardItem* Parent, int Stage, std::string const& Label,
                 std::string const& Class,
                 std::string const& File,
                 MethodTab* Tab = NULL)
{
   QStandardItem* Group;
   if (Label.empty())
      Group = Parent;
   else
   {
      Group = new QStandardItem(QString::fromStdString(Label));
      Group->setSelectable(false);
      Parent->appendRow(Group);
   }
}

MethodTab::MethodTab(boost::shared_ptr<RingingScheduler> Scheduler_, 
                     Ui::MainWindow* MainWindow)
   : ui(MainWindow),
     Scheduler(Scheduler_),
     MethodTree(MainWindow->MethodTree),
     MethodTreeModel(),
     CurrentMethod(NULL),
     MyTimer(-1)
{
   RandomSpliced = false;
   
   QStandardItem* Root = MethodTreeModel.invisibleRootItem();
   Favourites = new QStandardItem(QString("Favourites"));
   Root->appendRow(Favourites);

   for (int Bells = 4; Bells <= 12; ++Bells)
   {
      Methods[Bells] = new QStandardItem(QString(stage_name(Bells).c_str()));
      Methods[Bells]->setSelectable(false);
      Root->appendRow(Methods[Bells]);
   }

   LoadMethods(Methods[4], 4, "Plain", "", "methods/plain4.txt");
   LoadMethods(Methods[5], 5, "Plain", "", "methods/plain5.txt");
   LoadMethods(Methods[6], 6, "Plain", "", "methods/plain6.txt");
   LoadMethods(Methods[7], 7, "Plain", "", "methods/plain7.txt");
   LoadMethods(Methods[8], 8, "Plain", "", "methods/plain8.txt");
   LoadMethods(Methods[9], 9, "Plain", "", "methods/plain9.txt");
   LoadMethods(Methods[10], 10, "Plain", "", "methods/plain10.txt");
   LoadMethods(Methods[11], 11, "Plain", "", "methods/plain11.txt");
   LoadMethods(Methods[12], 12, "Plain", "", "methods/plain12.txt");

   LoadMethods(Methods[4], 4, "Principles", "", "methods/prin4.txt");
   LoadMethods(Methods[5], 5, "Principles", "", "methods/prin5.txt");
   LoadMethods(Methods[6], 6, "Principles", "", "methods/prin6.txt");
   LoadMethods(Methods[7], 7, "Principles", "", "methods/prin7.txt");
   LoadMethods(Methods[8], 8, "Principles", "", "methods/prin8.txt");
   LoadMethods(Methods[9], 9, "Principles", "", "methods/prin9.txt");
   LoadMethods(Methods[10], 10, "Principles", "", "methods/prin10.txt");
   LoadMethods(Methods[11], 11, "Principles", "", "methods/prin11.txt");
   LoadMethods(Methods[12], 12, "Principles", "", "methods/prin12.txt");

   LoadMethods(Methods[6], 6, "Surprise", "Surprise", "methods/surp6.txt");
   LoadMethods(Methods[8], 8, "Surprise", "Surprise", "methods/surp8.txt", this);
   LoadMethods(Methods[10], 10, "Surprise", "Surprise", "methods/surp10.txt");
   LoadMethods(Methods[12], 12, "Surprise", "Surprise", "methods/surp12.txt");

   LoadMethods(Methods[6], 6, "Treble Bob", "Treble Bob", "methods/tb6.txt");
   LoadMethods(Methods[8], 8, "Treble Bob", "Treble Bob", "methods/tb8.txt");
   LoadMethods(Methods[10], 10, "Treble Bob", "Treble Bob", "methods/tb10.txt");
   LoadMethods(Methods[12], 12, "Treble Bob", "Treble Bob", "methods/tb12.txt");

   LoadMethods(Methods[6], 6, "Delight", "Delight", "methods/del6.txt");
   LoadMethods(Methods[8], 8, "Delight", "Delight", "methods/del8.txt");
   LoadMethods(Methods[10], 10, "Delight", "Delight", "methods/del10.txt");   
   LoadMethods(Methods[12], 12, "Delight", "Delight", "methods/del12.txt");

#if 0
   // load the methods
   QStandardItem* Item = new QStandardItem(QString("Surprise"));
   Item->setSelectable(false);
   Methods[6]->appendRow(Item);

#endif

   QStandardItem* M = new QStandardItem(QString("Training Doubles"));
   //   method* MyMethod = new method(method::create_regular("Trainging Doubles", 5, 
   //                                                        "5.1.5.1.5.1.5.1.5.1.5", 
   //                                                        "5"));
   //   MyMethod->set_full_name("Training Doubles");
   //   M->setData(QVariant::fromValue((void*)MyMethod));
   //   Methods[5]->appendRow(M);


   MethodTree->setModel(&MethodTreeModel);

   // pre-load some common .wav files
   Scheduler->LoadCall("go");
   Scheduler->LoadCall("stand");
   Scheduler->LoadCall("thatsall");

   connect(MethodTree, SIGNAL(clicked(QModelIndex)), this, SLOT(MethodClicked(QModelIndex)));
   connect(ui->MethodStart, SIGNAL(clicked()), this, SLOT(MethodStart()));
   connect(ui->MethodStand, SIGNAL(clicked()), this, SLOT(MethodStand()));
   connect(ui->MethodStop, SIGNAL(clicked()), this, SLOT(MethodStop()));
   connect(ui->MethodRounds, SIGNAL(clicked()), this, SLOT(MethodRounds()));
   connect(ui->MethodGo, SIGNAL(clicked()), this, SLOT(MethodGo()));
   connect(ui->MethodThatsAll, SIGNAL(clicked()), this, SLOT(MethodThatsAll()));

   connect(ui->MethodEightSpliced, SIGNAL(clicked()), this, SLOT(MethodStartRandomSpliced()));
}

void
MethodTab::MethodClicked(QModelIndex Index)
{
   method* NextMethod = (method*)(Index.data(Qt::UserRole+1).value<void*>());
   if (!NextMethod)
      return;

   if (!Scheduler->IsRunning())
   {
      CurrentMethod = NextMethod;
      Scheduler->SetNumberOfBells(NextMethod->stage(), ui->MethodTenorBehind->isChecked());
      this->UiStanding();
   }
   if (MethodRinging)
   {
      ui->MethodSplice->setEnabled((*NextMethod) != Cursor.get_method() 
                                   && NextMethod->stage() == Cursor.stage());
   }
   CurrentMethod = NextMethod;
}

void
MethodTab::MethodStart()
{
   RandomSpliced = false;

   CHECK(CurrentMethod != NULL)(CurrentMethod);
   ui->CurrentMethod->setText(QString::fromStdString(CurrentMethod->full_name()));
   Cursor.reset(*CurrentMethod);
   NumCourses = 0;
   StandRow = -1;
   GoRow = -1;
   if (ui->MethodGoAfterBox->isChecked())
   {
      GoRow = 2*ui->MethodGoAfter->value();
      CallGo = true; // we want the auto-call ui->AudibleCalls->isChecked();
   }
   MethodRinging = false;
   RowsRounds = 0;
   AllowAutoStart = true;
   ThatsAll = false;
   this->UiRinging();
   Scheduler->Begin(boost::posix_time::microsec_clock::universal_time(), change(Cursor.stage()),
                    ui->MethodTenorBehind->isChecked());
   boost::posix_time::ptime Next = Scheduler->NextRowStarts() - Scheduler->Gap() * 2;
   MyTimer = this->startTimer(50);
}

void
MethodTab::MethodStand()
{
   // determine which row this will affect
   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   int NextRow = Scheduler->NumRows();

   // NextRow is the earliest that this call can possibly affect
   // and we always can affect this row because we generate the next row at the end of the call window.

   StandRow = NextRow;
   if (StandRow % 2 == 1)    // make it a handstroke
      ++StandRow;

   CallStand = ui->AudibleCalls->isChecked();
   if (CallStand) // see if we should schedule the "stand" call immediately
   {
      double StandLoc = StandRow - CallWindowStart;
      if (int(floor(StandLoc)) <= NextRow)
      {
         Scheduler->Call(StandLoc, "stand");
         CallStand = false;
      }
   }
   //   this->UiGoing();
}

void
MethodTab::MethodRounds()
{
   MethodRinging = false;
   AllowAutoStart = false;
   ThatsAll = false;
   NumCourses = 0;
   this->UiRinging();
}

void
MethodTab::MethodStop()
{
   this->killTimer(MyTimer);
   Scheduler->Abort();
   this->UiStanding();
}

void
MethodTab::MethodGo()
{
   // determine which row this will affect
   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   int NextRow = Scheduler->NumRows();
   // NextRow is the earliest that this call can possibly affect
   // and we always can affect this row because we generate the next row at the end of the call window.

   // reset the method cursor
   CHECK(CurrentMethod);
   Cursor.reset(*CurrentMethod);

   GoRow = NextRow;
   if (GoRow % 2 == 1)    // make it a handstroke
      ++GoRow;

   CallGo = ui->AudibleCalls->isChecked();
   if (CallGo) // see if we should schedule the "go" call immediately
   {
      double GoLoc = GoRow - CallWindowStart;
      if (int(floor(GoLoc)) <= NextRow)
      {
         Scheduler->Call(GoLoc, "go", Cursor.get_method().name());
         CallGo = false;
      }
   }
   NumCourses = 0;
   CallThatsAll = ui->AudibleCalls->isChecked();
   this->UiGoing();
}

void
MethodTab::MethodThatsAll()
{
   // We don't put in the call ourselves, but wait for it to be scheduled
   ThatsAll = true;
   CallThatsAll = ui->AudibleCalls->isChecked();
}

void
MethodTab::timerEvent(QTimerEvent*)
{
   // current time
   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   // time that the next row (yet to be scheduled) will start
   boost::posix_time::ptime NextRowStarts = Scheduler->NextRowStarts();
   int NextRow = Scheduler->NumRows();

   // see if it is time to schedule the next change
   if (Now <= NextRowStarts - Scheduler->RowDuration() * CallWindowFinish)
      return;

   // Firstly, see if it is time for any audible calls.
   // "Stand" and "Go" affect handstrokes only.
   
   if (StandRow > 0 && CallStand)
   {
      double StandLoc = StandRow - CallWindowStart;
      if (int(floor(StandLoc)) <= NextRow)
      {
         Scheduler->Call(StandLoc, "stand");
         CallStand = false;
      }
   }

   if (GoRow > 0 && CallGo)
   {
      double GoLoc = GoRow - CallWindowStart;
      if (int(floor(GoLoc)) <= NextRow)
      {
         Scheduler->Call(GoLoc, "go", Cursor.get_method().name());
         CallGo = false;
      }
   }

   // firstly, check to see if 'stand' or 'go' has been called
   if (Scheduler->NumRows() % 2 == 0) // only stand or go at handstroke
   {
      if (StandRow == NextRow)
      {
         // wait plenty of time to stop
         if (boost::posix_time::microsec_clock::universal_time() > Scheduler->NextRowStarts() + Scheduler->Gap() * 2)
         {
            Scheduler->Finish();
            this->killTimer(MyTimer);
            this->UiStanding();
         }
         return;
      }
      if (!MethodRinging)
      {
         if (GoRow == NextRow)
         {
            // we're starting the method
            MethodRinging = true;
            GoRow = -1;
            this->UiGoing();
         }
      }
   }

   if (!MethodRinging)
   {
      // if the method isn't going then we're ringing rounds
      Scheduler->NextRow(change(Cursor.stage()));
      return;
   }



   // hack for random 8-spliced

   if (RandomSpliced)
   {
      if (Cursor.is_call_available_now())
      {
         bool Bob = random() % 2;
         if (Bob)
         {
            Scheduler->Call(NextRow - CallWindowFinish, "bob");
            Cursor.call('-');
         }
      }
      if (Cursor.is_block_begin())
      {
         if (FirstMethod)
            FirstMethod = false;
         else
         {
            // splice
            //if (++MethodNum == EightSpliced.size())
            //   MethodNum = 0;
            MethodNum = random() % EightSpliced.size();
            method* NewMethod = EightSpliced[MethodNum];
            if (NewMethod != CurrentMethod)
            {
               Scheduler->Call(NextRow - CallWindowFinish, NewMethod->name());
               Cursor.splice(*NewMethod);
               CurrentMethod = NewMethod;
               ui->CurrentMethod->setText(QString::fromStdString(CurrentMethod->full_name()));
            }
         }
      }
   }

   // end hack for random 8-spliced




   // otherwise we're ringing the method
   Scheduler->NextRow(*Cursor);
   if (Cursor.is_rounds())
   {
      // if we're back to rounds 
      ++NumCourses;
      if ((ui->MethodKeepGoing->value() > 0 && NumCourses >= ui->MethodKeepGoing->value()) || ThatsAll)
      {
         if (!ThatsAll || CallThatsAll)
         {
            // auto-generated, so make it audible
            Scheduler->Call(NextRow - CallWindowFinish, "thatsall");
            CallThatsAll = false;
         }
         MethodRinging = false;
         this->UiRinging();

         // see if we are auto-standing
         if (ui->MethodStandAfterBox->isChecked())
         {
            StandRow = NextRow + 2*ui->MethodStandAfter->value();
            if (StandRow % 2 == 1)  // only stand at handstroke
               ++StandRow;
            CallStand = true;
         }
         return;
      }
   }
      
   // next change in the method
   ++Cursor;
}

void
MethodTab::MethodStartRandomSpliced()
{
   srandom(clock());
   //MethodNum = 0;
   MethodNum = random() % EightSpliced.size();
   CurrentMethod = EightSpliced[MethodNum];
   ui->CurrentMethod->setText(QString::fromStdString(CurrentMethod->full_name()));
   Cursor.reset(*CurrentMethod);
   NumCourses = 0;
   StandRow = -1;
   GoRow = -1;
   FirstMethod = true;
   RandomSpliced = true;
   if (ui->MethodGoAfterBox->isChecked())
   {
      GoRow = 2*ui->MethodGoAfter->value();
      CallGo = true; // we want the auto-call ui->AudibleCalls->isChecked();
   }
   MethodRinging = false;
   RowsRounds = 0;
   AllowAutoStart = true;
   ThatsAll = false;
   this->UiRinging();
   Scheduler->Begin(boost::posix_time::microsec_clock::universal_time(), change(Cursor.stage()),
                    ui->MethodTenorBehind->isChecked());
   boost::posix_time::ptime Next = Scheduler->NextRowStarts() - Scheduler->Gap() * 2;
   MyTimer = this->startTimer(50);
}


void
MethodTab::UiNoMethod()
{
   ui->MethodStart->setDisabled(true);
   ui->MethodStand->setDisabled(true);
   ui->MethodRounds->setDisabled(true);
   ui->MethodGo->setDisabled(true);
   ui->MethodStop->setDisabled(true);
   ui->MethodThatsAll->setDisabled(true);
   ui->MethodSplice->setDisabled(true);
   ui->CurrentMethod->setText(QString());
}

void
MethodTab::UiStanding()
{
   ui->MethodStart->setEnabled(true);
   ui->MethodStand->setDisabled(true);
   ui->MethodRounds->setDisabled(true);
   ui->MethodGo->setDisabled(true);
   ui->MethodStop->setDisabled(true);
   ui->MethodThatsAll->setDisabled(true);
   ui->MethodSplice->setDisabled(true);
   ui->CurrentMethod->setText(QString());
   if (CurrentMethod)
      ui->CurrentMethod->setText(QString::fromStdString(CurrentMethod->full_name()));
   else
      ui->CurrentMethod->setText(QString());
}

void
MethodTab::UiRinging()
{
   ui->MethodStart->setDisabled(true);
   ui->MethodStand->setEnabled(true);
   ui->MethodRounds->setDisabled(true);
   ui->MethodGo->setEnabled(true);
   ui->MethodStop->setEnabled(true);
   ui->MethodThatsAll->setDisabled(true);
   ui->MethodSplice->setDisabled(true);
}

void
MethodTab::UiGoing()
{
   ui->MethodStart->setDisabled(true);
   ui->MethodStand->setEnabled(true);
   ui->MethodRounds->setEnabled(true);
   ui->MethodGo->setDisabled(true);
   ui->MethodStop->setEnabled(true);
   ui->MethodThatsAll->setEnabled(true);
   ui->MethodSplice->setDisabled(true);
}
