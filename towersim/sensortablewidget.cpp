// -*- C++ -*- $Id: sensortablewidget.cpp 974 2012-07-04 22:20:50Z uqimccul $

#include "sensortablewidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <QApplication>
#include <QMimeData>
#include <QDrag>
#include "common/trace.h"

SensorTableWidget::SensorTableWidget(QWidget* Parent)
   : QTableWidget(Parent),
     SensorWindow(NULL),
     DragSelectedItem(NULL)
{
   this->setAcceptDrops(true);
   this->setRowCount(16);
   this->setColumnCount(1);
   for (int i = 0; i < 16; ++i)
   {
      QTableWidgetItem* Item = new QTableWidgetItem();
      Item->setData(Qt::UserRole, QVariant(i+1));
      this->setItem(i, 0, Item);
   }
}

void
SensorTableWidget::SetSensorWindow(SensorTab* SensorWindow_)
{
   SensorWindow = SensorWindow_;
}

void SensorTableWidget::dragEnterEvent(QDragEnterEvent* event)
{
   if (event->mimeData()->hasText() &&
       SensorWindow->HasAttachedSensor(event->mimeData()->text().toStdString()))
   {
      event->setDropAction(Qt::MoveAction);
      event->accept();
   }
   else
   {
      event->ignore();
   }

}


void
SensorTableWidget::dragMoveEvent(QDragMoveEvent* event)
{
   if (event->mimeData()->hasText() &&
       SensorWindow->HasAttachedSensor(event->mimeData()->text().toStdString()))
   {
      event->setDropAction(Qt::MoveAction);
      this->selectRow(this->rowAt(event->pos().y()));

      QTableWidgetItem* Item = this->itemAt(event->pos());
      if (Item != DragSelectedItem)
      {
         if (DragSelectedItem)
            DragSelectedItem->setSelected(false);

         if (Item)
            Item->setSelected(true);
      }
      DragSelectedItem = Item;

      this->update();
      event->accept();
   }
}

void
SensorTableWidget::dragLeaveEvent(QDragLeaveEvent*)
{
   if (DragSelectedItem)
   {
      DragSelectedItem->setSelected(false);
      DragSelectedItem = NULL;
   }
}

void
SensorTableWidget::mousePressEvent(QMouseEvent *event)
{
   if (event->button() == Qt::LeftButton)
      DragStartPos = event->pos();
   QTableWidget::mousePressEvent(event);
}

void
SensorTableWidget::mouseMoveEvent(QMouseEvent *event)
{
   if (!(event->buttons() & Qt::LeftButton))
   {
      QTableWidget::mouseMoveEvent(event);
      return;
   }

   int distance = (event->pos() - DragStartPos).manhattanLength();
   if (distance >= QApplication::startDragDistance())
   {

      QLabel* Label = dynamic_cast<QLabel*>(this->childAt(DragStartPos));
      if (!Label)
      {
	 return;
      }

      QPoint hotSpot = DragStartPos - Label->pos();

      QMimeData *mimeData = new QMimeData();
      mimeData->setText(Label->text());

      // Create drag
      QDrag *drag = new QDrag(this);
      drag->setMimeData(mimeData);
      QPixmap pixmap(Label->size());
      Label->render(&pixmap);
      drag->setPixmap(pixmap);
      drag->setHotSpot(hotSpot);

      Qt::DropAction dropAction = drag->exec(Qt::MoveAction);  // blocks

      if (dropAction == Qt::MoveAction)
      {
	 QTableWidgetItem* Where = this->itemAt(DragStartPos);

	 this->setCellWidget(this->row(Where), this->column(Where), NULL);
	 //delete Label;
	 Label->close();
      }
   }
}

void SensorTableWidget::dropEvent(QDropEvent* event)
{
   if (event->mimeData()->hasText() &&
       SensorWindow->HasAttachedSensor(event->mimeData()->text().toStdString()))
   {
      QTableWidgetItem* Item = this->itemAt(event->pos());
      if (!Item)
      {
	 event->ignore();
	 return;
      }
      int Bell = Item->data(Qt::UserRole).toInt();
      QLabel* OldLabel = dynamic_cast<QLabel*>(this->cellWidget(this->row(Item), this->column(Item)));

      // if we're dropping the label onto itself, ignore it
      if (OldLabel && OldLabel->text() == event->mimeData()->text())
      {
         event->ignore();
         return;
      }

      this->setCellWidget(this->row(Item), this->column(Item), NULL);
      if (OldLabel)
      {
	 // We can't just delete or close the label, we need to remove the cell widget
	 //OldLabel->close();
	 //	 delete OldLabel;
      }

      QLabel* Label = new QLabel(event->mimeData()->text());
      //Label->setAutoFillBackground(true);
      Label->setFrameShape(QFrame::Panel);
      Label->setFrameShadow(QFrame::Raised);
      Label->setAttribute(Qt::WA_DeleteOnClose);
      Label->setText(event->mimeData()->text());
      this->setCellWidget(Bell-1, 0, Label);
      TRACE(Item)(Label);
      SensorWindow->AssociateSensor(event->mimeData()->text().toStdString(), Bell);
      this->setCurrentCell(Bell-1, 0);

      event->acceptProposedAction();
   }
}

