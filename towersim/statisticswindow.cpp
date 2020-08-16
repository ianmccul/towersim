// -*- C++ -*- $Id: statisticswindow.cpp 974 2012-07-04 22:20:50Z uqimccul $

#include "statisticswindow.h"
#include "ui_ui-towersim-mainwindow.h"
#include "common/statistics.h"
#include "analysis-dialog.h"
#include <cmath>
#include <QComboBox>
#include <QLineEdit>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

StatisticsWindow::StatisticsWindow(boost::shared_ptr<RingingScheduler> Scheduler_, 
                                   Ui::MainWindow* MainWindow)
   : ui(MainWindow),
     Scheduler(Scheduler_)
{
   connect(ui->StatisticsSensorSelector, SIGNAL(currentIndexChanged(int)),
           this, SLOT(Update()));
   connect(ui->StatisticsAnalyze, SIGNAL(clicked()), this, SLOT(Analyze()));
}

void
PrintStatistic(QLabel* Label, double Value, double Frac, int N)
{
   std::string Text;
   if (N > 0)
   {
      Text = boost::lexical_cast<std::string>(int(Value/N + 0.5))
         + "ms "
         + boost::lexical_cast<std::string>(int(Frac*100/N + 0.5))
         + "%";
   }
   Label->setText(QString::fromStdString(Text));
}

void
PrintStatisticSqrt(QLabel* Label, double Value, double Frac, int N)
{
   std::string Text;
   if (N > 0)
   {
      Text = boost::lexical_cast<std::string>(int(std::sqrt(Value/N) + 0.5))
         + "ms "
         + boost::lexical_cast<std::string>(int(std::sqrt(Frac/N)*100 + 0.5))
         + "%";
   }
   Label->setText(QString::fromStdString(Text));
}

void
StatisticsWindow::Analyze()
{
   std::string Sensor = ui->StatisticsSensorSelector->currentText().toStdString();
   if (Sensor.empty())
      return;

   StrikingAnalysisDialog MyDialog(Scheduler, ui->centralwidget);
   MyDialog.exec();
}

void
StatisticsWindow::Update()
{
   // firstly get the available sensors
   std::set<std::string> Sensors = Scheduler->GetRecordedSensors();

   // Make sure that every entry in the combo box has a record,
   // and remove already existing items from our Sensors set
   int i = 0;
   while (i < ui->StatisticsSensorSelector->count())
   {
      std::string s = ui->StatisticsSensorSelector->itemText(i).toStdString();
      // if the item doesn't have a record, then remove it
      if (std::find(Sensors.begin(), Sensors.end(), s) == Sensors.end())
      {
         ui->StatisticsSensorSelector->removeItem(i);
      }
      else
      {
         // the item appears in our set; remove it so that we don't add it again
         Sensors.erase(s);
         ++i;
      }
   }

   // any remaining entry in Sensors doesn't yet appear in the combo box, so add it
   for (std::set<std::string>::const_iterator I = Sensors.begin(); I != Sensors.end(); ++I)
   {
      ui->StatisticsSensorSelector->addItem(QString::fromStdString(*I));
   }

   // Now update the statistics
   std::string Sensor = ui->StatisticsSensorSelector->currentText().toStdString();

   if (Sensor.empty())
   {
      ui->StatisticsRows->setText("");
      ui->StatisticsHandstrokeError->setText("");
      ui->StatisticsBackstrokeError->setText("");
      ui->StatisticsCombinedError->setText("");
      ui->StatisticsHandstrokeSD->setText("");
      ui->StatisticsBackstrokeSD->setText("");
      ui->StatisticsSD->setText("");
      ui->StatisticsInside5->setText("");
      ui->StatisticsInside10->setText("");
      ui->StatisticsInside25->setText("");
      ui->StatisticsInside50->setText("");
      ui->StatisticsLastTen->setText("");
   }
   else
   {
      // get the statistics
      int Rows = 0;
      int HandRows = 0;
      int BackRows = 0;
      double HandErrorSq = 0;  // accumulated percentages
      double BackErrorSq = 0;
      double TotalErrorSq = 0;
      double HandErrorMSSq = 0;
      double BackErrorMSSq = 0;
      double TotalErrorMSSq = 0;

      double HandError = 0;   // average absolute
      double BackError = 0;
      double TotalError = 0;
      double HandErrorMS = 0;
      double BackErrorMS = 0;
      double TotalErrorMS = 0;

      int Inside5 = 0;
      int Inside10 = 0;
      int Inside25 = 0;
      int Inside50 = 0;

      statistics::moving_average<double> LastTenMS(10);
      statistics::moving_average<double> LastTen(10);

      typedef RingingScheduler::SensorRecordListType::const_iterator IterType;
      for (IterType I = Scheduler->GetSensorRecord(Sensor).begin(); 
           I != Scheduler->GetSensorRecord(Sensor).end(); 
           ++I)
      {
         int Row = I->Row;
         double ErrorFrac = I->Error / Scheduler->GapOfRow(Row);
         double ErrorFracAbs = fabs(ErrorFrac);
         if (ErrorFracAbs <= 0.5)
         {
            ++Inside50;
            if (ErrorFracAbs <= 0.25)
            {
               ++Inside25;
               if (ErrorFracAbs <= 0.1)
               {
                  ++Inside10;
                  if (ErrorFracAbs <= 0.05)
                  {
                     ++Inside5;
                  }
               }
            }
         }

         double ErrorMS = I->Error.total_milliseconds();

         if (Row % 2 == 0)
         {
            // handstroke
            ++HandRows;
            HandErrorSq += ErrorFrac*ErrorFrac;
            HandErrorMSSq += ErrorMS*ErrorMS;
            HandError += std::abs(ErrorFrac);
            HandErrorMS += std::abs(ErrorMS);
         }
         else
         {
            ++BackRows;
            BackErrorSq += ErrorFrac*ErrorFrac;
            BackErrorMSSq += ErrorMS*ErrorMS;
            BackError += std::abs(ErrorFrac);
            BackErrorMS += std::abs(ErrorMS);
         }

         ++Rows;
         TotalErrorSq += ErrorFrac*ErrorFrac;
         TotalErrorMSSq += ErrorMS*ErrorMS;
         TotalError += std::abs(ErrorFrac);
         TotalErrorMS += std::abs(ErrorMS);
         LastTenMS.push(std::abs(ErrorMS));
         LastTen.push(std::abs(ErrorFrac));
      }

      ui->StatisticsRows->setNum(Rows);

      PrintStatistic(ui->StatisticsCombinedError, TotalErrorMS, TotalError, Rows);
      PrintStatistic(ui->StatisticsHandstrokeError, HandErrorMS, HandError, HandRows);
      PrintStatistic(ui->StatisticsBackstrokeError, BackErrorMS, BackError, BackRows);

      PrintStatisticSqrt(ui->StatisticsSD, TotalErrorMSSq, TotalErrorSq, Rows);
      PrintStatisticSqrt(ui->StatisticsHandstrokeSD, HandErrorMSSq, HandErrorSq, HandRows);
      PrintStatisticSqrt(ui->StatisticsBackstrokeSD, BackErrorMSSq, BackErrorSq, BackRows);

      // The moving average already does the averaging, so set the count to 1
      PrintStatistic(ui->StatisticsLastTen, LastTenMS(), LastTen(), 1);

      ui->StatisticsInside5->setNum(Inside5);
      ui->StatisticsInside10->setNum(Inside10);
      ui->StatisticsInside25->setNum(Inside25);
      ui->StatisticsInside50->setNum(Inside50);
   }

}
