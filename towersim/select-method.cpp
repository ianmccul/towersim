// -*- C++ -*- $Id: select-method.cpp 990 2012-07-16 06:58:12Z uqimccul $

#include "select-method.h"
#include <QSignalMapper>
#include <QListWidget>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QMenu>
#include <set>
#include <boost/algorithm/string/find.hpp>

Q_DECLARE_METATYPE(method*);

bool CompareFullName(method* m1, method* m2)
{
   return m1->full_name_stage_comment() < m2->full_name_stage_comment();
}

SelectMethodDialog::SelectMethodDialog(QWidget* Parent)
   : QDialog(Parent), CurrentStage(0)
{
   ui.setupUi(this);

   // load the methods
   QString DirName = QString(std::getenv("HOME")) + QString("/.towersim/methods/");

   // load the 'favourites' list
   std::set<std::string> Favourites;
   QFile File(DirName + "/favourites/favourites.txt");
   File.open(QIODevice::ReadOnly | QIODevice::Text);
   QTextStream in(&File);
   while (!in.atEnd()) 
   {
      std::string Line = in.readLine().toStdString();
      if (!Line.empty())
	 Favourites.insert(Line);
   }

   // read the method files
   QDir MethodsDirectory(DirName);
   MethodsDirectory.setFilter(QDir::Files);
   QStringList list = MethodsDirectory.entryList();

   for (int i = 0; i < list.size(); ++i)
   {
      if (list.at(i) == "favourites.txt")
	 continue;

      QFile file(DirName + list.at(i));
      file.open(QIODevice::ReadOnly | QIODevice::Text);
      QTextStream in(&file);
      while (!in.atEnd()) 
      {
	 std::string Line = in.readLine().toStdString();
	 if (Line.empty())
	    continue;			       
	 try
	 {
	    method* m = new method(ParseMethod(Line));	 
	    AllMethodsAtStage[m->stage()].push_back(m);
	    if (Favourites.count(m->full_name_stage_comment()))
	       FavouriteMethodsAtStage[m->stage()].push_back(m);
	 }
	 catch (ParseFailure& p)
	 {
	    QMessageBox::warning(this, "Towersim", p.what());
	 }
      }
   }

   // sort the method names
   for (std::map<int, MethodListType>::iterator I = AllMethodsAtStage.begin(); I != AllMethodsAtStage.end(); ++I)
   {
      std::sort(I->second.begin(), I->second.end(), CompareFullName);
   }


   for (int n = 3; n <= 16; ++n)
   {
      QWidget* Page = new QWidget();
      Page->setObjectName(QString::fromStdString("Page_" + boost::lexical_cast<std::string>(n)));
      QHBoxLayout* Layout = new QHBoxLayout(Page);
      Layout->setObjectName(QString::fromStdString("PageLayout_" 
						   + boost::lexical_cast<std::string>(n)));
      MethodListWidget[n] = new QListWidget(Page);
      MethodListWidget[n]->setObjectName(QString::fromStdString("MethodList_" 
							   + boost::lexical_cast<std::string>(n)));
      Layout->addWidget(MethodListWidget[n]);
      ui.MethodDisplay->addWidget(Page);
      MethodListWidget[n]->setContextMenuPolicy(Qt::CustomContextMenu);
      connect(MethodListWidget[n], SIGNAL(customContextMenuRequested(QPoint const&)),
	      this, SLOT(ShowContextMenu(QPoint const&)));

      this->InitializeMethodListWidget(n);
   }

   QSignalMapper* StageMapper = new QSignalMapper(this);
   connect(ui.Select03, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select03, 3);
   connect(ui.Select04, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select04, 4);
   connect(ui.Select05, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select05, 5);
   connect(ui.Select06, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select06, 6);
   connect(ui.Select07, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select07, 7);
   connect(ui.Select08, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select08, 8);
   connect(ui.Select09, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select09, 9);
   connect(ui.Select10, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select10, 10);
   connect(ui.Select11, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select11, 11);
   connect(ui.Select12, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select12, 12);
   connect(ui.Select13, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select13, 13);
   connect(ui.Select14, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select14, 14);
   connect(ui.Select15, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select15, 15);
   connect(ui.Select16, SIGNAL(clicked()), StageMapper, SLOT(map()));
   StageMapper->setMapping(ui.Select16, 16);

   connect(StageMapper, SIGNAL(mapped(int)), this, SLOT(StageChanged(int)));

   connect(ui.FilterSelectAll, SIGNAL(clicked()), this, SLOT(FilterAllClicked()));
   connect(ui.FilterSelectNone, SIGNAL(clicked()), this, SLOT(FilterNoneClicked()));

   connect(ui.FilterPrinciples, SIGNAL(clicked()), this, SLOT(FilterChanged()));
   connect(ui.FilterPlain, SIGNAL(clicked()), this, SLOT(FilterChanged()));
   connect(ui.FilterTrebleBob, SIGNAL(clicked()), this, SLOT(FilterChanged()));
   connect(ui.FilterDelight, SIGNAL(clicked()), this, SLOT(FilterChanged()));
   connect(ui.FilterSurprise, SIGNAL(clicked()), this, SLOT(FilterChanged()));
   connect(ui.FilterOther, SIGNAL(clicked()), this, SLOT(FilterChanged()));

   connect(ui.MethodSearch, SIGNAL(textChanged(QString const&)), this, SLOT(SearchStringChanged()));

   connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(Accepted()));

   ui.Select08->setChecked(true);
   this->StageChanged(8);
}
    
