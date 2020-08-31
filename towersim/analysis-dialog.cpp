// -*- C++ -*- $Id: analysis-dialog.cpp 966 2012-06-17 12:05:28Z ianmcc $

#include "analysis-dialog.h"
#include "ui_ui-analysis-dialog.h"
#include "movingaveragegraphicsview.h"

StrikingAnalysisDialog::StrikingAnalysisDialog(boost::shared_ptr<RingingScheduler> Scheduler_,
                                               QWidget* Parent)
   : QDialog(Parent), 
     Dialog(new Ui::StrikingAnalysis),
     MovingAverage(NULL)
{
   Dialog->setupUi(this);
   MovingAverage = new MovingAverageTab(Scheduler_, Dialog);
   //   Dialog->MovingAverageGraphic->show();
}
