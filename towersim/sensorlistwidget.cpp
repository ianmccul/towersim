// -*- C++ -*- $Id: sensorlistwidget.cpp 974 2012-07-04 22:20:50Z uqimccul $

#include "sensorlistwidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPixmap>
#include <QLabel>
#include <QMimeData>
#include <QDrag>
#include "common/trace.h"
#include "common/alphanum.h" // for alphanumeric sort

SensorListWidget::SensorListWidget(QWidget* Parent)
   : QListWidget(Parent),
     SensorWindow(NULL)
{
   this->setAcceptDrops(true);
}

void
SensorListWidget::AddSensor(std::string const& SensorName)
{
   QListWidgetItem* ListItem = new QListWidgetItem();
   QLabel* Item = new QLabel(QString(SensorName.c_str()));
   //Item->setAutoFillBackground(true);
   //Item->setFrameShape(QFrame::Panel);
   //Item->setFrameShadow(QFrame::Raised);
   Item->setAttribute(Qt::WA_DeleteOnClose);

   // find out which row we need to insert at
   int n = this->count();
   int i = 0;
   while (i < n && alphanum_less(dynamic_cast<QLabel*>(this->itemWidget(this->item(i)))
                                 ->text().toStdString(), SensorName))
      ++i;

   TRACE(i)(SensorName);
   this->insertItem(i, ListItem);
   this->setItemWidget(ListItem, Item);
}

void
SensorListWidget::SortSensors()
{
#if 0
   int n = this->count();
   for (int i = 0; i < n-1; ++i)
   {
      std::string label_i = dynamic_cast<QLabel*>(this->itemWidget(this->item(i)))
         ->text().toStdString();
      for (int j = i+1; j < n; ++j)
      {
         std::string label_j = dynamic_cast<QLabel*>(this->itemWidget(this->item(j)))
            ->text().toStdString();
         if (alphanum_less(label_j, label_i))
         {
            TRACE(label_i)(label_j);
            this->setItemWidget(this->item(i), new QLabel(QString::fromStdString(label_j)));
            this->setItemWidget(this->item(j), new QLabel(QString::fromStdString(label_i)));
            label_i = label_j;
            // no need to set label_j, we're about to overwrite it
         }
      }
   }
#endif
}

void
SensorListWidget::RemoveSensor(std::string const& SensorName)
{
   int n = this->count();
   int i = 0;
   while (i < n && dynamic_cast<QLabel*>(this->itemWidget(this->item(i)))
      ->text().toStdString() != SensorName)
      ++i;

   if (i < n)
   {
      delete this->takeItem(i);
   }
}

void
SensorListWidget::SetSensorWindow(SensorTab* SensorWindow_)
{
   SensorWindow = SensorWindow_;
}

void
SensorListWidget::dragEnterEvent(QDragEnterEvent* event)
{
   // we probably should check the event source
   if (event->mimeData()->hasText() &&
       SensorWindow->HasAttachedSensor(event->mimeData()->text().toStdString())
       && event->source() != this)
   {
      TRACE("drag enter accepted");
      event->setDropAction(Qt::MoveAction);
      event->accept();
   }
   else
   {
      event->ignore();
   }
}

void
SensorListWidget::dragMoveEvent(QDragMoveEvent*)
{
   //TRACE("DragMoveEvent");
}

void SensorListWidget::dropEvent(QDropEvent *event)
{
      event->acceptProposedAction();

   // we probably should check the event source
   if (event->mimeData()->hasText() &&
       SensorWindow->HasAttachedSensor(event->mimeData()->text().toStdString())
       && event->source() != this)
   {
      this->AddSensor(event->mimeData()->text().toStdString());
      event->acceptProposedAction();
   }
}

void
SensorListWidget::mousePressEvent(QMouseEvent *event)
{
   if (event->button() == Qt::LeftButton)
      DragStartPos = event->pos();

   QListWidget::mousePressEvent(event);
}

void
SensorListWidget::mouseMoveEvent(QMouseEvent *event)
{
   if (!(event->buttons() & Qt::LeftButton)) return;

   int distance = (event->pos() - DragStartPos).manhattanLength();
   if (distance >= QApplication::startDragDistance())
   {

      QListWidgetItem* WItem = this->itemAt(DragStartPos);
      if (!WItem)
	 return;

      QLabel* Item = dynamic_cast<QLabel*>(this->itemWidget(WItem));
      if (!Item)
	 return;

      QPoint hotSpot = DragStartPos - Item->pos();

      QMimeData *mimeData = new QMimeData();
      mimeData->setText(Item->text());

      // Create drag
      QDrag *drag = new QDrag(this);
      drag->setMimeData(mimeData);
      QPixmap pixmap(Item->size());
      Item->render(&pixmap);
      drag->setPixmap(pixmap);
      drag->setHotSpot(hotSpot);

      Qt::DropAction dropAction = drag->exec(Qt::MoveAction);  // blocks

      if (dropAction == Qt::MoveAction)
      {
         this->removeItemWidget(WItem);
	 //Item->close();  // do we need this?
         delete WItem;
	 //this->removeItemWidget(WItem);
      }
   }
}
