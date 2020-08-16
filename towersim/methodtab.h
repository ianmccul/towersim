// -*- C++ -*- $Id: methodtab.h 967 2012-06-24 12:26:14Z uqimccul $

#if !defined(METHODTAB_H_JKSDHWREUIYT3894Y89)
#define METHODTAB_H_JKSDHWREUIYT3894Y89

#include "ringingscheduler.h"
#include "../method.h"
#include <QObject>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>

namespace Ui
{
   class MainWindow;
} // namespace Ui

class MethodTab : public QObject
{
   Q_OBJECT

   public:
      MethodTab(boost::shared_ptr<RingingScheduler> Scheduler_, 
                    Ui::MainWindow* MainWindow);

   protected:
      void timerEvent(QTimerEvent *event);

   private slots:
      void MethodStart();
      void MethodStand();
      void MethodRounds();
      void MethodStop();
      void MethodGo();
      void MethodThatsAll();
      void MethodClicked(QModelIndex Index);

      void MethodStartRandomSpliced();

   private:
      void UiNoMethod();  // set enabled/disabled for no method selected
      void UiStanding(); // set enabled/disabled for 'standing' bells
      void UiRinging(); // set enabled/disabled for bells ringing rounds
      void UiGoing();  // set enabled/disabled for ringing a method

      Ui::MainWindow* ui;
      boost::shared_ptr<RingingScheduler> Scheduler;

      QTreeView* MethodTree;
      QStandardItemModel MethodTreeModel;
      QStandardItem* Favourites;
      std::map<int, QStandardItem*> Methods;  // map stage to a node

      method* CurrentMethod;
      method_cursor Cursor;

      int MyTimer;

      int RowsRounds; // number of rows of rounds that we've rung (for autostart)
      int NumCourses;  // number of courses that we've rung

      bool MethodRinging;  // true if we're ringing a method

      int StandRow;  // the row number where we're going to stand
      bool CallStand;  // true if we should make an audible call when standing

      int GoRow;  // the row number where we're going to start the method
      bool CallGo;      // true if we should make an audible call when going

      bool ThatsAll;  // flag to indicate that we're stopping at the next rounds
      bool CallThatsAll;  // true if we should make an audible 'thats all' call

      bool AllowAutoStart; // if we've gone back to rounds, then don't auto-start again

      bool RandomSpliced; // true if we're ringing random 8-spliced
      bool FirstMethod;  // flag to indicate we're ringing the first method, so don't splice
      int MethodNum;
  public:
      std::vector<method*> EightSpliced;
};

#endif
