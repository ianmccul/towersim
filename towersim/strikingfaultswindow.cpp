// -*- C++ -*- $Id: strikingfaultswindow.cpp 966 2012-06-17 12:05:28Z ianmcc $

#include "strikingfaultswindow.h"
#include "ui_ui-bellsim-mainwindow.h"
#include "ringingscheduler.h"
#include <boost/lexical_cast.hpp>

int const MaxBells = 16;  // number of bells, as configured in UI

StrikingFaultsWindow::StrikingFaultsWindow(boost::shared_ptr<RingingScheduler> Scheduler_, 
                               Ui::MainWindow* MainWindow)
   : ui(MainWindow),
   Scheduler(Scheduler_)
{

   // assemble the arrays for the individual bells for the delay dock
   QWidget* w = ui->DelayDock->nextInFocusChain();
   HandLabel.resize(16);  // cant find a way to enumerate the labels in order
   BackLabel.resize(16);  // cant find a way to enumerate the labels in order
   StrikingLabel.resize(16);  // cant find a way to enumerate the labels in order
   // iterate until we get back to the parent
   while (w != ui->DelayDock)
   {
      if (w->objectName().startsWith("HandDelay"))
         HandDelay.push_back(qobject_cast<QSlider*>(w));
      else if (w->objectName().startsWith("SeparateBackDelay"))
         SeparateBackDelay.push_back(qobject_cast<QCheckBox*>(w));
      else if (w->objectName().startsWith("BackDelay"))
         BackDelay.push_back(qobject_cast<QSlider*>(w));
      else if (w->objectName().startsWith("HandLabel"))
      {
         std::string s = w->objectName().toStdString();
         s = std::string(s.begin()+9,s.end());
         int i = boost::lexical_cast<int>(s);
         HandLabel[i-1] = qobject_cast<QLabel*>(w);
      }
      else if (w->objectName().startsWith("BackLabel"))
      {
         std::string s = w->objectName().toStdString();
         s = std::string(s.begin()+9,s.end());
         int i = boost::lexical_cast<int>(s);
         BackLabel[i-1] = qobject_cast<QLabel*>(w);
      }
      else if (w->objectName().startsWith("StrikingLabel"))
      {
         std::string s = w->objectName().toStdString();
         s = std::string(s.begin()+13,s.end());
         int i = boost::lexical_cast<int>(s);
         StrikingLabel[i-1] = qobject_cast<QLabel*>(w);
      }
      w = w->nextInFocusChain();
   }
   CHECK_EQUAL(HandDelay.size(), MaxBells);
   CHECK_EQUAL(SeparateBackDelay.size(), MaxBells);
   CHECK_EQUAL(BackDelay.size(), MaxBells);
   CHECK_EQUAL(HandLabel.size(), MaxBells);
   CHECK_EQUAL(BackLabel.size(), MaxBells);
   CHECK_EQUAL(StrikingLabel.size(), MaxBells);

   // error dock widget arrays
   w = ui->ErrorDock->nextInFocusChain();
   LabelHandError.resize(16);  // cant find a way to enumerate the labels in order
   LabelBackError.resize(16);  // cant find a way to enumerate the labels in order
   StrikingErrorLabel.resize(16);  // cant find a way to enumerate the labels in order
   while (w != ui->ErrorDock)
   {
      if (w->objectName().startsWith("HandError"))
         HandError.push_back(qobject_cast<QSlider*>(w));
      else if (w->objectName().startsWith("SeparateBackError"))
         SeparateBackError.push_back(qobject_cast<QCheckBox*>(w));
      else if (w->objectName().startsWith("BackError"))
         BackError.push_back(qobject_cast<QSlider*>(w));
      else if (w->objectName().startsWith("LabelHandError"))
      {
         std::string s = w->objectName().toStdString();
         s = std::string(s.begin()+14,s.end());
         int i = boost::lexical_cast<int>(s);
         LabelHandError[i-1] = qobject_cast<QLabel*>(w);
      }
      else if (w->objectName().startsWith("LabelBackError"))
      {
         std::string s = w->objectName().toStdString();
         s = std::string(s.begin()+14,s.end());
         int i = boost::lexical_cast<int>(s);
         LabelBackError[i-1] = qobject_cast<QLabel*>(w);
      }
      else if (w->objectName().startsWith("StrikingErrorLabel"))
      {
         std::string s = w->objectName().toStdString();
         s = std::string(s.begin()+18,s.end());
         int i = boost::lexical_cast<int>(s);
         StrikingErrorLabel[i-1] = qobject_cast<QLabel*>(w);
      }
      w = w->nextInFocusChain();
   }
   CHECK_EQUAL(HandError.size(), MaxBells);
   CHECK_EQUAL(SeparateBackError.size(), MaxBells);
   CHECK_EQUAL(BackError.size(), MaxBells);
   CHECK_EQUAL(LabelHandError.size(), MaxBells);
   CHECK_EQUAL(LabelBackError.size(), MaxBells);
   CHECK_EQUAL(StrikingErrorLabel.size(), MaxBells);

   NumBellsShown = 0;
   this->IncreaseNumBells(8);

   connect(ui->OverallHandError, SIGNAL(valueChanged(int)), this, 
           SLOT(OverallHandErrorValueChanged(int)));
   connect(ui->OverallSeparateBackError, SIGNAL(clicked(bool)), this, 
           SLOT(OverallSeparateBackErrorClicked(bool)));
   connect(ui->OverallBackError, SIGNAL(valueChanged(int)), this, 
           SLOT(OverallBackErrorValueChanged(int)));
   
   connect(ui->SensorErrorMultiplier, SIGNAL(valueChanged(int)), this, 
           SLOT(SensorErrorMultiplierValueChanged(int)));

   connect(ui->ResetBellDelay, SIGNAL(clicked()), this,
           SLOT(ResetBellDelay()));
   connect(ui->ResetBellError, SIGNAL(clicked()), this,
           SLOT(ResetBellError()));

   connect(ui->RingingSpeedFromSensors, SIGNAL(valueChanged(int)), this,
           SLOT(RingingSpeedFromSensorsValueChanged(int)));
   connect(ui->SensorErrorMultiplier, SIGNAL(valueChanged(int)), this,
           SLOT(SensorErrorMultiplierValueChanged(int)));

   for (int i = 0; i < MaxBells; ++i)
   {
      connect(HandDelay[i], SIGNAL(valueChanged(int)), this, 
              SLOT(HandDelayValueChanged(int)));
      connect(SeparateBackDelay[i], SIGNAL(clicked(bool)), this,
              SLOT(SeparateBackDelayClicked(bool)));
      connect(BackDelay[i], SIGNAL(valueChanged(int)), this, 
              SLOT(BackDelayValueChanged(int)));

      connect(HandError[i], SIGNAL(valueChanged(int)), this, 
              SLOT(HandErrorValueChanged(int)));
      connect(SeparateBackError[i], SIGNAL(clicked(bool)), this,
              SLOT(SeparateBackErrorClicked(bool)));
      connect(BackError[i], SIGNAL(valueChanged(int)), this, 
              SLOT(BackErrorValueChanged(int)));
   }

   Scheduler->SetUniformDistribution();
   ui->UniformDistribution->click();

   connect(ui->UniformDistribution, SIGNAL(clicked()), this,
           SLOT(UniformDistributionClicked()));
   connect(ui->GaussianDistribution, SIGNAL(clicked()), this,
           SLOT(GaussianDistributionClicked()));
}

