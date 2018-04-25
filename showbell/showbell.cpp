#include <QtGui>
#include <QtSvg>
#include <QGraphicsView>
#include <QtOpenGL>
#include <iostream>
#include <QSocketNotifier>

#include "bellmotion.h"
#include "common/math_const.h"

#include <sys/time.h>
#include <stdlib.h>
#include <cmath>

#define USE_SAMPLE_BUFFERS 1

int savex = 0;

static const int   TIMER_RESOLUTION = 2000;
static const float SECS_PER_TIMER   = TIMER_RESOLUTION / 1000.;
static const int SCALE_INTERVAL  = 1000;
static const int ROTATE_INTERVAL = 2000;

// A sample bell, only the lb is accurate (for the 5th at St Johns)
//BellStayParameters Bell(0.640, 0.75, 0.0, 0.2, -0.5, 0.5);
BellStayParameters Bell(0.640, 0.75, 0.0, 0.2, -10000, 10000);

double TimeScale = 0.5;

double CurrentTime()
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return 1E-6*tv.tv_usec + tv.tv_sec;
}

int64_t get_time()
{
   int64_t         ms; // Milliseconds
   time_t          s;  // Seconds
   struct timespec spec;

   clock_gettime(CLOCK_REALTIME, &spec);

   s  = spec.tv_sec;
   ms = spec.tv_nsec / 1000; // Convert nanoseconds to microseconds
   return ms + int64_t(s)*1000000;
}

double RadToDegrees(double theta)
{
   return 180 * theta / 3.141592653589;
}

inline
double sqr(double x)
{
   return x*x;
}

class Renderer : public QGLWidget
{
    Q_OBJECT
public:
    Renderer(QGLFormat const& Format) :
       QGLWidget(Format),
       BellPos(),
       Impulse(0),
       m_frames(0),
       m_rotate(0),
       m_scale(0.9),
       m_fps(0),
       m_fontMetrics(0)
    {
       Bell.kc = 0; // 0.5;
       Bell.kb = 0; // 0.02;
       Bell.stay_min = -10000;
       Bell.stay_max = 10000;

       TimeOffset = 0;

       std::cout << this->format().swapInterval() << '\n';

       Wheel = new QSvgRenderer(QString("wheel_bell.svg"));

       m_notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
       connect(m_notifier, SIGNAL(activated(int)), this, SLOT(readCommand()));

        m_scaleLine = new QTimeLine(SCALE_INTERVAL, this);
        m_scaleLine->setUpdateInterval(16);
        m_scaleLine->setEasingCurve(QEasingCurve::InCirc);
        connect(m_scaleLine, SIGNAL(finished()),
                SLOT(scaleChangeCurve()));
        connect(m_scaleLine, SIGNAL(valueChanged(qreal)),
                SLOT(scaleChangeScale(qreal)));

        m_rotateLine = new QTimeLine(ROTATE_INTERVAL, this);
        m_rotateLine->setEasingCurve(QEasingCurve::Linear);
        m_rotateLine->setUpdateInterval(10);
        connect(m_rotateLine, SIGNAL(finished()),
                SLOT(rotateChangeCurve()));
        connect(m_rotateLine, SIGNAL(valueChanged(qreal)),
                SLOT(rotateChangeScale(qreal)));

        QPalette pal = palette();
        pal.setBrush(QPalette::Window, QColor(200,200,200));
        this->setPalette(pal);

        BellPos.t = CurrentTime()*TimeScale;
        BellPos.phi = 0;
        BellPos.v_theta = 0;
    }
    ~Renderer()
    {
        delete Wheel;
    }

    void load(const QString &fileName)
    {
        startTimer(TIMER_RESOLUTION);
        m_rotateLine->start();
        m_scaleLine->start();
    }

