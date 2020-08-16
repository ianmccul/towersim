// -*- C++ -*- $Id: movingaveragegraphicsview.cpp 966 2012-06-17 12:05:28Z ianmcc $

#include "movingaveragegraphicsview.h"
#include "common/trace.h"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include "labels.h"

double const MinZoom = 1.0;
double const MaxZoom = 10.0;
double const ZoomFactor = std::sqrt(2.0);

MovingAverageGraphicsView::MovingAverageGraphicsView(QWidget* Parent)
   : QGraphicsView(Parent), 
     Scene(new QGraphicsScene()),
     Max(0),
     XZoom(1.0),
     YZoom(1.0)
{
}

void
MovingAverageGraphicsView::SetData(std::vector<double> const& Data_, std::string const& Units_)
{
   Data = Data_;
   Units = Units_;
   Max = *std::max_element(Data.begin(), Data.end());

   this->Redraw();
}

void
MovingAverageGraphicsView::resizeEvent(QResizeEvent* Event)
{
   this->Redraw();
}

void
MovingAverageGraphicsView::ZoomIn()
{
   YZoom *= ZoomFactor;
   if (YZoom > MaxZoom)
      YZoom = MaxZoom;
   this->Redraw();
}

bool
MovingAverageGraphicsView::CanZoomIn() const
{
   return XZoom < MaxZoom || YZoom < MaxZoom;
}

void
MovingAverageGraphicsView::ZoomOut()
{
   YZoom /= ZoomFactor;
   if (YZoom < MinZoom)
      YZoom = MinZoom;
   this->Redraw();
}

bool
MovingAverageGraphicsView::CanZoomOut() const
{
   return YZoom > MinZoom;
}

void
MovingAverageGraphicsView::Redraw()
{
   this->setScene(NULL);
   Scene->clear();
   delete Scene;
   Scene = new QGraphicsScene();

   int TopMargin = 20;
   int BottomMargin = 20;
   int LeftMargin = 50;
   int RightMargin = 50;
   int MinPixelsPerEntry = 1;

   int Height = std::floor(this->geometry().height()*YZoom + 0.5);
   int Width = std::floor(this->geometry().width()*XZoom + 0.5);

   // To calculate the overall height, we need to first
   // calculate the label positions because the final label
   // will always be > the maximum data point.
   // Find out the maximum number of labels from the height of the label text.
   QGraphicsTextItem* Label = new QGraphicsTextItem(QString::fromStdString("0" + Units));
   int MaxLabels = (Height-TopMargin-BottomMargin) / (Label->boundingRect().height()*2);
   delete Label;  // we'll reconstruct it later in the loop

   // check: if the size of the window is so small that we can't display at least 2 labels,
   // then don't display anything
   if (MaxLabels < 2 || Data.size() < 2)
      return;

   // get the labels
   std::vector<std::pair<double, std::string> > Labels = GraphLabels(0.0, Max, MaxLabels);

   // the bounds of the graph will be the position of the final label
   // Scale is the pixels per unit on the Y axis
   double Scale = (Height-TopMargin-BottomMargin) / Labels.back().first;

   // Pixels per data point on the X axis
   int PixelsPerEntry = (Width-LeftMargin-RightMargin) / Data.size();
   if (PixelsPerEntry < MinPixelsPerEntry)
      PixelsPerEntry = MinPixelsPerEntry;

   // width in pixels of the graph, excluding margins
   int GraphWidth = Data.size() * PixelsPerEntry;

   for (int i = 0; i < int(Data.size())-1; ++i)
   {
      int StartX = i*PixelsPerEntry + LeftMargin;
      int EndX = StartX + PixelsPerEntry;
      int StartY = Height - BottomMargin - std::floor(Data[i]*Scale+0.5);
      int EndY = Height - BottomMargin - std::floor(Data[i+1]*Scale+0.5);
      QGraphicsLineItem* Line = new QGraphicsLineItem(StartX, StartY, EndX, EndY);
      Scene->addItem(Line);
   }

   // show the axes and labels
   for (unsigned i = 0; i < Labels.size(); ++i)
   {
      Label = new QGraphicsTextItem(QString::fromStdString(Labels[i].second + Units));
      Label->setPos(LeftMargin - Label->boundingRect().width(), 
                    Height-BottomMargin-Scale*Labels[i].first-Label->boundingRect().height()/2);
      Scene->addItem(Label);

      QGraphicsLineItem* Axis = new QGraphicsLineItem(LeftMargin, 
                                                      Height-BottomMargin-Scale*Labels[i].first, 
                                                      LeftMargin+GraphWidth, 
                                                      Height-BottomMargin-Scale*Labels[i].first);
      Scene->addItem(Axis);
   }

   this->setScene(Scene);
   this->show();
}