method*
SelectMethodDialog::SelectedMethod()
{
   if (MethodListWidget[CurrentStage] == NULL)
      return NULL;
   QListWidgetItem* Item = MethodListWidget[CurrentStage]->currentItem();
   if (!Item)
      return NULL;
   QVariant Data = Item->data(Qt::UserRole);
   if (Data.isNull())
      return NULL;
   return Data.value<method*>();
}
  
void 
SelectMethodDialog::StageChanged(int Stage)
{
   if (Stage == CurrentStage)
      return;
   CurrentStage = Stage;
   ui.MethodDisplay->setCurrentIndex(Stage-3);
   this->UpdateWindow();
}

void 
SelectMethodDialog::FilterChanged()
{
   this->UpdateWindow();
}

void 
SelectMethodDialog::SearchStringChanged()
{
   this->UpdateWindow();
}

void 
SelectMethodDialog::FilterAllClicked()
{
   ui.FilterPrinciples->setChecked(true);
   ui.FilterPlain->setChecked(true);
   ui.FilterTrebleBob->setChecked(true);
   ui.FilterDelight->setChecked(true);
   ui.FilterSurprise->setChecked(true);
   ui.FilterOther->setChecked(true);
   this->UpdateWindow();
}

void 
SelectMethodDialog::FilterNoneClicked()
{
   ui.FilterPrinciples->setChecked(false);
   ui.FilterPlain->setChecked(false);
   ui.FilterTrebleBob->setChecked(false);
   ui.FilterDelight->setChecked(false);
   ui.FilterSurprise->setChecked(false);
   ui.FilterOther->setChecked(false);
   this->UpdateWindow();
}

void
SelectMethodDialog::ShowContextMenu(QPoint const& pos)
{
   // Get the currently selected method
   QListWidgetItem* Item = MethodListWidget[CurrentStage]->itemAt(pos);
   if (!Item)
      return;

   QVariant v = Item->data(Qt::UserRole);
   if (v.isNull())
      return;

   method* m = v.value<method*>();

   // Create a context menu
   QPoint GlobalPos = MethodListWidget[CurrentStage]->viewport()->mapToGlobal(pos);
   QMenu ContextMenu;

   // is the method in the favourites list?
   if (std::find(FavouriteMethodsAtStage[CurrentStage].begin(),
		 FavouriteMethodsAtStage[CurrentStage].end(),
		 m) == FavouriteMethodsAtStage[CurrentStage].end())
   {
      // not in the favourites list, so we have an option to add it
      ContextMenu.addAction("Add to favourites");
   }
   else
   {
      // already in the favourites list
      ContextMenu.addAction("Remove from favourites");
   }

   // run the menu
   QAction* SelectedItem = ContextMenu.exec(GlobalPos);
   if (!SelectedItem)
      return;  // no selected item

   // What was the action?
   std::string Action = SelectedItem->text().toStdString();
   if (Action == "Add to favourites")
      this->AddToFavourites(CurrentStage, m);
   else if (Action == "Remove from favourites")
      this->RemoveFromFavourites(CurrentStage, m);
}

void
SelectMethodDialog::Accepted()
{
   this->SaveFavourites();
}

void
SelectMethodDialog::AddToFavourites(int Stage, method* m)
{
   // add the method to the favourites list,
   // then sort the list and find the index of it
   FavouriteMethodsAtStage[Stage].push_back(m);
   std::sort(FavouriteMethodsAtStage[Stage].begin(), FavouriteMethodsAtStage[Stage].end(),
	     CompareFullName);
   int Index = std::find(FavouriteMethodsAtStage[Stage].begin(), 
			 FavouriteMethodsAtStage[Stage].end(),
			 m) - FavouriteMethodsAtStage[Stage].begin();

   // adjust the index for the recent methods and titles
   Index += RecentMethodsAtStage[Stage].size() + 2;
   QListWidgetItem* Item = new 
      QListWidgetItem(QString::fromStdString(m->full_name_stage_comment()));
   Item->setData(Qt::UserRole, qVariantFromValue(m));
   MethodListWidget[Stage]->insertItem(Index, Item);
}

