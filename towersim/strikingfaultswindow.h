// -*- C++ -*- $Id: strikingfaultswindow.h 966 2012-06-17 12:05:28Z ianmcc $

#if !defined(STRIKINGFAULTSWINDOW_H_JKSDHWREUIYT3894Y89)
#define STRIKINGFAULTSWINDOW_H_JKSDHWREUIYT3894Y89

#include "ringingscheduler.h"
#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QMessageBox>

// The UI can look a bit cluttered if we have individual details for
// all 16 bells (which 99% of the time will not be used).  So
// we only show a smaller number by default, and automatically
// increase it if a larger number of bells is actually selected.
int const DefaultMaxBellsShown = 8;

// forward declare our MainWindow object
namespace Ui
{
   class MainWindow;
} // namespace Ui

class StrikingFaultsWindow : public QObject
{
   Q_OBJECT

   public:
      StrikingFaultsWindow(boost::shared_ptr<RingingScheduler> Scheduler_, 
                     Ui::MainWindow* MainWindow);

      // if n > NumBellsShown, then make additional widgets visible to control the striking
      // of bells up to n.  If n <= NumBellsShown, do nothing.
      void IncreaseNumBells(int n);

   private slots:
      // overall errors
      void UniformDistributionClicked();
      void GaussianDistributionClicked();
      void OverallHandErrorValueChanged(int Value);
      void OverallSeparateBackErrorClicked(bool Clicked);
      void OverallBackErrorValueChanged(int Value);

      // bell errors
      void HandErrorValueChanged(int Value);
      void SeparateBackErrorClicked(bool Clicked);
      void BackErrorValueChanged(int Value);
      void ResetBellError();

      // bell delays
      void HandDelayValueChanged(int Value);
      void SeparateBackDelayClicked(bool Clicked);
      void BackDelayValueChanged(int Value);
      void ResetBellDelay();

      // sensor feedback
      void SensorErrorMultiplierValueChanged(int Value);
      void RingingSpeedFromSensorsValueChanged(int Value);

   private:
      template <typename T>
      unsigned FindSenderBell(std::vector<T*> const& Array, std::string const& ErrMsg = "UNKNOWN");

      Ui::MainWindow* ui;
      boost::shared_ptr<RingingScheduler> Scheduler;

      // The delay dock
      std::vector<QLabel*> StrikingLabel;
      std::vector<QSlider*> HandDelay;
      std::vector<QLabel*> HandLabel;
      std::vector<QCheckBox*> SeparateBackDelay;
      std::vector<QSlider*> BackDelay;
      std::vector<QLabel*> BackLabel;

      // the error dock
      std::vector<QLabel*> StrikingErrorLabel;
      std::vector<QSlider*> HandError;
      std::vector<QLabel*> LabelHandError;
      std::vector<QCheckBox*> SeparateBackError;
      std::vector<QSlider*> BackError;
      std::vector<QLabel*> LabelBackError;

      int NumBellsShown;
};

template <typename T>
unsigned
StrikingFaultsWindow::FindSenderBell(std::vector<T*> const& Array, std::string const& ErrMsg)
{
   T* Sender = qobject_cast<T*>(this->sender());
   if (!Sender)
   {
      QMessageBox::critical(NULL, QString::fromStdString("Unexpected error"), 
                            QString::fromStdString("Sender of " + ErrMsg + " is NULL"),
                            QMessageBox::Ok);
      return Array.size();
   }

   unsigned Bell = std::find(Array.begin(), Array.end(), Sender) - Array.begin();
   if (Bell >= Array.size())
   {
      QMessageBox::critical(NULL, QString::fromStdString("Unexpected error"), 
                            QString::fromStdString("Sender of " + ErrMsg + " is unidentified ("
                                                   + boost::lexical_cast<std::string>((void*)(Sender))
                                                   + ")"),
                            QMessageBox::Ok);
   }
   return Bell;
}   

#endif
