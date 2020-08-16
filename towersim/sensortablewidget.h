// -*- C++ -*- $Id: sensortablewidget.h 901 2012-01-29 02:54:23Z ianmcc $
//

#if !defined(SENSORTABLEWIDGET_H_JKSDHWREUIYT3894Y89)
#define SENSORTABLEWIDGET_H_JKSDHWREUIYT3894Y89

#include <QTableWidget>
#include "sensortab.h"

class SensorTableWidget : public QTableWidget
{

   Q_OBJECT

   public:
      SensorTableWidget(QWidget* Parent = NULL);

      void SetSensorWindow(SensorTab* SensorWindow_);

   protected:
      void mouseMoveEvent(QMouseEvent *event);
      void dragEnterEvent(QDragEnterEvent *event);
      void dragLeaveEvent(QDragLeaveEvent *event);
      void dropEvent(QDropEvent *event);
      void dragMoveEvent(QDragMoveEvent *event);
      void mousePressEvent(QMouseEvent *event);

   private:
      SensorTab* SensorWindow;
      QPoint DragStartPos;
      QTableWidgetItem* DragSelectedItem;
};

#endif