void
SelectMethodDialog::RemoveFromFavourites(int Stage, method* m)
{
   // Find the index of the method
   std::vector<method*>::iterator I = std::find(FavouriteMethodsAtStage[Stage].begin(), 
			 FavouriteMethodsAtStage[Stage].end(),
						m);
   if (I == FavouriteMethodsAtStage[Stage].end())
      return;  // it wasn't found

   int Index = I - FavouriteMethodsAtStage[Stage].begin();
   // remove the item from the vector and the list widget
   FavouriteMethodsAtStage[Stage].erase(I);
   // adjust the index for the recent methods and titles
   Index += RecentMethodsAtStage[Stage].size() + 2;
   QListWidgetItem* Item = MethodListWidget[Stage]->takeItem(Index);
   delete Item;
}

void
SelectMethodDialog::SaveFavourites()
{
   QString DirName = QString(std::getenv("HOME")) + QString("/.towersim/methods/");

   // load the 'favourites' list
   std::set<std::string> Favourites;
   QFile File(DirName + "/favourites/favourites.txt");
   File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
   for (int n = 3; n <= 16; ++n)
   {
      MethodListType::const_iterator Beg = FavouriteMethodsAtStage[n].begin();
      MethodListType::const_iterator End = FavouriteMethodsAtStage[n].end();
      for (MethodListType::const_iterator I = Beg; I != End; ++I)
      {
	 File.write(((*I)->full_name_stage_comment() + '\n').c_str());
      }
   }
   File.close();
}

void
SelectMethodDialog::InitializeMethodListWidget(int n)
{
   // the Recent section
   QListWidgetItem* W = new QListWidgetItem("Recent");
   QFont f = W->font();
   f.setBold(true);
   W->setFont(f);
   MethodListWidget[n]->addItem(W);

   for (MethodDequeType::const_iterator 
	I = RecentMethodsAtStage[n].begin(); 
	I != RecentMethodsAtStage[n].end(); 
	++I)
   {
      W = new QListWidgetItem(QString::fromStdString((*I)->full_name_stage_comment()));
      W->setData(Qt::UserRole, qVariantFromValue(*I));
      MethodListWidget[n]->addItem(W);
   }

   // the Favourites section
   W = new QListWidgetItem("Favourites");
   f = W->font();
   f.setBold(true);
   W->setFont(f);
   MethodListWidget[n]->addItem(W);

   for (MethodListType::const_iterator 
	I = FavouriteMethodsAtStage[n].begin(); 
	I != FavouriteMethodsAtStage[n].end(); 
	++I)
   {
      W = new QListWidgetItem(QString::fromStdString((*I)->full_name_stage_comment()));
      W->setData(Qt::UserRole, qVariantFromValue(*I));
      MethodListWidget[n]->addItem(W);
   }

   // The All Methods section
   W = new QListWidgetItem("All Methods");
   f = W->font();
   f.setBold(true);
   W->setFont(f);
   MethodListWidget[n]->addItem(W);

   for (MethodListType::const_iterator 
	I = AllMethodsAtStage[n].begin(); 
	I != AllMethodsAtStage[n].end(); 
	++I)
   {
      W = new QListWidgetItem(QString::fromStdString((*I)->full_name_stage_comment()));
      W->setData(Qt::UserRole, qVariantFromValue(*I));
      MethodListWidget[n]->addItem(W);
   }
}

void 
SelectMethodDialog::UpdateWindow()
{
   QListWidget* Widget = MethodListWidget[CurrentStage];
   if (!Widget)
      return;

   int Count = Widget->count();
   std::string SearchString = ui.MethodSearch->text().toStdString();
   for (int i = 0; i < Count; ++i)
   {
      bool Show = true;
      QListWidgetItem* Item = Widget->item(i);
      QVariant Data = Item->data(Qt::UserRole);
      if (Data.isNull())   // this means it is a label, not a method
	 continue;
      method* m = Data.value<method*>();
      switch (m->type())
      {
         case Principle : Show = ui.FilterPrinciples->isChecked(); break;
         case Plain     : Show = ui.FilterPlain->isChecked(); break;
         case TrebleBob : Show = ui.FilterTrebleBob->isChecked(); break;
         case Delight   : Show = ui.FilterDelight->isChecked(); break;
         case Surprise  : Show = ui.FilterSurprise->isChecked(); break;
         default        : Show = ui.FilterOther->isChecked();
      }
      if (Show && !SearchString.empty())
      {
	 std::string Name = m->full_name_stage_comment();
	 Show = boost::algorithm::ifind_first(Name, SearchString);
      }
      Item->setHidden(!Show);
   }
}


