// -*- C++ -*- $Id: sensortab.cpp 974 2012-07-04 22:20:50Z uqimccul $

#include "sensortab.h"
#include "ui_ui-towersim-mainwindow.h"
#include <QMessageBox>
#include <QLabel>
#include "common/trace.h"

SensorTab::SensorTab(boost::shared_ptr<RingingScheduler> Sched, Ui::MainWindow* MainWindow)
   : Scheduler(Sched), ui(MainWindow)

{

   ui->AddSensorButton->setIcon(QApplication::style()->
				standardIcon(QStyle::SP_ArrowLeft));

   ui->RemoveSensorButton->setIcon(QApplication::style()->
				   standardIcon(QStyle::SP_ArrowRight));

   connect(ui->SensorClear, SIGNAL(clicked()), 
           this, SLOT(SensorClear()));
   connect(ui->AddSensorButton, SIGNAL(clicked()), 
           this, SLOT(AddSensorClicked()));
   connect(ui->RemoveSensorButton, SIGNAL(clicked()), 
           this, SLOT(RemoveSensorClicked()));


   ui->SensorList->SetSensorWindow(this);
   ui->SensorTable->SetSensorWindow(this);
}

void
SensorTab::SensorClear()
{
   for (int i = 0; i < 16; ++i)
   {
      QLabel* Label = dynamic_cast<QLabel*>(ui->SensorTable->cellWidget(i,0));
      if (Label)
      {
	 Label->close();
	 this->DisassociateSensor(i+1);
      }
   }
}

void
SensorTab::AddSensorClicked()
{
   QTableWidgetItem* TableItem = ui->SensorTable->currentItem();
   if (!TableItem)
      return;

   QListWidgetItem* ListItem = ui->SensorList->currentItem();
   if (!ListItem)
      return;

   QLabel* Label = dynamic_cast<QLabel*>(ui->SensorList->itemWidget(ListItem));
   if (!Label)
      return;

   QLabel* NewLabel = new QLabel(Label->text());
   Label->close();
   delete ui->SensorList->takeItem(ui->SensorList->row(ListItem));
   
   //delete ListItem;

   int Bell = TableItem->data(Qt::UserRole).toInt();
   QWidget* OldLabel = ui->SensorTable->cellWidget(Bell-1,0);
   TRACE(OldLabel);
   if (OldLabel)
   {
      OldLabel->close();
      this->DisassociateSensor(Bell);
   }

   NewLabel->setFrameShape(QFrame::Panel);
   NewLabel->setFrameShadow(QFrame::Raised);
   NewLabel->setAttribute(Qt::WA_DeleteOnClose);
   ui->SensorTable->setCellWidget(Bell-1, 0, NewLabel);
   TRACE(NewLabel)(ui->SensorTable->cellWidget(Bell-1,0))(ui->SensorTable->item(Bell-1,0));
   this->AssociateSensor(NewLabel->text().toStdString(), Bell);
}

void
SensorTab::RemoveSensorClicked()
{
   QTableWidgetItem* Item = ui->SensorTable->currentItem();
   if (!Item)
      return;

   int Bell = Item->data(Qt::UserRole).toInt();
   QLabel* Label = dynamic_cast<QLabel*>(ui->SensorTable->cellWidget(Bell-1,0));
   if (!Label)
      return;

   ui->SensorTable->setCellWidget(Bell-1, 0, NULL);
   this->DisassociateSensor(Bell);
}

void
SensorTab::AttachSensor(std::string const& SensorName)
{
   TRACE("Attach")(SensorName);
   AttachedSensors.insert(SensorName);
   ui->SensorList->AddSensor(SensorName);
}

void
SensorTab::DetachSensor(std::string const& SensorName)
{
   AttachedSensors.erase(SensorName);
   ui->SensorList->RemoveSensor(SensorName);
}

bool
SensorTab::HasAttachedSensor(std::string const& s)
{
   return AttachedSensors.find(s) != AttachedSensors.end();
}

void
SensorTab::AssociateSensor(std::string const& Sensor, int Bell)
{
   TRACE("Associate")(Sensor)(Bell);

   // see if there is already a sensor attached to this bell
   std::string SensorName = Scheduler->GetServer()->GetAssociatedSensor(Bell);
   if (!SensorName.empty())  // if so, return it to the SensorList
   {
      ui->SensorList->AddSensor(SensorName);
   }
   Scheduler->GetServer()->AssociateSensor(Sensor, Bell);
}

void 
SensorTab::DisassociateSensor(int Bell)
{
   TRACE("Disassociate")(Bell);
   std::string SensorName = Scheduler->GetServer()->GetAssociatedSensor(Bell);
   if (!SensorName.empty())
   {
      ui->SensorList->AddSensor(SensorName);
   }
   Scheduler->GetServer()->DisassociateSensor(Bell);
}
