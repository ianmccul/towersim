// -*- C++ -*- $Id: analysis-dialog.h 966 2012-06-17 12:05:28Z ianmcc $

#if !defined(ANALYSISDIALOG_H_SDJKHEWORIJH3495UY89J)
#define ANALYSISDIALOG_H_SDJKHEWORIJH3495UY89J

#include "ringingscheduler.h"
#include "movingaveragetab.h"
#include <QDialog>

namespace Ui
{
   class StrikingAnalysis;
} // namespace Ui

class StrikingAnalysisDialog : public QDialog
{
   Q_OBJECT

   public:
      StrikingAnalysisDialog(boost::shared_ptr<RingingScheduler> Scheduler_,
                             QWidget* Parent = NULL);

#if 0
      void SetData(boost::shared_ptr<RingingScheduler> Scheduler, std::string const& Sensor);

      void RedrawScene();

   public slots:
      void SetScaleFractional();
      void SetScaleAbsolute();
      void ToggleScale();
#endif

   private:
      Ui::StrikingAnalysis* Dialog;
      MovingAverageTab* MovingAverage;
};

#endif