void
StrikingFaultsWindow::IncreaseNumBells(int n)
{
   if (n <= NumBellsShown)
      return;

   NumBellsShown = n;
   for (int i = 0; i < MaxBells; ++i)
   {
      // delay dock
      StrikingLabel[i]->setVisible(i < NumBellsShown);
      HandDelay[i]->setVisible(i < NumBellsShown);
      HandLabel[i]->setVisible(i < NumBellsShown);
      SeparateBackDelay[i]->setVisible(i < NumBellsShown);
      BackDelay[i]->setVisible(i < NumBellsShown);
      BackLabel[i]->setVisible(i < NumBellsShown);
      ui->DelayDockLayout->setRowStretch(i+1, i < NumBellsShown);

      // error dock
      StrikingErrorLabel[i]->setVisible(i < NumBellsShown);
      HandError[i]->setVisible(i < NumBellsShown);
      LabelHandError[i]->setVisible(i < NumBellsShown);
      SeparateBackError[i]->setVisible(i < NumBellsShown);
      BackError[i]->setVisible(i < NumBellsShown);
      LabelBackError[i]->setVisible(i < NumBellsShown);
      ui->ErrorDockLayout->setRowStretch(i+4, i < NumBellsShown);
   }
   ui->ErrorDockLayout->setRowStretch(0, 0);
   ui->ErrorDockLayout->setRowStretch(1, 0);
   ui->ErrorDockLayout->setRowStretch(2, 0);
   ui->ErrorDockLayout->setRowStretch(3, 0);
}