   void DrawBellAt(QPainter& p, double x, double y, double Size, double BellAngle)
   {
        p.save();
        p.translate(x, y);
        p.rotate(-BellAngle);
        p.translate(-x, -y);
        Wheel->render(&p, QRectF(x-Size/2, y-Size/2, Size, Size));
        p.restore();
   }

protected:
    virtual void paintEvent(QPaintEvent *)
    {
        QPainter p(this);

        p.setRenderHint(QPainter::HighQualityAntialiasing);

#if 0
        double Now = CurrentTime()*TimeScale;
        // determine the direction of the impulse, it is in the direction of the rope
        if (BellPos.theta+Bell.garter > -math_const::pi*0.25)
        {
           // rope is to the right, pulling is to the left
           Impulse = -fabs(Impulse);
        }
        UpdateBellPosition(BellPos, Bell, Now, Impulse);
        if (Impulse != 0)
           Impulse += 0.1 * (Impulse / fabs(Impulse));
        //Impulse = 0;
#endif

        int Height = this->height();
        int Width = this->width();

        double RopeLength = 5.5;  // length of the rope (to the top of the sally), in units of bell radius
        double RopeWidth = 0.02; // width of the rope (in units of bell size)

        // determine the layout
        // The maximum length of the rope is RopeLength, and this goes from radius/sqrt(2) below the
        // centre of the bell.  So the total height of the rope+bell is radius * (RopeLength + 1 + 1/sqrt(2))
        // add a margin of radius at the top and bottom, and we end up with
        // max height = radius * (RopeLength + 1 + 1/sqrt(2) + 2)
        // or
        double Radius = Height / (RopeLength + 3.0 + sqrt(0.5));
        // and if the window is very narrow ....
        if (Width < Radius*3)
           Radius = Width/3.0;

        // location of the bell
        double CentreX = this->width() / 2.0;
        double CentreY = this->height() / 2.0;

        double x = CentreX;
        double y = Radius * (RopeLength + 1 + sqrt(0.5)) * 0.5 - Radius;

        double ThisSize = Radius*2;

        {
           //double ThisSize =  BellSize * (1 + (ScaleFactor-1.0)*i/NumBells);
           this->DrawBellAt(p, x, y, ThisSize,
                            RadToDegrees(BellPos.theta));

           // Add the rope pulley
           double PulleyScale = 0.05;  // sise of the pulley in fractions of a bell size
           double PulleySize = ThisSize*PulleyScale;
           p.setBrush(Qt::SolidPattern);
           QRectF PulleyBox(x-ThisSize/2, y+ThisSize/2, PulleySize, PulleySize);
           p.drawEllipse(PulleyBox);

           // keep track of how much rope is used from the mid-point of the pulley to the garter hole
           // in units of the bell radius
           double RopeLengthFromPulley = 0;

           // draw the rope
           QPen RopePen(QColor(255,0,0));
           RopePen.setWidthF(ThisSize*RopeWidth);
           p.setPen(RopePen);
           if (BellPos.theta+Bell.garter > 0)
           {
              // in this case the rope goes horizontal
              // Section of rope around the wheel
              p.drawArc(QRectF(x-ThisSize/2, y-ThisSize/2, ThisSize, ThisSize), -90*16, int(RadToDegrees(BellPos.theta + Bell.garter)*16+0.5));
              RopeLengthFromPulley += BellPos.theta + Bell.garter;
              // section of rope from the wheel to the pulley edge
              p.drawLine(QLineF(x, y+ThisSize/2, x-ThisSize/2 + PulleySize/2, y+ThisSize/2));
              RopeLengthFromPulley += 1-PulleyScale;
              // arc around pulley
              p.drawArc(PulleyBox, 180*16, -90*16);
              RopeLengthFromPulley += math_const::pi * 0.5 * PulleyScale;
           }
           else if (BellPos.theta+Bell.garter < -math_const::pi/2)
           {
              // The rope goes vertically
              // Draw the section around the wheel
              p.drawArc(QRectF(x-ThisSize/2, y-ThisSize/2, ThisSize, ThisSize), 180*16, 90*16 + int(RadToDegrees(BellPos.theta + Bell.garter)*16+0.5));
              RopeLengthFromPulley += -math_const::pi*0.5 - (BellPos.theta + Bell.garter);
              // section from the wheel to the pulley
              p.drawLine(QLineF(x-ThisSize/2, y, x-ThisSize/2, y+ThisSize/2+PulleySize/2));
              RopeLengthFromPulley += 1+PulleyScale;
           }
           else
           {
              // In this case the garter hole is adjacent to the pulley.  We need to draw the rope at an angle.
              // We have a imaginary right-angled triangle, with the hypotenuse being from the centre of the
              // pulley to the garter hole, and the other point being at the edge of the pulley block.

              // Location of the garter hole, in units of fractions of the bell radius
              double HoleX = sin(BellPos.theta + Bell.garter);
              double HoleY = cos(BellPos.theta + Bell.garter);

              double HolePulleyCentreSq = sqr(1 - PulleyScale + HoleX) + sqr(1 + PulleyScale - HoleY);
              double HolePulleyEdgeSq = HolePulleyCentreSq - sqr(PulleyScale);

              double HolePulleyAngle = atan2(1 + PulleyScale - HoleY, 1 - PulleyScale + HoleX);

              // from the sine rule, sqrt(LenHolePulleyCentreSq) = sqrt(LenHolePulleyEdgeSq) / sin(RopeAngle)
              // where RopeAngle is the angle from the hole-pulley_centre and the hole-pulley_edge
              double AngleAtPulley = HolePulleyAngle + asin(std::sqrt(HolePulleyEdgeSq / HolePulleyCentreSq));

              // draw the line from the garter hole to the pulley tangent
              p.drawLine(QLineF(x+ThisSize/2*sin(BellPos.theta+Bell.garter), y+ThisSize/2*cos(BellPos.theta+Bell.garter),
                                x-ThisSize/2+PulleySize/2*(1 + cos(AngleAtPulley)),
                                y+ThisSize/2+PulleySize/2*(1-sin(AngleAtPulley))));
              RopeLengthFromPulley += sqrt(HolePulleyEdgeSq);

              // draw in the arc
              p.drawArc(PulleyBox, 180*16, -180*16 + int(RadToDegrees(AngleAtPulley)*16+0.5));
              RopeLengthFromPulley += (math_const::pi - AngleAtPulley) * PulleyScale;
           }

           // draw the remainder of the rope
           if (RopeLengthFromPulley < RopeLength)
              p.drawLine(QLineF(x-ThisSize/2, y+ThisSize*(1+PulleyScale)/2, x-ThisSize/2,  y+ThisSize*(1+PulleyScale+RopeLength-RopeLengthFromPulley)/2));

        }

        this->renderFps(&p);

        p.end();

        ++m_frames;

        QTimer::singleShot(0, this, SLOT(update()));
    }

