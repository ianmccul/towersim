// -*- C++ -*- $Id: select-method.h 990 2012-07-16 06:58:12Z uqimccul $

#if !defined(SELECT_METHOD_H_HDCDJSKHL328P9)
#define SELECT_METHOD_H_HDCDJSKHL328P9

#include "change/method.h"
#include <QDialog>
#include <QListWidget>
#include "ui_ui-select-method.h"
#include <vector>
#include <deque>

class SelectMethodDialog : public QDialog
{
   Q_OBJECT

   public:
      SelectMethodDialog(QWidget* Parent = NULL);
  
      // returns the currently selected method
      method* SelectedMethod();

   private slots:
      void StageChanged(int Stage);
      void FilterChanged();
      void SearchStringChanged();
      void FilterAllClicked();
      void FilterNoneClicked();

      void ShowContextMenu(QPoint const& pos);
      void Accepted();

   private:
      void AddToFavourites(int Stage, method* m);
      void RemoveFromFavourites(int Stage, method* m);
      void SaveFavourites();

      // helper to initialize the method list at a given stage
      void InitializeMethodListWidget(int n);

      // updates the window.  Currently does this immediately, but
      // perhaps in the future we make it lazy
      void UpdateWindow();

      Ui::SelectMethodDialog ui;

      std::string MethodsDirectory;

      typedef std::vector<method*> MethodListType;
      typedef std::deque<method*> MethodDequeType;
      std::map<int, MethodListType> FavouriteMethodsAtStage;
      std::map<int, std::deque<method*> > RecentMethodsAtStage;
      std::map<int, MethodListType> AllMethodsAtStage;

      std::map<int, QListWidget*> MethodListWidget;

      int CurrentStage;
      std::string SearchTerm;
      MethodListType::const_iterator CurrentSearchPoint;

      std::map<int, std::string> SavedCursor;
};

#endif

