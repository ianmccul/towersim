// -*- C++ -*- $Id: towersim-mainwindow.h 974 2012-07-04 22:20:50Z uqimccul $

#if !defined(BELLSIM_MAINWINDOW_H_HSDKJHERUIOTYIO)
#define BELLSIM_MAINWINDOW_H_HSDKJHERUIOTYIO

#include "sensorparser.h"
#include "methodcontroller.h"
#include "statisticswindow.h"
#include "sensortab.h"
#include "strikingwindow.h"
#include <QObject>
#include <QMainWindow>
#include <QTime>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

namespace Ui
{
   class MainWindow;
} // namespace Ui

class SensorInputServer;
class SensorBoxReceiverType;

class TowerSimMainWindow : public QMainWindow
{
   Q_OBJECT

   public:
      TowerSimMainWindow(QWidget* Parent = NULL);
   
      virtual ~TowerSimMainWindow();

      void AttachSensor(std::string const& SensorName);
      void AttachSensor(::AttachSensor const& a);
      void DetachSensor(std::string const& SensorName);
     
      void AssociateSensor(std::string const& Sensor, int Bell);
      void DisassociateSensor(int Bell);

      // updates the statistics window
      void UpdateSensorStatistics();

      // update the bell gap display
      void UpdateBellGap();
  
      boost::shared_ptr<RingingScheduler> GetScheduler() const { return Scheduler; }
 
   private slots:
      void ChangesPerMinute_valueChanged(double Value);
      void PealSpeed_timeChanged(QTime Time);

      // update the UI display
      void NumberOfBells_valueChanged(int Value);

   private:
      boost::shared_ptr<RingingScheduler> Scheduler;
      Ui::MainWindow* ui;
      MethodController* MController;
      SensorInputServer* SensorInput;
      StatisticsWindow* StatisticsWin;
      SensorTab* SensorWindow;

      std::vector<QLabel*> CCLabel;
      std::vector<QPushButton*> CCButton;

      double ChangesPerMinute;  // floating point (accurate!) version

      bool ProtectModifySpeed;  // flag to avoid recursion in
         // updating the tied peal-speed and changes-per-minute controls

}; // class TowerSimMainWindow

#endif