    void timerEvent(QTimerEvent *)
    {
        m_fps = m_frames/SECS_PER_TIMER;
        m_frames = 0;
    }
    QSize sizeHint() const
    {
        return Wheel->defaultSize() + QSize(40, 40);
    }

   virtual void keyPressEvent(QKeyEvent* k)
   {
      if (k->key() == Qt::Key_Space && !k->isAutoRepeat())
         TimeOffset -= 10000000;
      else
         QGLWidget::keyPressEvent(k);
   }

   virtual void keyReleaseEvent(QKeyEvent* k)
   {
      if (k->key() == Qt::Key_Space && !k->isAutoRepeat())
         Impulse = 0;
      else
         QGLWidget::keyReleaseEvent(k);
   }

private:
    void renderFps(QPainter *p)
    {
        if (!m_fontMetrics) {
            m_fontMetrics = new QFontMetrics(p->font());
        }
        QString str;
        if (m_fps) {
            str = tr("fps %1").arg(m_fps, 0, 'g', 5);
        } else {
            str = tr("fps -");
        }
        QRect rect = m_fontMetrics->boundingRect(str);
        p->setPen(Qt::black);
        p->drawText(width() - 19 - rect.width(),
                    height() - 10, str);

        str = tr("time %1").arg((get_time() - TimeOffset - Epoch) / 1e6, 6, 'f', 2);
        rect = m_fontMetrics->boundingRect(str);
        p->drawText(width() - 100 - rect.width(),
                    height() - 10, str);

    }
   private slots:
      void readCommand()
      {
	if (TimeOffset == 0)
	  {
	    TimeOffset = 1;
	    Epoch = get_time();
	  }
	int count = 0;
         while (TimeOffset == 0 || get_time() > NextTime+TimeOffset && ++count < 2)
         {
            if (TimeOffset != 0)
               BellPos.theta = NextAngle*math_const::pi/180;

            std::cin >> NextTime >> NextAngle;

	    //std::cout << "got something" << std::endl;
	    std::cout << NextTime << ' ' <<  get_time() << std::endl;

            if (TimeOffset == 0)
            {
               Epoch = NextTime;
               TimeOffset = get_time() - NextTime;
            }

            //std::cout << (double(NextTime - Epoch)*1e-6) << ' ' << NextAngle << '\n';
         }
      }


      void scaleChangeCurve()
      {
         m_scaleLine->toggleDirection();
         m_scaleLine->start();
      }
      void scaleChangeScale(qreal sc)
      {
         m_scale = 0.9 + sc;
      }
      void rotateChangeCurve()
      {
         m_rotateLine->start();
      }
      void rotateChangeScale(qreal sc)
      {
         m_rotate = -sc * 360.;
      }

   private:
      uint64_t NextTime = 0;
      uint64_t TimeOffset;
      uint64_t Epoch;
      double NextAngle;


      BellStayPosition BellPos;
      double Impulse;

      QSvgRenderer* Wheel;
      int m_frames;
      qreal m_rotate;
      qreal m_scale;
      QTimeLine *m_scaleLine;
      QTimeLine *m_rotateLine;
      qreal m_fps;
      QFontMetrics *m_fontMetrics;

      QSocketNotifier *m_notifier;
};

#include "showbell.moc"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QString fileName;
    if (argc < 2) {
        fileName = QLatin1String("wheel_bell.svg");
    } else {
        fileName = QLatin1String(argv[1]);
    }

    QGLFormat MyFormat;
    MyFormat.setDoubleBuffer(true);
    MyFormat.setAlpha(true);
    MyFormat.setDepth(false);
    MyFormat.setSampleBuffers(true);
    MyFormat.setSwapInterval(1);

    Renderer renderer(MyFormat);
    renderer.load(fileName);
    renderer.show();
    //    renderer.showFullScreen();

    return app.exec();
}
