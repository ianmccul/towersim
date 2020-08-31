// -*- C++ -*- $Id: movingaveragegraphicsview.h 966 2012-06-17 12:05:28Z ianmcc $
//

#if !defined(MOVINGAVERAGEGRAPHICSVIEW_H_JKSDHWREUIYT3894Y89)
#define MOVINGAVERAGEGRAPHICSVIEW_H_JKSDHWREUIYT3894Y89

#include <QGraphicsView>
#include <vector>

class MovingAverageGraphicsView : public QGraphicsView
{
   public:
      MovingAverageGraphicsView(QWidget* Parent = NULL);

      void SetData(std::vector<double> const& Data_, std::string const& Units_);

      // Zoom in on the graph.
      void ZoomIn();

      // returns true if the current zoom is less than the maximum possible
      bool CanZoomIn() const;

      // Zoom out on the graph.
      void ZoomOut();

      // returns true if the current zoom is larger than the smallest possible
      bool CanZoomOut() const;

   protected:
      virtual void resizeEvent(QResizeEvent* Event);

   private:
      void Redraw();

      QGraphicsScene* Scene;
      std::vector<double> Data;
      double Max;
      std::string Units;
      double XZoom;
      double YZoom;
};

#endif
