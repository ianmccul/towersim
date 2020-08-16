// -*- C++ -*- $Id: sensorlistwidget.h 901 2012-01-29 02:54:23Z ianmcc $
//

#if !defined(SENSORLISTWIDGET_H_JKSDHWREUIYT3894Y89)
#define SENSORLISTWIDGET_H_JKSDHWREUIYT3894Y89

#include <QListWidget>
#include "sensortab.h"

class SensorListWidget : public QListWidget
{
   Q_OBJECT

   public:
      SensorListWidget(QWidget* Parent = NULL);

      // We need access to the SensorTab so this needs to be set after
      // construction
      void SetSensorWindow(SensorTab* SensorWindow_);

      // Adds a sensor to the list
      void AddSensor(std::string const& SensorName);

      // Removes a sensor to the list, if possible
      void RemoveSensor(std::string const& SensorName);

      // sorts the sensor list
      void SortSensors();

   protected:
      void dragEnterEvent(QDragEnterEvent *event);
      void dropEvent(QDropEvent *event);
      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void dragMoveEvent(QDragMoveEvent *event);

   private:
      SensorTab* SensorWindow;
      QPoint DragStartPos;
};

#endif
