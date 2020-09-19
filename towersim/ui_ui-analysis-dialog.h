/********************************************************************************
** Form generated from reading UI file 'ui-analysis-dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_2D_ANALYSIS_2D_DIALOG_H
#define UI_UI_2D_ANALYSIS_2D_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
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
            StrikingAnalysis->setObjectName(QStringLiteral("StrikingAnalysis"));
        StrikingAnalysis->resize(762, 393);
        verticalLayout = new QVBoxLayout(StrikingAnalysis);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        StrikingAnalysisInfo = new QTabWidget(StrikingAnalysis);
        StrikingAnalysisInfo->setObjectName(QStringLiteral("StrikingAnalysisInfo"));
        MovingAverage = new QWidget();
        MovingAverage->setObjectName(QStringLiteral("MovingAverage"));
        verticalLayout_2 = new QVBoxLayout(MovingAverage);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        MovingAverageSensor = new QComboBox(MovingAverage);
        MovingAverageSensor->setObjectName(QStringLiteral("MovingAverageSensor"));

        horizontalLayout->addWidget(MovingAverageSensor);

        MovingAverageStatistic = new QComboBox(MovingAverage);
        MovingAverageStatistic->setObjectName(QStringLiteral("MovingAverageStatistic"));

        horizontalLayout->addWidget(MovingAverageStatistic);

        MovingAverageToggleAxis = new QPushButton(MovingAverage);
        MovingAverageToggleAxis->setObjectName(QStringLiteral("MovingAverageToggleAxis"));

        horizontalLayout->addWidget(MovingAverageToggleAxis);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        MovingAverageZoomIn = new QPushButton(MovingAverage);
        MovingAverageZoomIn->setObjectName(QStringLiteral("MovingAverageZoomIn"));

        horizontalLayout->addWidget(MovingAverageZoomIn);

        MovingAverageZoomOut = new QPushButton(MovingAverage);
        MovingAverageZoomOut->setObjectName(QStringLiteral("MovingAverageZoomOut"));

        horizontalLayout->addWidget(MovingAverageZoomOut);

        label = new QLabel(MovingAverage);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        MovingAverageLength = new QSpinBox(MovingAverage);
        MovingAverageLength->setObjectName(QStringLiteral("MovingAverageLength"));
        MovingAverageLength->setMinimum(1);
        MovingAverageLength->setValue(10);

        horizontalLayout->addWidget(MovingAverageLength);


        verticalLayout_2->addLayout(horizontalLayout);

        MovingAverageGraphic = new MovingAverageGraphicsView(MovingAverage);
        MovingAverageGraphic->setObjectName(QStringLiteral("MovingAverageGraphic"));

        verticalLayout_2->addWidget(MovingAverageGraphic);

        StrikingAnalysisInfo->addTab(MovingAverage, QString());
        t2 = new QWidget();
        t2->setObjectName(QStringLiteral("t2"));
        StrikingAnalysisInfo->addTab(t2, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        StrikingAnalysisInfo->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        StrikingAnalysisInfo->addTab(tab_2, QString());

        verticalLayout->addWidget(StrikingAnalysisInfo);

        Close = new QPushButton(StrikingAnalysis);
        Close->setObjectName(QStringLiteral("Close"));

        verticalLayout->addWidget(Close);


        retranslateUi(StrikingAnalysis);
        QObject::connect(Close, SIGNAL(clicked()), StrikingAnalysis, SLOT(accept()));

        StrikingAnalysisInfo->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(StrikingAnalysis);
    } // setupUi

    void retranslateUi(QDialog *StrikingAnalysis)
    {
        StrikingAnalysis->setWindowTitle(QApplication::translate("StrikingAnalysis", "Striking Analysis", Q_NULLPTR));
        MovingAverageStatistic->clear();
        MovingAverageStatistic->insertItems(0, QStringList()
         << QApplication::translate("StrikingAnalysis", "Average error", Q_NULLPTR)
         << QApplication::translate("StrikingAnalysis", "Standard dev", Q_NULLPTR)
        );
        MovingAverageToggleAxis->setText(QApplication::translate("StrikingAnalysis", "Toggle Y-axis %/ms", Q_NULLPTR));
        MovingAverageZoomIn->setText(QApplication::translate("StrikingAnalysis", "Zoom in", Q_NULLPTR));
        MovingAverageZoomOut->setText(QApplication::translate("StrikingAnalysis", "Zoom out", Q_NULLPTR));
        label->setText(QApplication::translate("StrikingAnalysis", "Sample length:", Q_NULLPTR));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(MovingAverage), QApplication::translate("StrikingAnalysis", "Graph", Q_NULLPTR));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(t2), QApplication::translate("StrikingAnalysis", "Striking", Q_NULLPTR));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(tab), QApplication::translate("StrikingAnalysis", "Statistics", Q_NULLPTR));
        StrikingAnalysisInfo->setTabText(StrikingAnalysisInfo->indexOf(tab_2), QApplication::translate("StrikingAnalysis", "Save", Q_NULLPTR));
        Close->setText(QApplication::translate("StrikingAnalysis", "Close", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class StrikingAnalysis: public Ui_StrikingAnalysis {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_2D_ANALYSIS_2D_DIALOG_H
