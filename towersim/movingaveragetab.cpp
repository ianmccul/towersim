// -*- C++ -*- $Id: movingaveragetab.cpp 966 2012-06-17 12:05:28Z ianmcc $

#include "movingaveragetab.h"
#include "common/statistics.h"
#include "movingaveragegraphicsview.h"
#include "ui_ui-analysis-dialog.h"

MovingAverageTab::MovingAverageTab(boost::shared_ptr<RingingScheduler> Scheduler_,
                       Ui::StrikingAnalysis* Dialog_)
   : Scheduler(Scheduler_),
     Dialog(Dialog_),
     ScalePercent(false)
{
   // populate the sensor selector
   std::set<std::string> Sensors = Scheduler->GetRecordedSensors();
   for (std::set<std::string>::const_iterator I = Sensors.begin(); I != Sensors.end(); ++I)
   {
      Dialog->MovingAverageSensor->addItem(QString::fromStdString(*I));
   }

   // update the graph whenever anything changes
   connect(Dialog->MovingAverageSensor, SIGNAL(currentIndexChanged(int)),
           this, SLOT(UpdateData()));
   connect(Dialog->MovingAverageStatistic, SIGNAL(currentIndexChanged(int)),
           this, SLOT(UpdateData()));
   connect(Dialog->MovingAverageToggleAxis, SIGNAL(clicked()),
           this, SLOT(ToggleAxisClicked()));
   connect(Dialog->MovingAverageLength, SIGNAL(valueChanged(int)),
           this, SLOT(UpdateData()));
   connect(Dialog->MovingAverageZoomIn, SIGNAL(clicked()),
           this, SLOT(ZoomIn()));
   connect(Dialog->MovingAverageZoomOut, SIGNAL(clicked()),
           this, SLOT(ZoomOut()));

   Dialog->MovingAverageZoomIn->setEnabled(Dialog->MovingAverageGraphic->CanZoomIn());
   Dialog->MovingAverageZoomOut->setEnabled(Dialog->MovingAverageGraphic->CanZoomOut());

   this->UpdateData();
}

MovingAverageTab::Statistics
MovingAverageTab::WhichStatistic()
{
   std::string s = Dialog->MovingAverageStatistic->currentText().toStdString();
   if (s == "Average error")
      return TotalError;
   else if (s == "Standard dev")
      return TotalSD;
   return TotalError;  // default
}

void
MovingAverageTab::ToggleAxisClicked()
{
   ScalePercent = !ScalePercent;
   this->UpdateData();
}

void
MovingAverageTab::UpdateData()
{
   std::vector<double> Data;
   std::string Units = ScalePercent ? "%" : "ms";
   std::string const Sensor = Dialog->MovingAverageSensor->currentText().toStdString();
   Statistics s = this->WhichStatistic();

   if (!Sensor.empty())
   {
      statistics::moving_average<double> DataStat(Dialog->MovingAverageLength->value());
      typedef RingingScheduler::SensorRecordListType::const_iterator IterType;
      for (IterType I = Scheduler->GetSensorRecord(Sensor).begin(); 
           I != Scheduler->GetSensorRecord(Sensor).end(); 
           ++I)
      {
         int Row = I->Row;
         double Val = 0;
         if (ScalePercent)
         {
            double FracError = 100.0 * I->Error / Scheduler->GapOfRow(Row);
            switch (s)
            {
               case TotalError : Val = std::abs(FracError); break;
               case TotalSD : Val = FracError * FracError; break;
            }
         }
         else
         {
            double Error = I->Error.total_milliseconds();
            switch (s)
            {
               case TotalError : Val = std::abs(Error); break;
               case TotalSD : Val = Error*Error; break;
            }
         }
         DataStat.push(Val);
         if (s == TotalSD)
         {
            Data.push_back(std::sqrt(DataStat()));
         }
         else
         {
            Data.push_back(DataStat());
         }
      }
   }

   Dialog->MovingAverageGraphic->SetData(Data, Units);
}

void
MovingAverageTab::ZoomIn()
{
   Dialog->MovingAverageGraphic->ZoomIn();
   Dialog->MovingAverageZoomIn->setEnabled(Dialog->MovingAverageGraphic->CanZoomIn());
   Dialog->MovingAverageZoomOut->setEnabled(Dialog->MovingAverageGraphic->CanZoomOut());
}

void
MovingAverageTab::ZoomOut()
{
   Dialog->MovingAverageGraphic->ZoomOut();
   Dialog->MovingAverageZoomIn->setEnabled(Dialog->MovingAverageGraphic->CanZoomIn());
   Dialog->MovingAverageZoomOut->setEnabled(Dialog->MovingAverageGraphic->CanZoomOut());
}

