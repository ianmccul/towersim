// -*- C++ -*- $Id: movingaveragetab.h 966 2012-06-17 12:05:28Z ianmcc $

#if !defined(MOVINGAVERAGETAB_H_SDHCJWEIOUT48U)
#define MOVINGAVERAGETAB_H_SDHCJWEIOUT48U

#include "ringingscheduler.h"
#include <QObject>

namespace Ui
{
   class StrikingAnalysis;
} // namespace Ui

class MovingAverageTab : public QObject
{
   Q_OBJECT

   public:
      MovingAverageTab(boost::shared_ptr<RingingScheduler> Scheduler_,
                       Ui::StrikingAnalysis* Dialog_);

      enum Statistics {TotalError, TotalSD};

      Statistics WhichStatistic();

   private slots:
      void UpdateData();
      void ToggleAxisClicked();
      void ZoomIn();
      void ZoomOut();

   private:
      boost::shared_ptr<RingingScheduler> Scheduler;
      Ui::StrikingAnalysis* Dialog;
      bool ScalePercent; // true if the graph is showing percent, false for ms
};

#endif
