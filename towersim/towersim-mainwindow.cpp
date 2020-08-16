// -*- C++ -*- $Id: towersim-mainwindow.cpp 1778 2016-08-29 08:34:22Z uqimccul $

#include "towersim-mainwindow.h"
#include "ui_ui-towersim-mainwindow.h"
#include "sensorinputserver.h"
#include <QMessageBox>
#include <QString>
#include <cmath>
#include "common/trace.h"

#if 0
void
SensorBoxReceiverType::IndexChanged(int NewIndex)
{
   if (Index != 0)
      Parent->DisassociateSensor(Bell);

   Index = NewIndex;
   if (Index != 0)
      Parent->AssociateSensor(Bell);
}
#endif

struct StandardOutputSink
{
   template <typename T>
   void operator()(T const& c)
   {
      std::cout << c << std::flush;
   }
};

TowerSimMainWindow::TowerSimMainWindow(QWidget* Parent)
   : QMainWindow(Parent), 
     ui(new Ui::MainWindow), 
     ProtectModifySpeed(false)
{
   ui->setupUi(this);

   boost::shared_ptr<RingingServer> Server = 
      boost::shared_ptr<RingingServer>(new RingingServer());
   Scheduler = boost::shared_ptr<RingingScheduler>(new RingingScheduler(Server));
   Scheduler->SetNumberOfBells(8);  // some default

   //   this->tabifyDockWidget(ui->ErrorDock, ui->DelayDock);
   //this->tabifyDockWidget(ui->FeedbackDock, ui->ErrorDock);

   connect(ui->ChangesPerMinute, SIGNAL(valueChanged(double)),
           this, SLOT(ChangesPerMinute_valueChanged(double)));
   connect(ui->PealSpeed, SIGNAL(timeChanged(QTime)),
           this, SLOT(PealSpeed_timeChanged(QTime)));

   connect(ui->NumberOfBells, SIGNAL(valueChanged(int)),
           this, SLOT(NumberOfBells_valueChanged(int)));

   // setup the tied changes-per-minute and peal speed controls
   ChangesPerMinute = ui->ChangesPerMinute->value();
   this->ChangesPerMinute_valueChanged(ChangesPerMinute);

   MController = new MethodController(Scheduler, ui);

   SensorInput = new SensorInputServer(this);
   SensorInput->ListenForConnections(5701);

   char const* Host = getenv("TOWERSIM_REMOTE_IP");
   if (!Host)
      Host = "localhost";

   SensorInput->ConnectTo(QString(Host), 5700, true);

   StatisticsWin = new StatisticsWindow(Scheduler, ui);
   SensorWindow = new SensorTab(Scheduler, ui);

   // Emit output commands from the RingingServer to the striking graphic
   Server->RegisterCommandVisitor(ui->StrikingGraphic);
   Server->RegisterCommandVisitor(new StandardOutputSink());

   ui->StrikingGraphic->ShowRealTime(true);
}

TowerSimMainWindow::~TowerSimMainWindow()
{
   delete ui;
   delete MController;
   delete SensorInput;
   delete SensorWindow;
}

// slots

void 
TowerSimMainWindow::ChangesPerMinute_valueChanged(double Value)
{
   // avoid recursion
   if (ProtectModifySpeed)
      return;

   ChangesPerMinute = Value;
   Scheduler->SetChangesPerMinute(ChangesPerMinute);

   this->UpdateBellGap();

   // calculate the new peal speed
   double PealSpeed = 5040.0 / Value; // peal speed in minutes
   int Hours = std::floor(PealSpeed / 60);
   PealSpeed -= Hours*60;
   int Minutes = std::floor(PealSpeed);
   PealSpeed -= Minutes;
   int Seconds = std::floor(PealSpeed*60 + 0.5);

   QTime PealTime = QTime(Hours, Minutes, Seconds);
   
   ProtectModifySpeed = true;
   ui->PealSpeed->setTime(PealTime);
   ProtectModifySpeed = false;
}

void 
TowerSimMainWindow::PealSpeed_timeChanged(QTime Time)
{
   // avoid recursion
   if (ProtectModifySpeed)
      return;

   // calcuate the new changes per minute
   double Minutes = Time.hour()*60.0 + Time.minute() + Time.second()/60.0;
   ChangesPerMinute = 5040.0 / Minutes;

   // don't exceed the limiting changes per minute
   double MinChangesPerMinute = ui->ChangesPerMinute->minimum();
   double MaxChangesPerMinute = ui->ChangesPerMinute->maximum();

   if (ChangesPerMinute < MinChangesPerMinute)
   {
      ChangesPerMinute = MinChangesPerMinute;
      this->ChangesPerMinute_valueChanged(ChangesPerMinute);
   }
   else if (ChangesPerMinute > MaxChangesPerMinute)
   {
      ChangesPerMinute = MaxChangesPerMinute;
      this->ChangesPerMinute_valueChanged(ChangesPerMinute);
   }

   Scheduler->SetChangesPerMinute(ChangesPerMinute);

   this->UpdateBellGap();

   ProtectModifySpeed = true;
   ui->ChangesPerMinute->setValue(ChangesPerMinute);
   ProtectModifySpeed = false;
}

void
TowerSimMainWindow::NumberOfBells_valueChanged(int)
{
   this->UpdateBellGap();
}

void
TowerSimMainWindow::UpdateBellGap()
{
   std::string Gap = boost::lexical_cast<std::string>(int(Scheduler->Gap().total_milliseconds() + 0.5));
   Gap += "ms";
   ui->BellGap->setText(QString::fromStdString(Gap));
}

void
TowerSimMainWindow::AssociateSensor(std::string const& Sensor, int Bell)
{
   //std::string Sensor = SensorBoxes[Bell-1]->currentText().toStdString();
   TRACE("Associate")(Sensor)(Bell);
   Scheduler->GetServer()->AssociateSensor(Sensor, Bell);
}

void
TowerSimMainWindow::DisassociateSensor(int Bell)
{
   Scheduler->GetServer()->DisassociateSensor(Bell);
}

void
TowerSimMainWindow::UpdateSensorStatistics()
{
   if (Scheduler->IsRunning())
      StatisticsWin->Update();
}

void
TowerSimMainWindow::AttachSensor(std::string const& SensorName)
{
   SensorWindow->AttachSensor(SensorName);
}

void
TowerSimMainWindow::AttachSensor(::AttachSensor const& a)
{
   SensorWindow->AttachSensor(a.Sensor);
   Scheduler->SetSensorAdjust(a.Sensor, a.HandAdjust, a.BackAdjust);
}

void
TowerSimMainWindow::DetachSensor(std::string const& SensorName)
{
   SensorWindow->DetachSensor(SensorName);
}

