// -*- C++ -*- $Id: statisticswindow.h 966 2012-06-17 12:05:28Z ianmcc $

#if !defined(STATISTICSWINDOW_H_JKSDHWREUIYT3894Y89)
#define STATISTICSWINDOW_H_JKSDHWREUIYT3894Y89

#include "ringingscheduler.h"
#include <QObject>


namespace Ui
{
   class MainWindow;
} // namespace Ui

class StatisticsWindow : public QObject
{
   Q_OBJECT

   public:
      StatisticsWindow(boost::shared_ptr<RingingScheduler> Scheduler_, 
                       Ui::MainWindow* MainWindow);

   public slots:
      void Update();
      void Analyze();

   private:
      Ui::MainWindow* ui;
      boost::shared_ptr<RingingScheduler> Scheduler;

};

#endif