//
// slots
//

// bell errors

void
StrikingFaultsWindow::UniformDistributionClicked()
{
   Scheduler->SetUniformDistribution();
}

void
StrikingFaultsWindow::GaussianDistributionClicked()
{
   Scheduler->SetGaussianDistribution();
}

void
StrikingFaultsWindow::OverallHandErrorValueChanged(int Value)
{
   // is the backstroke error tied to handstroke?
   if (!ui->OverallSeparateBackError->isChecked())
      ui->OverallBackError->setValue(Value);
   Scheduler->SetHandstrokeStrikingError(Value * 0.01);
}

void
StrikingFaultsWindow::OverallSeparateBackErrorClicked(bool Clicked)
{
   // if we're tieing hand and back errors again, then set
   // the backstroke error to what we've currently got for handstroke
   if (!Clicked)
      ui->OverallBackError->setValue(ui->OverallHandError->value());
}

void
StrikingFaultsWindow::OverallBackErrorValueChanged(int Value)
{
   // is the backstroke error tied to handstroke?
   if (!ui->OverallSeparateBackError->isChecked())
      ui->OverallHandError->setValue(Value);
   Scheduler->SetBackstrokeStrikingError(Value * 0.01);
}

void
StrikingFaultsWindow::HandErrorValueChanged(int Value)
{
   int Bell = this->FindSenderBell(HandError, "HandError");
   // is the backstroke delay of the bell tied to the handstroke?
   if (!SeparateBackError[Bell]->isChecked())
      BackError[Bell]->setValue(Value);
   Scheduler->SetBellHandstrokeStrikingError(Bell+1, Value * 0.01);
}
      
void
StrikingFaultsWindow::SeparateBackErrorClicked(bool Clicked)
{
   int Bell = this->FindSenderBell(SeparateBackError, "SeparateBackError");
   if (!Clicked)
   {
      BackError[Bell]->setValue(HandError[Bell]->value());
   }
}
      
void
StrikingFaultsWindow::BackErrorValueChanged(int Value)
{
   int Bell = this->FindSenderBell(BackError, "BackError");
   // is the backstroke error of the bell tied to the handstroke?
   if (!SeparateBackError[Bell]->isChecked())
      HandError[Bell]->setValue(Value);
   Scheduler->SetBellBackstrokeStrikingError(Bell+1, Value * 0.01);
}
     
void
StrikingFaultsWindow::ResetBellError()
{
   // set all errors to zero.
   // Do we want to clear the SeparateBell check boxes too???
   for (int i = 0; i < MaxBells; ++i)
   {
      HandError[i]->setValue(0);
      BackError[i]->setValue(0);
   }
   ui->OverallHandError->setValue(0);
   ui->OverallBackError->setValue(0);
}

// delays

void
StrikingFaultsWindow::HandDelayValueChanged(int Value)
{
   int Bell = this->FindSenderBell(HandDelay, "HandDelay");
   // is the backstroke delay of the bell tied to the handstroke?
   if (!SeparateBackDelay[Bell]->isChecked())
      BackDelay[Bell]->setValue(Value);
   Scheduler->SetBellHandstrokeStrikingDelay(Bell+1, Value * 0.01);
}

void
StrikingFaultsWindow::SeparateBackDelayClicked(bool Clicked)
{
   int Bell = this->FindSenderBell(SeparateBackDelay, "SeparateBackDelay");
   if (!Clicked)
   {
      BackDelay[Bell]->setValue(HandDelay[Bell]->value());
   }
}

void
StrikingFaultsWindow::BackDelayValueChanged(int Value)
{
   int Bell = this->FindSenderBell(BackDelay, "BackDelay");
   // is the backstroke error of the bell tied to the handstroke?
   if (!SeparateBackDelay[Bell]->isChecked())
      HandDelay[Bell]->setValue(Value);
   Scheduler->SetBellBackstrokeStrikingDelay(Bell+1, Value * 0.01);
}

void
StrikingFaultsWindow::ResetBellDelay()
{
   // set all delays to zero.
   // Do we want to clear the SeparateBell check boxes too???
   for (int i = 0; i < MaxBells; ++i)
   {
      HandDelay[i]->setValue(0);
      BackDelay[i]->setValue(0);
   }
}

// sensor feedback slots

void
StrikingFaultsWindow::SensorErrorMultiplierValueChanged(int Value)
{
   Scheduler->SetSensorErrorMultiplier(Value * 0.01);
}

void
StrikingFaultsWindow::RingingSpeedFromSensorsValueChanged(int Value)
{
}
