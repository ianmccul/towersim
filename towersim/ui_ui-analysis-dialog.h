/********************************************************************************
** Form generated from reading UI file 'ui-analysis-dialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_2D_ANALYSIS_2D_DIALOG_H
#define UI_UI_2D_ANALYSIS_2D_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "movingaveragegraphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_StrikingAnalysis
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *StrikingAnalysisInfo;
    QWidget *MovingAverage;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QComboBox *MovingAverageSensor;
    QComboBox *MovingAverageStatistic;
    QPushButton *MovingAverageToggleAxis;
    QSpacerItem *horizontalSpacer;
    QPushButton *MovingAverageZoomIn;
    QPushButton *MovingAverageZoomOut;
    QLabel *label;
    QSpinBox *MovingAverageLength;
    MovingAverageGraphicsView *MovingAverageGraphic;
    QWidget *t2;
    QWidget *tab;
    QWidget *tab_2;
    QPushButton *Close;

    void setupUi(QDialog *StrikingAnalysis)
    {
        if (StrikingAnalysis->objectName().isEmpty())
            StrikingAnalysis->setObjectName(QString::fromUtf8("StrikingAnalysis"));
        StrikingAnalysis->resize(762, 393);
        verticalLayout = new QVBoxLayout(StrikingAnalysis);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        StrikingAnalysisInfo = new QTabWidget(StrikingAnalysis);
        StrikingAnalysisInfo->setObjectName(QString::fromUtf8("StrikingAnalysisInfo"));
        MovingAverage = new QWidget();
        MovingAverage->setObjectName(QString::fromUtf8("MovingAverage"));
        verticalLayout_2 = new QVBoxLayout(MovingAverage);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        MovingAverageSensor = new QComboBox(MovingAverage);
        MovingAverageSensor->setObjectName(QString::fromUtf8("MovingAverageSensor"));

        horizontalLayout->addWidget(MovingAverageSensor);

        MovingAverageStatistic = new QComboBox(MovingAverage);
        MovingAverageStatistic->setObjectName(QString::fromUtf8("MovingAverageStatistic"));

        horizontalLayout->addWidget(MovingAverageStatistic);

        MovingAverageToggleAxis = new QPushButton(MovingAverage);
        MovingAverageToggleAxis->setObjectName(QString::fromUtf8("MovingAverageToggleAxis"));

        horizontalLayout->addWidget(MovingAverageToggleAxis);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        MovingAverageZoomIn = new QPushButton(MovingAverage);
        MovingAverageZoomIn->setObjectName(QString::fromUtf8("MovingAverageZoomIn"));

        horizontalLayout->addWidget(MovingAverageZoomIn);

        MovingAverageZoomOut = new QPushButton(MovingAverage);
        MovingAverageZoomOut->setObjectName(QString::fromUtf8("MovingAverageZoomOut"));

        horizontalLayout->addWidget(MovingAverageZoomOut);

        label = new QLabel(MovingAverage);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        MovingAverageLength = new QSpinBox(MovingAverage);
        MovingAverageLength->setObjectName(QString::fromUtf8("MovingAverageLength"));
        MovingAverageLength->setMinimum(1);
        MovingAverageLength->setValue(10);

        horizontalLayout->addWidget(MovingAverageLength);


        verticalLayout_2->addLayout(horizontalLayout);

        MovingAverageGraphic = new MovingAverageGraphicsView(MovingAverage);
        MovingAverageGraphic->setObjectName(QString::fromUtf8("MovingAverageGraphic"));

        verticalLayout_2->addWidget(MovingAverageGraphic);

        StrikingAnalysisInfo->addTab(MovingAverage, QString());
        t2 = new QWidget();
        t2->setObjectName(QString::fromUtf8("t2"));
        StrikingAnalysisInfo->addTab(t2, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        StrikingAnalysisInfo->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        StrikingAnalysisInfo->addTab(tab_2, QString());

        verticalLayout->addWidget(StrikingAnalysisInfo);

        Close = new QPushButton(StrikingAnalysis);
        Close->setObjectName(QString::fromUtf8("Close"));

        verticalLayout->addWidget(Close);


        retranslateUi(StrikingAnalysis);
        QObject::connect(Close, SIGNAL(clicked()), StrikingAnalysis, SLOT(accept()));

        StrikingAnalysisInfo->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(StrikingAnalysis);
    } // setupUi

    void retranslateUi(QDialog *StrikingAnalysis)
    {
        StrikingAnalysis->setWindowTitle(QApplication::translate("StrikingAnalysis", "Striking Analysis", 0, QApplication::UnicodeUTF8));
        MovingAverageStatistic->clear();
        MovingAverageStatistic->insertItems(0, QStringList()
         << QApplication::translate("StrikingAnalysis", "Average error", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("StrikingAnalysis", "Standard dev", 0, QApplication::UnicodeUTF8)
        );
        MovingAverageToggleAxis->setText(QApplication::translate("StrikingAnalysis", "Toggle Y-axis %/ms", 0, QApplication::UnicodeUTF8));
        MovingAverageZoomIn->setText(QApplication::translate("StrikingAnalysis", "Zoom in", 0, QApplication::UnicodeUTF8));
        MovingAverageZoomOut->setText(QApplication::translate("StrikingAnalysis", "Zoom out", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("StrikingAnalysis", "Sample length:", 0, QApplication::UnicodeUTF8));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(MovingAverage), QApplication::translate("StrikingAnalysis", "Graph", 0, QApplication::UnicodeUTF8));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(t2), QApplication::translate("StrikingAnalysis", "Striking", 0, QApplication::UnicodeUTF8));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(tab), QApplication::translate("StrikingAnalysis", "Statistics", 0, QApplication::UnicodeUTF8));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(tab_2), QApplication::translate("StrikingAnalysis", "Save", 0, QApplication::UnicodeUTF8));
        Close->setText(QApplication::translate("StrikingAnalysis", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class StrikingAnalysis: public Ui_StrikingAnalysis {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_2D_ANALYSIS_2D_DIALOG_H
