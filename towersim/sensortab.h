// -*- C++ -*- $Id: sensortab.h 896 2012-01-17 13:15:47Z ianmcc $
//

#if !defined(SENSORTAB_H_JKSDHWREUIYT3894Y89)
#define SENSORTAB_H_JKSDHWREUIYT3894Y89

#include <QObject>
#include <set>
#include <boost/shared_ptr.hpp>
#include "ringingscheduler.h"

namespace Ui
{
   class MainWindow;
} // namespace Ui

#if 0
class SensorBoxReceiverType : public QObject
{
   Q_OBJECT

   public:
      SensorBoxReceiverType(BellsimMainWindow* Parent_, int Bell_)
      : Parent(Parent_), Bell(Bell_), Index(1) { }

   private slots:
      void IndexChanged(int NewIndex);

   private:
      BellsimMainWindow* Parent;
      int Bell;
      int Index;
};
#endif

class SensorTab : public QObject
{
   Q_OBJECT

   public:
      SensorTab(boost::shared_ptr<RingingScheduler> Sched, Ui::MainWindow* MainWindow);

      void AttachSensor(std::string const& SensorName);
      void DetachSensor(std::string const& SensorName);

      void AssociateSensor(std::string const& Sensor, int Bell);
      void DisassociateSensor(int Bell);

      // returns true if there is an attached sensor named s
      bool HasAttachedSensor(std::string const& s);

   private slots:
      void SensorClear();
      void AddSensorClicked();
      void RemoveSensorClicked();

   private:
      boost::shared_ptr<RingingScheduler> Scheduler;
      Ui::MainWindow* ui;
 
      std::set<std::string> AttachedSensors;

      //std::vector<QComboBox*> SensorBoxes;
   //      std::vector<SensorBoxReceiverType*> SensorBoxReceiver;

}; // class SensorTab

#endif
