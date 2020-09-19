/********************************************************************************
** Form generated from reading UI file 'ui-towersim-mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_2D_TOWERSIM_2D_MAINWINDOW_H
#define UI_UI_2D_TOWERSIM_2D_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "sensorlistwidget.h"
#include "sensortablewidget.h"
#include "strikingwindow.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionFile;
    QAction *actionListen_for_connections;
    QAction *actionSetup_local_sensors;
    QAction *actionAbout_Bell_Ringing_Simulator;
    QAction *actionPreferences;
    QAction *actionSensors;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_10;
    QVBoxLayout *verticalLayout_3;
    QPushButton *RingingStart;
    QPushButton *RingingAuto;
    QPushButton *RingingRounds;
    QPushButton *RingingStand;
    QPushButton *RingingStop;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_24;
    QLabel *label_19;
    QSpinBox *NumberOfBells;
    QLabel *label_20;
    QComboBox *CCStyle;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *CC1;
    QLabel *CC2;
    QLabel *CC3;
    QLabel *CC4;
    QLabel *CC5;
    QLabel *CC6;
    QLabel *CC7;
    QLabel *CC8;
    QLabel *CC9;
    QLabel *CC10;
    QLabel *CC11;
    QLabel *CC12;
    QLabel *CC13;
    QLabel *CC14;
    QLabel *CC15;
    QLabel *CC16;
    QGroupBox *groupBox_7;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *SelectCallChange;
    QRadioButton *SelectDodge;
    QRadioButton *SelectRightPlaces;
    QRadioButton *SelectWrongPlaces;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *verticalSpacer_5;
    QPushButton *Call12;
    QPushButton *Call23;
    QPushButton *Call34;
    QPushButton *Call45;
    QPushButton *Call56;
    QPushButton *Call67;
    QPushButton *Call78;
    QPushButton *Call89;
    QPushButton *Call90;
    QPushButton *Call0E;
    QPushButton *CallET;
    QPushButton *CallTA;
    QPushButton *CallAB;
    QPushButton *CallBC;
    QPushButton *CallCD;
    QSpacerItem *verticalSpacer_7;
    QHBoxLayout *horizontalLayout_9;
    QVBoxLayout *verticalLayout_7;
    QFrame *frame;
    QVBoxLayout *verticalLayout_9;
    QPushButton *RingMethod;
    QPushButton *RingTouch;
    QSpacerItem *verticalSpacer_3;
    QPushButton *MethodGo;
    QGroupBox *MethodGoAfterBox;
    QVBoxLayout *verticalLayout_18;
    QPushButton *MethodGoDelay;
    QSpinBox *MethodGoAfter;
    QLabel *label_50;
    QStackedWidget *stackedWidget;
    QWidget *stackedWidgetPage1;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *MethodTenorBehind;
    QGridLayout *gridLayout;
    QLabel *label_22;
    QLabel *CurrentMethod;
    QLabel *label_24;
    QLabel *NextMethod;
    QPushButton *SelectMethod;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_4;
    QVBoxLayout *verticalLayout;
    QCheckBox *MethodCallRepeat;
    QCheckBox *MethodRandomCalls;
    QGroupBox *groupBox_9;
    QVBoxLayout *verticalLayout_14;
    QPushButton *MethodCall0;
    QPushButton *MethodCall1;
    QPushButton *MethodCall2;
    QPushButton *MethodCall3;
    QPushButton *MethodSplice;
    QVBoxLayout *verticalLayout_11;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_16;
    QSpinBox *MethodKeepGoing;
    QLabel *label_23;
    QPushButton *MethodThatsAll;
    QGroupBox *MethodStandAfterBox;
    QVBoxLayout *verticalLayout_19;
    QSpinBox *MethodStandAfter;
    QLabel *label_52;
    QWidget *stackedWidgetPage2;
    QHBoxLayout *horizontalLayout_17;
    QHBoxLayout *horizontalLayout_15;
    QListView *listView;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_11;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_49;
    QComboBox *StatisticsSensorSelector;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QFormLayout *formLayout;
    QLabel *label_42;
    QLabel *StatisticsRows;
    QLabel *label_43;
    QLabel *StatisticsCombinedError;
    QLabel *label_5;
    QLabel *StatisticsLastTen;
    QLabel *label_44;
    QLabel *StatisticsHandstrokeError;
    QLabel *label_45;
    QLabel *StatisticsBackstrokeError;
    QLabel *label_2;
    QLabel *StatisticsSD;
    QLabel *label_3;
    QLabel *StatisticsHandstrokeSD;
    QLabel *label_4;
    QLabel *StatisticsBackstrokeSD;
    QLabel *label;
    QLabel *StatisticsInside5;
    QLabel *label_46;
    QLabel *StatisticsInside10;
    QLabel *label_47;
    QLabel *StatisticsInside25;
    QLabel *label_48;
    QLabel *StatisticsInside50;
    QPushButton *StatisticsAnalyze;
    StrikingWindow *StrikingGraphic;
    QStatusBar *statusbar;
    QDockWidget *SensorDock;
    QWidget *dockWidgetContents_2;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_12;
    SensorTableWidget *SensorTable;
    QVBoxLayout *verticalLayout_6;
    QSpacerItem *verticalSpacer_11;
    QToolButton *AddSensorButton;
    QSpacerItem *verticalSpacer_10;
    QToolButton *RemoveSensorButton;
    QSpacerItem *verticalSpacer_12;
    SensorListWidget *SensorList;
    QPushButton *SensorClear;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_17;
    QTimeEdit *PealSpeed;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_18;
    QDoubleSpinBox *ChangesPerMinute;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_6;
    QLabel *BellGap;
    QSpacerItem *horizontalSpacer;
    QCheckBox *AudibleCalls;
    QToolBar *toolBar_2;
    QDockWidget *FeedbackDock;
    QWidget *dockWidgetContents_5;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_29;
    QSlider *RingingSpeedFromSensors;
    QSpinBox *spinBox;
    QGroupBox *groupBox_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_26;
    QSlider *SensorErrorMultipler;
    QSpinBox *SensorErrorMultiplier;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1132, 980);
        MainWindow->setMinimumSize(QSize(0, 0));
        actionFile = new QAction(MainWindow);
        actionFile->setObjectName(QStringLiteral("actionFile"));
        actionListen_for_connections = new QAction(MainWindow);
        actionListen_for_connections->setObjectName(QStringLiteral("actionListen_for_connections"));
        actionListen_for_connections->setCheckable(true);
        actionListen_for_connections->setChecked(true);
        actionSetup_local_sensors = new QAction(MainWindow);
        actionSetup_local_sensors->setObjectName(QStringLiteral("actionSetup_local_sensors"));
        actionAbout_Bell_Ringing_Simulator = new QAction(MainWindow);
        actionAbout_Bell_Ringing_Simulator->setObjectName(QStringLiteral("actionAbout_Bell_Ringing_Simulator"));
        actionPreferences = new QAction(MainWindow);
        actionPreferences->setObjectName(QStringLiteral("actionPreferences"));
        actionSensors = new QAction(MainWindow);
        actionSensors->setObjectName(QStringLiteral("actionSensors"));
        actionSensors->setCheckable(true);
        actionSensors->setChecked(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        verticalLayout_12 = new QVBoxLayout(centralwidget);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        RingingStart = new QPushButton(centralwidget);
        RingingStart->setObjectName(QStringLiteral("RingingStart"));
        RingingStart->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(RingingStart);

        RingingAuto = new QPushButton(centralwidget);
        RingingAuto->setObjectName(QStringLiteral("RingingAuto"));
        RingingAuto->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(RingingAuto);

        RingingRounds = new QPushButton(centralwidget);
        RingingRounds->setObjectName(QStringLiteral("RingingRounds"));
        RingingRounds->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(RingingRounds);

        RingingStand = new QPushButton(centralwidget);
        RingingStand->setObjectName(QStringLiteral("RingingStand"));
        RingingStand->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(RingingStand);

        RingingStop = new QPushButton(centralwidget);
        RingingStop->setObjectName(QStringLiteral("RingingStop"));
        RingingStop->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(RingingStop);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout_10->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setObjectName(QStringLiteral("horizontalLayout_24"));
        label_19 = new QLabel(centralwidget);
        label_19->setObjectName(QStringLiteral("label_19"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_19->sizePolicy().hasHeightForWidth());
        label_19->setSizePolicy(sizePolicy1);

        horizontalLayout_24->addWidget(label_19);

        NumberOfBells = new QSpinBox(centralwidget);
        NumberOfBells->setObjectName(QStringLiteral("NumberOfBells"));
        sizePolicy1.setHeightForWidth(NumberOfBells->sizePolicy().hasHeightForWidth());
        NumberOfBells->setSizePolicy(sizePolicy1);
        NumberOfBells->setFrame(true);
        NumberOfBells->setAccelerated(false);
        NumberOfBells->setMinimum(3);
        NumberOfBells->setMaximum(16);
        NumberOfBells->setValue(8);

        horizontalLayout_24->addWidget(NumberOfBells);

        label_20 = new QLabel(centralwidget);
        label_20->setObjectName(QStringLiteral("label_20"));
        sizePolicy1.setHeightForWidth(label_20->sizePolicy().hasHeightForWidth());
        label_20->setSizePolicy(sizePolicy1);

        horizontalLayout_24->addWidget(label_20);

        CCStyle = new QComboBox(centralwidget);
        CCStyle->setObjectName(QStringLiteral("CCStyle"));
        CCStyle->setEnabled(false);
        sizePolicy1.setHeightForWidth(CCStyle->sizePolicy().hasHeightForWidth());
        CCStyle->setSizePolicy(sizePolicy1);
        CCStyle->setDuplicatesEnabled(false);

        horizontalLayout_24->addWidget(CCStyle);


        verticalLayout_2->addLayout(horizontalLayout_24);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy2);
        groupBox_3->setMinimumSize(QSize(0, 30));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        QBrush brush2(QColor(127, 127, 127, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush2);
        QBrush brush3(QColor(170, 170, 170, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        QBrush brush4(QColor(255, 255, 220, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        groupBox_3->setPalette(palette);
        groupBox_3->setAutoFillBackground(true);
        horizontalLayout_2 = new QHBoxLayout(groupBox_3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        CC1 = new QLabel(groupBox_3);
        CC1->setObjectName(QStringLiteral("CC1"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(CC1->sizePolicy().hasHeightForWidth());
        CC1->setSizePolicy(sizePolicy3);
        QFont font;
        font.setFamily(QStringLiteral("Times New Roman"));
        font.setPointSize(28);
        font.setKerning(false);
        CC1->setFont(font);
        CC1->setAutoFillBackground(false);
        CC1->setTextFormat(Qt::PlainText);
        CC1->setScaledContents(true);
        CC1->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC1);

        CC2 = new QLabel(groupBox_3);
        CC2->setObjectName(QStringLiteral("CC2"));
        sizePolicy3.setHeightForWidth(CC2->sizePolicy().hasHeightForWidth());
        CC2->setSizePolicy(sizePolicy3);
        CC2->setFont(font);
        CC2->setAutoFillBackground(false);
        CC2->setTextFormat(Qt::PlainText);
        CC2->setScaledContents(true);
        CC2->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC2);

        CC3 = new QLabel(groupBox_3);
        CC3->setObjectName(QStringLiteral("CC3"));
        sizePolicy3.setHeightForWidth(CC3->sizePolicy().hasHeightForWidth());
        CC3->setSizePolicy(sizePolicy3);
        CC3->setFont(font);
        CC3->setAutoFillBackground(false);
        CC3->setTextFormat(Qt::PlainText);
        CC3->setScaledContents(true);
        CC3->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC3);

        CC4 = new QLabel(groupBox_3);
        CC4->setObjectName(QStringLiteral("CC4"));
        sizePolicy3.setHeightForWidth(CC4->sizePolicy().hasHeightForWidth());
        CC4->setSizePolicy(sizePolicy3);
        CC4->setFont(font);
        CC4->setAutoFillBackground(false);
        CC4->setTextFormat(Qt::PlainText);
        CC4->setScaledContents(true);
        CC4->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC4);

        CC5 = new QLabel(groupBox_3);
        CC5->setObjectName(QStringLiteral("CC5"));
        sizePolicy3.setHeightForWidth(CC5->sizePolicy().hasHeightForWidth());
        CC5->setSizePolicy(sizePolicy3);
        CC5->setFont(font);
        CC5->setAutoFillBackground(false);
        CC5->setTextFormat(Qt::PlainText);
        CC5->setScaledContents(true);
        CC5->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC5);

        CC6 = new QLabel(groupBox_3);
        CC6->setObjectName(QStringLiteral("CC6"));
        sizePolicy3.setHeightForWidth(CC6->sizePolicy().hasHeightForWidth());
        CC6->setSizePolicy(sizePolicy3);
        CC6->setFont(font);
        CC6->setAutoFillBackground(false);
        CC6->setTextFormat(Qt::PlainText);
        CC6->setScaledContents(true);
        CC6->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC6);

        CC7 = new QLabel(groupBox_3);
        CC7->setObjectName(QStringLiteral("CC7"));
        sizePolicy3.setHeightForWidth(CC7->sizePolicy().hasHeightForWidth());
        CC7->setSizePolicy(sizePolicy3);
        CC7->setFont(font);
        CC7->setAutoFillBackground(false);
        CC7->setTextFormat(Qt::PlainText);
        CC7->setScaledContents(true);
        CC7->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC7);

        CC8 = new QLabel(groupBox_3);
        CC8->setObjectName(QStringLiteral("CC8"));
        sizePolicy3.setHeightForWidth(CC8->sizePolicy().hasHeightForWidth());
        CC8->setSizePolicy(sizePolicy3);
        CC8->setFont(font);
        CC8->setAutoFillBackground(false);
        CC8->setTextFormat(Qt::PlainText);
        CC8->setScaledContents(true);
        CC8->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC8);

        CC9 = new QLabel(groupBox_3);
        CC9->setObjectName(QStringLiteral("CC9"));
        sizePolicy3.setHeightForWidth(CC9->sizePolicy().hasHeightForWidth());
        CC9->setSizePolicy(sizePolicy3);
        CC9->setFont(font);
        CC9->setAutoFillBackground(false);
        CC9->setTextFormat(Qt::PlainText);
        CC9->setScaledContents(true);
        CC9->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC9);

        CC10 = new QLabel(groupBox_3);
        CC10->setObjectName(QStringLiteral("CC10"));
        sizePolicy3.setHeightForWidth(CC10->sizePolicy().hasHeightForWidth());
        CC10->setSizePolicy(sizePolicy3);
        CC10->setFont(font);
        CC10->setAutoFillBackground(false);
        CC10->setTextFormat(Qt::PlainText);
        CC10->setScaledContents(true);
        CC10->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC10);

        CC11 = new QLabel(groupBox_3);
        CC11->setObjectName(QStringLiteral("CC11"));
        sizePolicy3.setHeightForWidth(CC11->sizePolicy().hasHeightForWidth());
        CC11->setSizePolicy(sizePolicy3);
        CC11->setFont(font);
        CC11->setAutoFillBackground(false);
        CC11->setTextFormat(Qt::PlainText);
        CC11->setScaledContents(true);
        CC11->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC11);

        CC12 = new QLabel(groupBox_3);
        CC12->setObjectName(QStringLiteral("CC12"));
        sizePolicy3.setHeightForWidth(CC12->sizePolicy().hasHeightForWidth());
        CC12->setSizePolicy(sizePolicy3);
        CC12->setFont(font);
        CC12->setAutoFillBackground(false);
        CC12->setTextFormat(Qt::PlainText);
        CC12->setScaledContents(true);
        CC12->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC12);

        CC13 = new QLabel(groupBox_3);
        CC13->setObjectName(QStringLiteral("CC13"));
        sizePolicy3.setHeightForWidth(CC13->sizePolicy().hasHeightForWidth());
        CC13->setSizePolicy(sizePolicy3);
        CC13->setFont(font);
        CC13->setAutoFillBackground(false);
        CC13->setTextFormat(Qt::PlainText);
        CC13->setScaledContents(true);
        CC13->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC13);

        CC14 = new QLabel(groupBox_3);
        CC14->setObjectName(QStringLiteral("CC14"));
        sizePolicy3.setHeightForWidth(CC14->sizePolicy().hasHeightForWidth());
        CC14->setSizePolicy(sizePolicy3);
        CC14->setFont(font);
        CC14->setAutoFillBackground(false);
        CC14->setTextFormat(Qt::PlainText);
        CC14->setScaledContents(true);
        CC14->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC14);

        CC15 = new QLabel(groupBox_3);
        CC15->setObjectName(QStringLiteral("CC15"));
        sizePolicy3.setHeightForWidth(CC15->sizePolicy().hasHeightForWidth());
        CC15->setSizePolicy(sizePolicy3);
        CC15->setFont(font);
        CC15->setAutoFillBackground(false);
        CC15->setTextFormat(Qt::PlainText);
        CC15->setScaledContents(true);
        CC15->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC15);

        CC16 = new QLabel(groupBox_3);
        CC16->setObjectName(QStringLiteral("CC16"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(CC16->sizePolicy().hasHeightForWidth());
        CC16->setSizePolicy(sizePolicy4);
        CC16->setFont(font);
        CC16->setAutoFillBackground(false);
        CC16->setTextFormat(Qt::PlainText);
        CC16->setScaledContents(true);
        CC16->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(CC16);

        horizontalLayout_2->setStretch(0, 2);
        horizontalLayout_2->setStretch(1, 2);
        horizontalLayout_2->setStretch(2, 2);
        horizontalLayout_2->setStretch(3, 2);
        horizontalLayout_2->setStretch(4, 2);
        horizontalLayout_2->setStretch(5, 2);
        horizontalLayout_2->setStretch(6, 2);
        horizontalLayout_2->setStretch(7, 2);
        horizontalLayout_2->setStretch(8, 2);
        horizontalLayout_2->setStretch(9, 2);
        horizontalLayout_2->setStretch(10, 2);
        horizontalLayout_2->setStretch(11, 2);
        horizontalLayout_2->setStretch(12, 2);
        horizontalLayout_2->setStretch(13, 2);
        horizontalLayout_2->setStretch(14, 2);
        horizontalLayout_2->setStretch(15, 2);

        verticalLayout_2->addWidget(groupBox_3);

        groupBox_7 = new QGroupBox(centralwidget);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(groupBox_7->sizePolicy().hasHeightForWidth());
        groupBox_7->setSizePolicy(sizePolicy5);
        groupBox_7->setFlat(true);
        horizontalLayout_4 = new QHBoxLayout(groupBox_7);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        SelectCallChange = new QRadioButton(groupBox_7);
        SelectCallChange->setObjectName(QStringLiteral("SelectCallChange"));
        SelectCallChange->setChecked(true);

        horizontalLayout_4->addWidget(SelectCallChange);

        SelectDodge = new QRadioButton(groupBox_7);
        SelectDodge->setObjectName(QStringLiteral("SelectDodge"));

        horizontalLayout_4->addWidget(SelectDodge);

        SelectRightPlaces = new QRadioButton(groupBox_7);
        SelectRightPlaces->setObjectName(QStringLiteral("SelectRightPlaces"));

        horizontalLayout_4->addWidget(SelectRightPlaces);

        SelectWrongPlaces = new QRadioButton(groupBox_7);
        SelectWrongPlaces->setObjectName(QStringLiteral("SelectWrongPlaces"));

        horizontalLayout_4->addWidget(SelectWrongPlaces);


        verticalLayout_2->addWidget(groupBox_7);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalSpacer_5 = new QSpacerItem(13, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(verticalSpacer_5);

        Call12 = new QPushButton(centralwidget);
        Call12->setObjectName(QStringLiteral("Call12"));
        Call12->setEnabled(true);
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(Call12->sizePolicy().hasHeightForWidth());
        Call12->setSizePolicy(sizePolicy6);
        Call12->setMinimumSize(QSize(10, 30));
        Call12->setAutoFillBackground(false);
        Call12->setAutoDefault(false);
        Call12->setFlat(false);

        horizontalLayout->addWidget(Call12);

        Call23 = new QPushButton(centralwidget);
        Call23->setObjectName(QStringLiteral("Call23"));
        sizePolicy6.setHeightForWidth(Call23->sizePolicy().hasHeightForWidth());
        Call23->setSizePolicy(sizePolicy6);
        Call23->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call23);

        Call34 = new QPushButton(centralwidget);
        Call34->setObjectName(QStringLiteral("Call34"));
        sizePolicy6.setHeightForWidth(Call34->sizePolicy().hasHeightForWidth());
        Call34->setSizePolicy(sizePolicy6);
        Call34->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call34);

        Call45 = new QPushButton(centralwidget);
        Call45->setObjectName(QStringLiteral("Call45"));
        sizePolicy6.setHeightForWidth(Call45->sizePolicy().hasHeightForWidth());
        Call45->setSizePolicy(sizePolicy6);
        Call45->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call45);

        Call56 = new QPushButton(centralwidget);
        Call56->setObjectName(QStringLiteral("Call56"));
        sizePolicy6.setHeightForWidth(Call56->sizePolicy().hasHeightForWidth());
        Call56->setSizePolicy(sizePolicy6);
        Call56->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call56);

        Call67 = new QPushButton(centralwidget);
        Call67->setObjectName(QStringLiteral("Call67"));
        sizePolicy6.setHeightForWidth(Call67->sizePolicy().hasHeightForWidth());
        Call67->setSizePolicy(sizePolicy6);
        Call67->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call67);

        Call78 = new QPushButton(centralwidget);
        Call78->setObjectName(QStringLiteral("Call78"));
        sizePolicy6.setHeightForWidth(Call78->sizePolicy().hasHeightForWidth());
        Call78->setSizePolicy(sizePolicy6);
        Call78->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call78);

        Call89 = new QPushButton(centralwidget);
        Call89->setObjectName(QStringLiteral("Call89"));
        sizePolicy6.setHeightForWidth(Call89->sizePolicy().hasHeightForWidth());
        Call89->setSizePolicy(sizePolicy6);
        Call89->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call89);

        Call90 = new QPushButton(centralwidget);
        Call90->setObjectName(QStringLiteral("Call90"));
        sizePolicy6.setHeightForWidth(Call90->sizePolicy().hasHeightForWidth());
        Call90->setSizePolicy(sizePolicy6);
        Call90->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call90);

        Call0E = new QPushButton(centralwidget);
        Call0E->setObjectName(QStringLiteral("Call0E"));
        sizePolicy6.setHeightForWidth(Call0E->sizePolicy().hasHeightForWidth());
        Call0E->setSizePolicy(sizePolicy6);
        Call0E->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(Call0E);

        CallET = new QPushButton(centralwidget);
        CallET->setObjectName(QStringLiteral("CallET"));
        sizePolicy6.setHeightForWidth(CallET->sizePolicy().hasHeightForWidth());
        CallET->setSizePolicy(sizePolicy6);
        CallET->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(CallET);

        CallTA = new QPushButton(centralwidget);
        CallTA->setObjectName(QStringLiteral("CallTA"));
        sizePolicy6.setHeightForWidth(CallTA->sizePolicy().hasHeightForWidth());
        CallTA->setSizePolicy(sizePolicy6);
        CallTA->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(CallTA);

        CallAB = new QPushButton(centralwidget);
        CallAB->setObjectName(QStringLiteral("CallAB"));
        sizePolicy6.setHeightForWidth(CallAB->sizePolicy().hasHeightForWidth());
        CallAB->setSizePolicy(sizePolicy6);
        CallAB->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(CallAB);

        CallBC = new QPushButton(centralwidget);
        CallBC->setObjectName(QStringLiteral("CallBC"));
        sizePolicy6.setHeightForWidth(CallBC->sizePolicy().hasHeightForWidth());
        CallBC->setSizePolicy(sizePolicy6);
        CallBC->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(CallBC);

        CallCD = new QPushButton(centralwidget);
        CallCD->setObjectName(QStringLiteral("CallCD"));
        sizePolicy6.setHeightForWidth(CallCD->sizePolicy().hasHeightForWidth());
        CallCD->setSizePolicy(sizePolicy6);
        CallCD->setMinimumSize(QSize(10, 30));

        horizontalLayout->addWidget(CallCD);

        verticalSpacer_7 = new QSpacerItem(13, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(verticalSpacer_7);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 2);
        horizontalLayout->setStretch(2, 2);
        horizontalLayout->setStretch(3, 2);
        horizontalLayout->setStretch(4, 2);
        horizontalLayout->setStretch(5, 2);
        horizontalLayout->setStretch(6, 2);
        horizontalLayout->setStretch(7, 2);
        horizontalLayout->setStretch(8, 2);
        horizontalLayout->setStretch(9, 2);
        horizontalLayout->setStretch(10, 2);
        horizontalLayout->setStretch(11, 2);
        horizontalLayout->setStretch(12, 2);
        horizontalLayout->setStretch(13, 2);
        horizontalLayout->setStretch(14, 2);
        horizontalLayout->setStretch(15, 2);
        horizontalLayout->setStretch(16, 1);

        verticalLayout_2->addLayout(horizontalLayout);

        verticalLayout_2->setStretch(1, 1);

        horizontalLayout_10->addLayout(verticalLayout_2);


        verticalLayout_12->addLayout(horizontalLayout_10);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_9 = new QVBoxLayout(frame);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        RingMethod = new QPushButton(frame);
        RingMethod->setObjectName(QStringLiteral("RingMethod"));
        RingMethod->setCheckable(true);
        RingMethod->setChecked(true);
        RingMethod->setAutoExclusive(true);

        verticalLayout_9->addWidget(RingMethod);

        RingTouch = new QPushButton(frame);
        RingTouch->setObjectName(QStringLiteral("RingTouch"));
        RingTouch->setCheckable(true);
        RingTouch->setAutoExclusive(true);

        verticalLayout_9->addWidget(RingTouch);


        verticalLayout_7->addWidget(frame);

        verticalSpacer_3 = new QSpacerItem(13, 13, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_3);

        MethodGo = new QPushButton(centralwidget);
        MethodGo->setObjectName(QStringLiteral("MethodGo"));
        MethodGo->setEnabled(false);
        MethodGo->setMinimumSize(QSize(0, 30));

        verticalLayout_7->addWidget(MethodGo);

        MethodGoAfterBox = new QGroupBox(centralwidget);
        MethodGoAfterBox->setObjectName(QStringLiteral("MethodGoAfterBox"));
        QSizePolicy sizePolicy7(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(MethodGoAfterBox->sizePolicy().hasHeightForWidth());
        MethodGoAfterBox->setSizePolicy(sizePolicy7);
        MethodGoAfterBox->setCheckable(false);
        MethodGoAfterBox->setChecked(false);
        verticalLayout_18 = new QVBoxLayout(MethodGoAfterBox);
        verticalLayout_18->setObjectName(QStringLiteral("verticalLayout_18"));
        MethodGoDelay = new QPushButton(MethodGoAfterBox);
        MethodGoDelay->setObjectName(QStringLiteral("MethodGoDelay"));
        MethodGoDelay->setEnabled(false);
        MethodGoDelay->setMinimumSize(QSize(0, 30));
        MethodGoDelay->setCheckable(true);

        verticalLayout_18->addWidget(MethodGoDelay);

        MethodGoAfter = new QSpinBox(MethodGoAfterBox);
        MethodGoAfter->setObjectName(QStringLiteral("MethodGoAfter"));
        MethodGoAfter->setMinimum(1);
        MethodGoAfter->setValue(6);

        verticalLayout_18->addWidget(MethodGoAfter);

        label_50 = new QLabel(MethodGoAfterBox);
        label_50->setObjectName(QStringLiteral("label_50"));
        label_50->setAlignment(Qt::AlignCenter);

        verticalLayout_18->addWidget(label_50);


        verticalLayout_7->addWidget(MethodGoAfterBox);


        horizontalLayout_9->addLayout(verticalLayout_7);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setEnabled(true);
        QSizePolicy sizePolicy8(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy8.setHorizontalStretch(1);
        sizePolicy8.setVerticalStretch(1);
        sizePolicy8.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy8);
        stackedWidget->setFrameShape(QFrame::Panel);
        stackedWidget->setFrameShadow(QFrame::Raised);
        stackedWidgetPage1 = new QWidget();
        stackedWidgetPage1->setObjectName(QStringLiteral("stackedWidgetPage1"));
        horizontalLayout_7 = new QHBoxLayout(stackedWidgetPage1);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        MethodTenorBehind = new QCheckBox(stackedWidgetPage1);
        MethodTenorBehind->setObjectName(QStringLiteral("MethodTenorBehind"));
        MethodTenorBehind->setChecked(true);
        MethodTenorBehind->setTristate(false);

        verticalLayout_4->addWidget(MethodTenorBehind);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_22 = new QLabel(stackedWidgetPage1);
        label_22->setObjectName(QStringLiteral("label_22"));

        gridLayout->addWidget(label_22, 0, 0, 1, 1);

        CurrentMethod = new QLabel(stackedWidgetPage1);
        CurrentMethod->setObjectName(QStringLiteral("CurrentMethod"));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette1.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush);
        palette1.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        CurrentMethod->setPalette(palette1);
        CurrentMethod->setCursor(QCursor(Qt::IBeamCursor));
        CurrentMethod->setAutoFillBackground(true);
        CurrentMethod->setFrameShape(QFrame::Box);

        gridLayout->addWidget(CurrentMethod, 0, 1, 1, 1);

        label_24 = new QLabel(stackedWidgetPage1);
        label_24->setObjectName(QStringLiteral("label_24"));

        gridLayout->addWidget(label_24, 1, 0, 1, 1);

        NextMethod = new QLabel(stackedWidgetPage1);
        NextMethod->setObjectName(QStringLiteral("NextMethod"));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette2.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush);
        palette2.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        NextMethod->setPalette(palette2);
        NextMethod->setCursor(QCursor(Qt::IBeamCursor));
        NextMethod->setAutoFillBackground(true);
        NextMethod->setFrameShape(QFrame::Box);

        gridLayout->addWidget(NextMethod, 1, 1, 1, 1);

        gridLayout->setColumnStretch(1, 1);

        verticalLayout_4->addLayout(gridLayout);

        SelectMethod = new QPushButton(stackedWidgetPage1);
        SelectMethod->setObjectName(QStringLiteral("SelectMethod"));

        verticalLayout_4->addWidget(SelectMethod);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        MethodCallRepeat = new QCheckBox(stackedWidgetPage1);
        MethodCallRepeat->setObjectName(QStringLiteral("MethodCallRepeat"));

        verticalLayout->addWidget(MethodCallRepeat);

        MethodRandomCalls = new QCheckBox(stackedWidgetPage1);
        MethodRandomCalls->setObjectName(QStringLiteral("MethodRandomCalls"));

        verticalLayout->addWidget(MethodRandomCalls);


        horizontalLayout_6->addLayout(verticalLayout);


        verticalLayout_4->addLayout(horizontalLayout_6);


        horizontalLayout_7->addLayout(verticalLayout_4);

        groupBox_9 = new QGroupBox(stackedWidgetPage1);
        groupBox_9->setObjectName(QStringLiteral("groupBox_9"));
        verticalLayout_14 = new QVBoxLayout(groupBox_9);
        verticalLayout_14->setObjectName(QStringLiteral("verticalLayout_14"));
        MethodCall0 = new QPushButton(groupBox_9);
        MethodCall0->setObjectName(QStringLiteral("MethodCall0"));
        MethodCall0->setMinimumSize(QSize(0, 30));
        MethodCall0->setCheckable(true);

        verticalLayout_14->addWidget(MethodCall0);

        MethodCall1 = new QPushButton(groupBox_9);
        MethodCall1->setObjectName(QStringLiteral("MethodCall1"));
        MethodCall1->setMinimumSize(QSize(0, 30));
        MethodCall1->setCheckable(true);

        verticalLayout_14->addWidget(MethodCall1);

        MethodCall2 = new QPushButton(groupBox_9);
        MethodCall2->setObjectName(QStringLiteral("MethodCall2"));
        MethodCall2->setEnabled(false);
        MethodCall2->setMinimumSize(QSize(0, 30));
        MethodCall2->setCheckable(true);

        verticalLayout_14->addWidget(MethodCall2);

        MethodCall3 = new QPushButton(groupBox_9);
        MethodCall3->setObjectName(QStringLiteral("MethodCall3"));
        MethodCall3->setEnabled(false);
        MethodCall3->setMinimumSize(QSize(0, 30));
        MethodCall3->setCheckable(true);

        verticalLayout_14->addWidget(MethodCall3);

        MethodSplice = new QPushButton(groupBox_9);
        MethodSplice->setObjectName(QStringLiteral("MethodSplice"));
        MethodSplice->setEnabled(false);
        MethodSplice->setMinimumSize(QSize(0, 30));
        MethodSplice->setCheckable(true);

        verticalLayout_14->addWidget(MethodSplice);


        horizontalLayout_7->addWidget(groupBox_9);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        groupBox_2 = new QGroupBox(stackedWidgetPage1);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        sizePolicy7.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy7);
        verticalLayout_16 = new QVBoxLayout(groupBox_2);
        verticalLayout_16->setObjectName(QStringLiteral("verticalLayout_16"));
        MethodKeepGoing = new QSpinBox(groupBox_2);
        MethodKeepGoing->setObjectName(QStringLiteral("MethodKeepGoing"));
        MethodKeepGoing->setValue(1);

        verticalLayout_16->addWidget(MethodKeepGoing);

        label_23 = new QLabel(groupBox_2);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setAlignment(Qt::AlignCenter);

        verticalLayout_16->addWidget(label_23);


        verticalLayout_11->addWidget(groupBox_2);

        MethodThatsAll = new QPushButton(stackedWidgetPage1);
        MethodThatsAll->setObjectName(QStringLiteral("MethodThatsAll"));
        MethodThatsAll->setEnabled(false);
        MethodThatsAll->setMinimumSize(QSize(0, 30));
        MethodThatsAll->setCheckable(true);

        verticalLayout_11->addWidget(MethodThatsAll);

        MethodStandAfterBox = new QGroupBox(stackedWidgetPage1);
        MethodStandAfterBox->setObjectName(QStringLiteral("MethodStandAfterBox"));
        sizePolicy7.setHeightForWidth(MethodStandAfterBox->sizePolicy().hasHeightForWidth());
        MethodStandAfterBox->setSizePolicy(sizePolicy7);
        MethodStandAfterBox->setCheckable(true);
        MethodStandAfterBox->setChecked(true);
        verticalLayout_19 = new QVBoxLayout(MethodStandAfterBox);
        verticalLayout_19->setObjectName(QStringLiteral("verticalLayout_19"));
        MethodStandAfter = new QSpinBox(MethodStandAfterBox);
        MethodStandAfter->setObjectName(QStringLiteral("MethodStandAfter"));
        MethodStandAfter->setMinimum(1);
        MethodStandAfter->setValue(2);

        verticalLayout_19->addWidget(MethodStandAfter);

        label_52 = new QLabel(MethodStandAfterBox);
        label_52->setObjectName(QStringLiteral("label_52"));
        label_52->setAlignment(Qt::AlignCenter);

        verticalLayout_19->addWidget(label_52);


        verticalLayout_11->addWidget(MethodStandAfterBox);


        horizontalLayout_7->addLayout(verticalLayout_11);

        stackedWidget->addWidget(stackedWidgetPage1);
        stackedWidgetPage2 = new QWidget();
        stackedWidgetPage2->setObjectName(QStringLiteral("stackedWidgetPage2"));
        horizontalLayout_17 = new QHBoxLayout(stackedWidgetPage2);
        horizontalLayout_17->setObjectName(QStringLiteral("horizontalLayout_17"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        listView = new QListView(stackedWidgetPage2);
        listView->setObjectName(QStringLiteral("listView"));

        horizontalLayout_15->addWidget(listView);


        horizontalLayout_17->addLayout(horizontalLayout_15);

        stackedWidget->addWidget(stackedWidgetPage2);

        horizontalLayout_9->addWidget(stackedWidget);


        verticalLayout_12->addLayout(horizontalLayout_9);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout_11 = new QHBoxLayout(groupBox);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        groupBox_6 = new QGroupBox(groupBox);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        verticalLayout_10 = new QVBoxLayout(groupBox_6);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        label_49 = new QLabel(groupBox_6);
        label_49->setObjectName(QStringLiteral("label_49"));
        label_49->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_13->addWidget(label_49);

        StatisticsSensorSelector = new QComboBox(groupBox_6);
        StatisticsSensorSelector->setObjectName(QStringLiteral("StatisticsSensorSelector"));
        QSizePolicy sizePolicy9(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy9.setHorizontalStretch(0);
        sizePolicy9.setVerticalStretch(0);
        sizePolicy9.setHeightForWidth(StatisticsSensorSelector->sizePolicy().hasHeightForWidth());
        StatisticsSensorSelector->setSizePolicy(sizePolicy9);

        horizontalLayout_13->addWidget(StatisticsSensorSelector);


        verticalLayout_10->addLayout(horizontalLayout_13);

        scrollArea = new QScrollArea(groupBox_6);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        sizePolicy4.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy4);
        scrollArea->setMinimumSize(QSize(250, 0));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 227, 302));
        formLayout = new QFormLayout(scrollAreaWidgetContents);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label_42 = new QLabel(scrollAreaWidgetContents);
        label_42->setObjectName(QStringLiteral("label_42"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_42);

        StatisticsRows = new QLabel(scrollAreaWidgetContents);
        StatisticsRows->setObjectName(QStringLiteral("StatisticsRows"));
        sizePolicy9.setHeightForWidth(StatisticsRows->sizePolicy().hasHeightForWidth());
        StatisticsRows->setSizePolicy(sizePolicy9);
        StatisticsRows->setMinimumSize(QSize(100, 20));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette3.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette3.setBrush(QPalette::Active, QPalette::Text, brush);
        palette3.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette3.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette3.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette3.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette3.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette3.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette3.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette3.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette3.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette3.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette3.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsRows->setPalette(palette3);
        StatisticsRows->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsRows->setAutoFillBackground(true);
        StatisticsRows->setFrameShape(QFrame::Box);
        StatisticsRows->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(0, QFormLayout::FieldRole, StatisticsRows);

        label_43 = new QLabel(scrollAreaWidgetContents);
        label_43->setObjectName(QStringLiteral("label_43"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_43);

        StatisticsCombinedError = new QLabel(scrollAreaWidgetContents);
        StatisticsCombinedError->setObjectName(QStringLiteral("StatisticsCombinedError"));
        sizePolicy9.setHeightForWidth(StatisticsCombinedError->sizePolicy().hasHeightForWidth());
        StatisticsCombinedError->setSizePolicy(sizePolicy9);
        StatisticsCombinedError->setMinimumSize(QSize(100, 20));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette4.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette4.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette4.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette4.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette4.setBrush(QPalette::Active, QPalette::Text, brush);
        palette4.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette4.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette4.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette4.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette4.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette4.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette4.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette4.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette4.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette4.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette4.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette4.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette4.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette4.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsCombinedError->setPalette(palette4);
        StatisticsCombinedError->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsCombinedError->setAutoFillBackground(true);
        StatisticsCombinedError->setFrameShape(QFrame::Box);
        StatisticsCombinedError->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(1, QFormLayout::FieldRole, StatisticsCombinedError);

        label_5 = new QLabel(scrollAreaWidgetContents);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        StatisticsLastTen = new QLabel(scrollAreaWidgetContents);
        StatisticsLastTen->setObjectName(QStringLiteral("StatisticsLastTen"));
        sizePolicy9.setHeightForWidth(StatisticsLastTen->sizePolicy().hasHeightForWidth());
        StatisticsLastTen->setSizePolicy(sizePolicy9);
        StatisticsLastTen->setMinimumSize(QSize(100, 20));
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette5.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette5.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette5.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette5.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette5.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette5.setBrush(QPalette::Active, QPalette::Text, brush);
        palette5.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette5.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette5.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette5.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette5.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette5.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette5.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette5.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette5.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette5.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette5.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette5.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette5.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette5.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsLastTen->setPalette(palette5);
        StatisticsLastTen->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsLastTen->setAutoFillBackground(true);
        StatisticsLastTen->setFrameShape(QFrame::Box);
        StatisticsLastTen->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(2, QFormLayout::FieldRole, StatisticsLastTen);

        label_44 = new QLabel(scrollAreaWidgetContents);
        label_44->setObjectName(QStringLiteral("label_44"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_44);

        StatisticsHandstrokeError = new QLabel(scrollAreaWidgetContents);
        StatisticsHandstrokeError->setObjectName(QStringLiteral("StatisticsHandstrokeError"));
        sizePolicy9.setHeightForWidth(StatisticsHandstrokeError->sizePolicy().hasHeightForWidth());
        StatisticsHandstrokeError->setSizePolicy(sizePolicy9);
        StatisticsHandstrokeError->setMinimumSize(QSize(100, 20));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette6.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette6.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette6.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette6.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette6.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette6.setBrush(QPalette::Active, QPalette::Text, brush);
        palette6.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette6.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette6.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette6.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette6.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette6.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette6.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette6.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette6.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette6.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette6.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette6.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette6.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette6.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette6.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsHandstrokeError->setPalette(palette6);
        StatisticsHandstrokeError->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsHandstrokeError->setAutoFillBackground(true);
        StatisticsHandstrokeError->setFrameShape(QFrame::Box);
        StatisticsHandstrokeError->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(3, QFormLayout::FieldRole, StatisticsHandstrokeError);

        label_45 = new QLabel(scrollAreaWidgetContents);
        label_45->setObjectName(QStringLiteral("label_45"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_45);

        StatisticsBackstrokeError = new QLabel(scrollAreaWidgetContents);
        StatisticsBackstrokeError->setObjectName(QStringLiteral("StatisticsBackstrokeError"));
        sizePolicy9.setHeightForWidth(StatisticsBackstrokeError->sizePolicy().hasHeightForWidth());
        StatisticsBackstrokeError->setSizePolicy(sizePolicy9);
        StatisticsBackstrokeError->setMinimumSize(QSize(100, 20));
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette7.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette7.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette7.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette7.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette7.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette7.setBrush(QPalette::Active, QPalette::Text, brush);
        palette7.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette7.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette7.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette7.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette7.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette7.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette7.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette7.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette7.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette7.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette7.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette7.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette7.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette7.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette7.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette7.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsBackstrokeError->setPalette(palette7);
        StatisticsBackstrokeError->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsBackstrokeError->setAutoFillBackground(true);
        StatisticsBackstrokeError->setFrameShape(QFrame::Box);
        StatisticsBackstrokeError->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(4, QFormLayout::FieldRole, StatisticsBackstrokeError);

        label_2 = new QLabel(scrollAreaWidgetContents);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_2);

        StatisticsSD = new QLabel(scrollAreaWidgetContents);
        StatisticsSD->setObjectName(QStringLiteral("StatisticsSD"));
        sizePolicy9.setHeightForWidth(StatisticsSD->sizePolicy().hasHeightForWidth());
        StatisticsSD->setSizePolicy(sizePolicy9);
        StatisticsSD->setMinimumSize(QSize(100, 20));
        QPalette palette8;
        palette8.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette8.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette8.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette8.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette8.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette8.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette8.setBrush(QPalette::Active, QPalette::Text, brush);
        palette8.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette8.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette8.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette8.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette8.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette8.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette8.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette8.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette8.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette8.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette8.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette8.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette8.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette8.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette8.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette8.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette8.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette8.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette8.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette8.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette8.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsSD->setPalette(palette8);
        StatisticsSD->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsSD->setAutoFillBackground(true);
        StatisticsSD->setFrameShape(QFrame::Box);
        StatisticsSD->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(5, QFormLayout::FieldRole, StatisticsSD);

        label_3 = new QLabel(scrollAreaWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_3);

        StatisticsHandstrokeSD = new QLabel(scrollAreaWidgetContents);
        StatisticsHandstrokeSD->setObjectName(QStringLiteral("StatisticsHandstrokeSD"));
        sizePolicy9.setHeightForWidth(StatisticsHandstrokeSD->sizePolicy().hasHeightForWidth());
        StatisticsHandstrokeSD->setSizePolicy(sizePolicy9);
        StatisticsHandstrokeSD->setMinimumSize(QSize(100, 20));
        QPalette palette9;
        palette9.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette9.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette9.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette9.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette9.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette9.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette9.setBrush(QPalette::Active, QPalette::Text, brush);
        palette9.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette9.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette9.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette9.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette9.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette9.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette9.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette9.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette9.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette9.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette9.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette9.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette9.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette9.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette9.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette9.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette9.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette9.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette9.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette9.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette9.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsHandstrokeSD->setPalette(palette9);
        StatisticsHandstrokeSD->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsHandstrokeSD->setAutoFillBackground(true);
        StatisticsHandstrokeSD->setFrameShape(QFrame::Box);
        StatisticsHandstrokeSD->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(6, QFormLayout::FieldRole, StatisticsHandstrokeSD);

        label_4 = new QLabel(scrollAreaWidgetContents);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_4);

        StatisticsBackstrokeSD = new QLabel(scrollAreaWidgetContents);
        StatisticsBackstrokeSD->setObjectName(QStringLiteral("StatisticsBackstrokeSD"));
        sizePolicy9.setHeightForWidth(StatisticsBackstrokeSD->sizePolicy().hasHeightForWidth());
        StatisticsBackstrokeSD->setSizePolicy(sizePolicy9);
        StatisticsBackstrokeSD->setMinimumSize(QSize(100, 20));
        QPalette palette10;
        palette10.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette10.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette10.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette10.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette10.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette10.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette10.setBrush(QPalette::Active, QPalette::Text, brush);
        palette10.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette10.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette10.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette10.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette10.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette10.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette10.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette10.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette10.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette10.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette10.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette10.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette10.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette10.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette10.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette10.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette10.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette10.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette10.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette10.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette10.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette10.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette10.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette10.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette10.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette10.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette10.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette10.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette10.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette10.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette10.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette10.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette10.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsBackstrokeSD->setPalette(palette10);
        StatisticsBackstrokeSD->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsBackstrokeSD->setAutoFillBackground(true);
        StatisticsBackstrokeSD->setFrameShape(QFrame::Box);
        StatisticsBackstrokeSD->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(7, QFormLayout::FieldRole, StatisticsBackstrokeSD);

        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label);

        StatisticsInside5 = new QLabel(scrollAreaWidgetContents);
        StatisticsInside5->setObjectName(QStringLiteral("StatisticsInside5"));
        sizePolicy9.setHeightForWidth(StatisticsInside5->sizePolicy().hasHeightForWidth());
        StatisticsInside5->setSizePolicy(sizePolicy9);
        StatisticsInside5->setMinimumSize(QSize(100, 20));
        QPalette palette11;
        palette11.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette11.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette11.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette11.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette11.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette11.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette11.setBrush(QPalette::Active, QPalette::Text, brush);
        palette11.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette11.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette11.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette11.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette11.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette11.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette11.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette11.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette11.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette11.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette11.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette11.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette11.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette11.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette11.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette11.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette11.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette11.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette11.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette11.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette11.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette11.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette11.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette11.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette11.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette11.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette11.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette11.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette11.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette11.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette11.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette11.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette11.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette11.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette11.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette11.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette11.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette11.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsInside5->setPalette(palette11);
        StatisticsInside5->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsInside5->setAutoFillBackground(true);
        StatisticsInside5->setFrameShape(QFrame::Box);
        StatisticsInside5->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(8, QFormLayout::FieldRole, StatisticsInside5);

        label_46 = new QLabel(scrollAreaWidgetContents);
        label_46->setObjectName(QStringLiteral("label_46"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_46);

        StatisticsInside10 = new QLabel(scrollAreaWidgetContents);
        StatisticsInside10->setObjectName(QStringLiteral("StatisticsInside10"));
        sizePolicy9.setHeightForWidth(StatisticsInside10->sizePolicy().hasHeightForWidth());
        StatisticsInside10->setSizePolicy(sizePolicy9);
        StatisticsInside10->setMinimumSize(QSize(100, 20));
        QPalette palette12;
        palette12.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette12.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette12.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette12.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette12.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette12.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette12.setBrush(QPalette::Active, QPalette::Text, brush);
        palette12.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette12.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette12.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette12.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette12.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette12.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette12.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette12.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette12.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette12.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette12.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette12.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette12.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette12.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette12.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette12.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette12.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette12.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette12.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette12.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette12.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette12.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette12.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette12.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsInside10->setPalette(palette12);
        StatisticsInside10->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsInside10->setAutoFillBackground(true);
        StatisticsInside10->setFrameShape(QFrame::Box);
        StatisticsInside10->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(9, QFormLayout::FieldRole, StatisticsInside10);

        label_47 = new QLabel(scrollAreaWidgetContents);
        label_47->setObjectName(QStringLiteral("label_47"));

        formLayout->setWidget(10, QFormLayout::LabelRole, label_47);

        StatisticsInside25 = new QLabel(scrollAreaWidgetContents);
        StatisticsInside25->setObjectName(QStringLiteral("StatisticsInside25"));
        sizePolicy9.setHeightForWidth(StatisticsInside25->sizePolicy().hasHeightForWidth());
        StatisticsInside25->setSizePolicy(sizePolicy9);
        StatisticsInside25->setMinimumSize(QSize(100, 20));
        QPalette palette13;
        palette13.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette13.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette13.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette13.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette13.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette13.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette13.setBrush(QPalette::Active, QPalette::Text, brush);
        palette13.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette13.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette13.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette13.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette13.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette13.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette13.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette13.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette13.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette13.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette13.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette13.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette13.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette13.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette13.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette13.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette13.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette13.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette13.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette13.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette13.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette13.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette13.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette13.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette13.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette13.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette13.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette13.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette13.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette13.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette13.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette13.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette13.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette13.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette13.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette13.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette13.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette13.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsInside25->setPalette(palette13);
        StatisticsInside25->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsInside25->setAutoFillBackground(true);
        StatisticsInside25->setFrameShape(QFrame::Box);
        StatisticsInside25->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(10, QFormLayout::FieldRole, StatisticsInside25);

        label_48 = new QLabel(scrollAreaWidgetContents);
        label_48->setObjectName(QStringLiteral("label_48"));

        formLayout->setWidget(11, QFormLayout::LabelRole, label_48);

        StatisticsInside50 = new QLabel(scrollAreaWidgetContents);
        StatisticsInside50->setObjectName(QStringLiteral("StatisticsInside50"));
        sizePolicy9.setHeightForWidth(StatisticsInside50->sizePolicy().hasHeightForWidth());
        StatisticsInside50->setSizePolicy(sizePolicy9);
        StatisticsInside50->setMinimumSize(QSize(100, 20));
        QPalette palette14;
        palette14.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette14.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette14.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette14.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette14.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette14.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette14.setBrush(QPalette::Active, QPalette::Text, brush);
        palette14.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette14.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette14.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette14.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette14.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette14.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette14.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette14.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette14.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette14.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette14.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette14.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette14.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette14.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette14.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette14.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette14.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette14.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette14.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette14.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette14.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette14.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette14.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette14.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette14.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette14.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette14.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette14.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette14.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette14.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette14.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette14.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette14.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette14.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette14.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette14.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette14.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette14.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        StatisticsInside50->setPalette(palette14);
        StatisticsInside50->setCursor(QCursor(Qt::IBeamCursor));
        StatisticsInside50->setAutoFillBackground(true);
        StatisticsInside50->setFrameShape(QFrame::Box);
        StatisticsInside50->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        formLayout->setWidget(11, QFormLayout::FieldRole, StatisticsInside50);

        scrollArea->setWidget(scrollAreaWidgetContents);
        label_46->raise();
        label_42->raise();
        label_47->raise();
        label_48->raise();
        StatisticsRows->raise();
        StatisticsHandstrokeError->raise();
        StatisticsBackstrokeError->raise();
        label_44->raise();
        label_45->raise();
        StatisticsInside10->raise();
        StatisticsInside25->raise();
        StatisticsInside50->raise();
        label_43->raise();
        StatisticsCombinedError->raise();
        StatisticsSD->raise();
        label_2->raise();
        StatisticsBackstrokeSD->raise();
        StatisticsHandstrokeSD->raise();
        label_3->raise();
        label_4->raise();
        StatisticsInside5->raise();
        label->raise();
        StatisticsLastTen->raise();
        label_5->raise();

        verticalLayout_10->addWidget(scrollArea);

        StatisticsAnalyze = new QPushButton(groupBox_6);
        StatisticsAnalyze->setObjectName(QStringLiteral("StatisticsAnalyze"));

        verticalLayout_10->addWidget(StatisticsAnalyze);


        horizontalLayout_11->addWidget(groupBox_6);

        StrikingGraphic = new StrikingWindow(groupBox);
        StrikingGraphic->setObjectName(QStringLiteral("StrikingGraphic"));
        StrikingGraphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        StrikingGraphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        StrikingGraphic->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        StrikingGraphic->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

        horizontalLayout_11->addWidget(StrikingGraphic);

        horizontalLayout_11->setStretch(1, 1);

        verticalLayout_12->addWidget(groupBox);

        verticalLayout_12->setStretch(2, 1);
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);
        SensorDock = new QDockWidget(MainWindow);
        SensorDock->setObjectName(QStringLiteral("SensorDock"));
        QSizePolicy sizePolicy10(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy10.setHorizontalStretch(0);
        sizePolicy10.setVerticalStretch(0);
        sizePolicy10.setHeightForWidth(SensorDock->sizePolicy().hasHeightForWidth());
        SensorDock->setSizePolicy(sizePolicy10);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
        verticalLayout_8 = new QVBoxLayout(dockWidgetContents_2);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        SensorTable = new SensorTableWidget(dockWidgetContents_2);
        if (SensorTable->columnCount() < 1)
            SensorTable->setColumnCount(1);
        if (SensorTable->rowCount() < 16)
            SensorTable->setRowCount(16);
        SensorTable->setObjectName(QStringLiteral("SensorTable"));
        sizePolicy3.setHeightForWidth(SensorTable->sizePolicy().hasHeightForWidth());
        SensorTable->setSizePolicy(sizePolicy3);
        SensorTable->setMinimumSize(QSize(50, 486));
        SensorTable->setAcceptDrops(true);
        SensorTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        SensorTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        SensorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        SensorTable->setDragEnabled(true);
        SensorTable->setDragDropMode(QAbstractItemView::DragDrop);
        SensorTable->setDefaultDropAction(Qt::MoveAction);
        SensorTable->setAlternatingRowColors(true);
        SensorTable->setSelectionMode(QAbstractItemView::SingleSelection);
        SensorTable->setShowGrid(true);
        SensorTable->setWordWrap(false);
        SensorTable->setCornerButtonEnabled(false);
        SensorTable->setRowCount(16);
        SensorTable->setColumnCount(1);
        SensorTable->horizontalHeader()->setVisible(false);
        SensorTable->horizontalHeader()->setDefaultSectionSize(70);
        SensorTable->horizontalHeader()->setStretchLastSection(true);

        horizontalLayout_12->addWidget(SensorTable);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalSpacer_11 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_11);

        AddSensorButton = new QToolButton(dockWidgetContents_2);
        AddSensorButton->setObjectName(QStringLiteral("AddSensorButton"));

        verticalLayout_6->addWidget(AddSensorButton);

        verticalSpacer_10 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_10);

        RemoveSensorButton = new QToolButton(dockWidgetContents_2);
        RemoveSensorButton->setObjectName(QStringLiteral("RemoveSensorButton"));

        verticalLayout_6->addWidget(RemoveSensorButton);

        verticalSpacer_12 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_12);


        horizontalLayout_12->addLayout(verticalLayout_6);

        SensorList = new SensorListWidget(dockWidgetContents_2);
        SensorList->setObjectName(QStringLiteral("SensorList"));
        QSizePolicy sizePolicy11(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy11.setHorizontalStretch(0);
        sizePolicy11.setVerticalStretch(0);
        sizePolicy11.setHeightForWidth(SensorList->sizePolicy().hasHeightForWidth());
        SensorList->setSizePolicy(sizePolicy11);
        SensorList->setAcceptDrops(true);
        SensorList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        SensorList->setDragEnabled(true);
        SensorList->setDragDropOverwriteMode(true);
        SensorList->setDragDropMode(QAbstractItemView::DragDrop);
        SensorList->setDefaultDropAction(Qt::MoveAction);
        SensorList->setSelectionRectVisible(true);

        horizontalLayout_12->addWidget(SensorList);


        verticalLayout_8->addLayout(horizontalLayout_12);

        SensorClear = new QPushButton(dockWidgetContents_2);
        SensorClear->setObjectName(QStringLiteral("SensorClear"));

        verticalLayout_8->addWidget(SensorClear);

        SensorDock->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), SensorDock);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QStringLiteral("dockWidget"));
        sizePolicy9.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
        dockWidget->setSizePolicy(sizePolicy9);
        dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidget->setAllowedAreas(Qt::TopDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        horizontalLayout_3 = new QHBoxLayout(dockWidgetContents);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_17 = new QLabel(dockWidgetContents);
        label_17->setObjectName(QStringLiteral("label_17"));
        sizePolicy1.setHeightForWidth(label_17->sizePolicy().hasHeightForWidth());
        label_17->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(label_17);

        PealSpeed = new QTimeEdit(dockWidgetContents);
        PealSpeed->setObjectName(QStringLiteral("PealSpeed"));
        sizePolicy1.setHeightForWidth(PealSpeed->sizePolicy().hasHeightForWidth());
        PealSpeed->setSizePolicy(sizePolicy1);
        PealSpeed->setWrapping(false);
        PealSpeed->setCurrentSection(QDateTimeEdit::HourSection);
        PealSpeed->setCalendarPopup(false);
        PealSpeed->setTimeSpec(Qt::LocalTime);

        horizontalLayout_3->addWidget(PealSpeed);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        label_18 = new QLabel(dockWidgetContents);
        label_18->setObjectName(QStringLiteral("label_18"));
        sizePolicy1.setHeightForWidth(label_18->sizePolicy().hasHeightForWidth());
        label_18->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(label_18);

        ChangesPerMinute = new QDoubleSpinBox(dockWidgetContents);
        ChangesPerMinute->setObjectName(QStringLiteral("ChangesPerMinute"));
        sizePolicy1.setHeightForWidth(ChangesPerMinute->sizePolicy().hasHeightForWidth());
        ChangesPerMinute->setSizePolicy(sizePolicy1);
        ChangesPerMinute->setMinimumSize(QSize(70, 0));
        ChangesPerMinute->setMinimum(4);
        ChangesPerMinute->setValue(28);

        horizontalLayout_3->addWidget(ChangesPerMinute);

        pushButton = new QPushButton(dockWidgetContents);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_3->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        label_6 = new QLabel(dockWidgetContents);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_3->addWidget(label_6);

        BellGap = new QLabel(dockWidgetContents);
        BellGap->setObjectName(QStringLiteral("BellGap"));
        QPalette palette15;
        palette15.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette15.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette15.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette15.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette15.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette15.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette15.setBrush(QPalette::Active, QPalette::Text, brush);
        palette15.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette15.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette15.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette15.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette15.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette15.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette15.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette15.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette15.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette15.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette15.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette15.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette15.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette15.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette15.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette15.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette15.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette15.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette15.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette15.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette15.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette15.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette15.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette15.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette15.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette15.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette15.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette15.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette15.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette15.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette15.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette15.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette15.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette15.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette15.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette15.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette15.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette15.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        BellGap->setPalette(palette15);
        BellGap->setAutoFillBackground(true);
        BellGap->setFrameShape(QFrame::Box);
        BellGap->setTextFormat(Qt::PlainText);
        BellGap->setTextInteractionFlags(Qt::TextSelectableByMouse);

        horizontalLayout_3->addWidget(BellGap);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        AudibleCalls = new QCheckBox(dockWidgetContents);
        AudibleCalls->setObjectName(QStringLiteral("AudibleCalls"));
        AudibleCalls->setEnabled(true);
        AudibleCalls->setMinimumSize(QSize(120, 0));
        AudibleCalls->setCheckable(true);
        AudibleCalls->setChecked(true);

        horizontalLayout_3->addWidget(AudibleCalls);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(4), dockWidget);
        toolBar_2 = new QToolBar(MainWindow);
        toolBar_2->setObjectName(QStringLiteral("toolBar_2"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar_2);
        FeedbackDock = new QDockWidget(MainWindow);
        FeedbackDock->setObjectName(QStringLiteral("FeedbackDock"));
        dockWidgetContents_5 = new QWidget();
        dockWidgetContents_5->setObjectName(QStringLiteral("dockWidgetContents_5"));
        verticalLayout_5 = new QVBoxLayout(dockWidgetContents_5);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        groupBox_4 = new QGroupBox(dockWidgetContents_5);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        horizontalLayout_8 = new QHBoxLayout(groupBox_4);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_29 = new QLabel(groupBox_4);
        label_29->setObjectName(QStringLiteral("label_29"));

        horizontalLayout_8->addWidget(label_29);

        RingingSpeedFromSensors = new QSlider(groupBox_4);
        RingingSpeedFromSensors->setObjectName(QStringLiteral("RingingSpeedFromSensors"));
        RingingSpeedFromSensors->setMaximum(100);
        RingingSpeedFromSensors->setOrientation(Qt::Horizontal);
        RingingSpeedFromSensors->setTickPosition(QSlider::TicksBelow);
        RingingSpeedFromSensors->setTickInterval(25);

        horizontalLayout_8->addWidget(RingingSpeedFromSensors);

        spinBox = new QSpinBox(groupBox_4);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMaximum(100);

        horizontalLayout_8->addWidget(spinBox);


        verticalLayout_5->addWidget(groupBox_4);

        groupBox_5 = new QGroupBox(dockWidgetContents_5);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        horizontalLayout_5 = new QHBoxLayout(groupBox_5);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_26 = new QLabel(groupBox_5);
        label_26->setObjectName(QStringLiteral("label_26"));

        horizontalLayout_5->addWidget(label_26);

        SensorErrorMultipler = new QSlider(groupBox_5);
        SensorErrorMultipler->setObjectName(QStringLiteral("SensorErrorMultipler"));
        SensorErrorMultipler->setMaximum(200);
        SensorErrorMultipler->setSingleStep(1);
        SensorErrorMultipler->setValue(0);
        SensorErrorMultipler->setTracking(true);
        SensorErrorMultipler->setOrientation(Qt::Horizontal);
        SensorErrorMultipler->setInvertedAppearance(false);
        SensorErrorMultipler->setInvertedControls(false);
        SensorErrorMultipler->setTickPosition(QSlider::TicksBelow);
        SensorErrorMultipler->setTickInterval(50);

        horizontalLayout_5->addWidget(SensorErrorMultipler);

        SensorErrorMultiplier = new QSpinBox(groupBox_5);
        SensorErrorMultiplier->setObjectName(QStringLiteral("SensorErrorMultiplier"));
        SensorErrorMultiplier->setMaximum(200);
        SensorErrorMultiplier->setValue(0);

        horizontalLayout_5->addWidget(SensorErrorMultiplier);


        verticalLayout_5->addWidget(groupBox_5);

        FeedbackDock->setWidget(dockWidgetContents_5);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), FeedbackDock);
#ifndef QT_NO_SHORTCUT
        label_19->setBuddy(NumberOfBells);
        label_20->setBuddy(CCStyle);
        label_17->setBuddy(PealSpeed);
        label_18->setBuddy(ChangesPerMinute);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(PealSpeed, ChangesPerMinute);
        QWidget::setTabOrder(ChangesPerMinute, AudibleCalls);
        QWidget::setTabOrder(AudibleCalls, NumberOfBells);
        QWidget::setTabOrder(NumberOfBells, Call56);
        QWidget::setTabOrder(Call56, Call12);
        QWidget::setTabOrder(Call12, Call23);
        QWidget::setTabOrder(Call23, CallET);
        QWidget::setTabOrder(CallET, Call0E);
        QWidget::setTabOrder(Call0E, Call89);
        QWidget::setTabOrder(Call89, Call78);
        QWidget::setTabOrder(Call78, Call45);
        QWidget::setTabOrder(Call45, CallAB);
        QWidget::setTabOrder(CallAB, CCStyle);
        QWidget::setTabOrder(CCStyle, Call34);
        QWidget::setTabOrder(Call34, Call67);
        QWidget::setTabOrder(Call67, Call90);
        QWidget::setTabOrder(Call90, CallTA);
        QWidget::setTabOrder(CallTA, CallBC);
        QWidget::setTabOrder(CallBC, SensorErrorMultipler);
        QWidget::setTabOrder(SensorErrorMultipler, SensorErrorMultiplier);
        QWidget::setTabOrder(SensorErrorMultiplier, SensorClear);
        QWidget::setTabOrder(SensorClear, CallCD);
        QWidget::setTabOrder(CallCD, MethodTenorBehind);
        QWidget::setTabOrder(MethodTenorBehind, MethodKeepGoing);

        toolBar_2->addAction(actionFile);
        toolBar_2->addAction(actionListen_for_connections);

        retranslateUi(MainWindow);
        QObject::connect(SensorErrorMultipler, SIGNAL(sliderMoved(int)), SensorErrorMultiplier, SLOT(setValue(int)));
        QObject::connect(SensorErrorMultiplier, SIGNAL(valueChanged(int)), SensorErrorMultipler, SLOT(setValue(int)));
        QObject::connect(RingingSpeedFromSensors, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
        QObject::connect(spinBox, SIGNAL(valueChanged(int)), RingingSpeedFromSensors, SLOT(setValue(int)));

        Call12->setDefault(false);
        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Bell Ringing Simulator", Q_NULLPTR));
        actionFile->setText(QApplication::translate("MainWindow", "Quit", Q_NULLPTR));
        actionListen_for_connections->setText(QApplication::translate("MainWindow", "Listen for connections", Q_NULLPTR));
        actionSetup_local_sensors->setText(QApplication::translate("MainWindow", "Configure local sensors...", Q_NULLPTR));
        actionAbout_Bell_Ringing_Simulator->setText(QApplication::translate("MainWindow", "About Bell Ringing Simulator", Q_NULLPTR));
        actionPreferences->setText(QApplication::translate("MainWindow", "Preferences...", Q_NULLPTR));
        actionSensors->setText(QApplication::translate("MainWindow", "Sensors", Q_NULLPTR));
        RingingStart->setText(QApplication::translate("MainWindow", "Start", Q_NULLPTR));
        RingingAuto->setText(QApplication::translate("MainWindow", "Auto", Q_NULLPTR));
        RingingRounds->setText(QApplication::translate("MainWindow", "Rounds", Q_NULLPTR));
        RingingStand->setText(QApplication::translate("MainWindow", "Stand", Q_NULLPTR));
        RingingStop->setText(QApplication::translate("MainWindow", "Stop", Q_NULLPTR));
        label_19->setText(QApplication::translate("MainWindow", "Number of bells:", Q_NULLPTR));
        label_20->setText(QApplication::translate("MainWindow", "Call style:", Q_NULLPTR));
        CCStyle->clear();
        CCStyle->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Up", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Down", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Place bells", Q_NULLPTR)
        );
        groupBox_3->setTitle(QString());
        CC1->setText(QApplication::translate("MainWindow", "1", Q_NULLPTR));
        CC2->setText(QApplication::translate("MainWindow", "2", Q_NULLPTR));
        CC3->setText(QApplication::translate("MainWindow", "3", Q_NULLPTR));
        CC4->setText(QApplication::translate("MainWindow", "4", Q_NULLPTR));
        CC5->setText(QApplication::translate("MainWindow", "5", Q_NULLPTR));
        CC6->setText(QApplication::translate("MainWindow", "6", Q_NULLPTR));
        CC7->setText(QApplication::translate("MainWindow", "7", Q_NULLPTR));
        CC8->setText(QApplication::translate("MainWindow", "8", Q_NULLPTR));
        CC9->setText(QApplication::translate("MainWindow", "9", Q_NULLPTR));
        CC10->setText(QApplication::translate("MainWindow", "0", Q_NULLPTR));
        CC11->setText(QApplication::translate("MainWindow", "E", Q_NULLPTR));
        CC12->setText(QApplication::translate("MainWindow", "T", Q_NULLPTR));
        CC13->setText(QApplication::translate("MainWindow", "A", Q_NULLPTR));
        CC14->setText(QApplication::translate("MainWindow", "B", Q_NULLPTR));
        CC15->setText(QApplication::translate("MainWindow", "C", Q_NULLPTR));
        CC16->setText(QApplication::translate("MainWindow", "D", Q_NULLPTR));
        groupBox_7->setTitle(QString());
        SelectCallChange->setText(QApplication::translate("MainWindow", "Call change", Q_NULLPTR));
        SelectDodge->setText(QApplication::translate("MainWindow", "Dodging", Q_NULLPTR));
        SelectRightPlaces->setText(QApplication::translate("MainWindow", "Right places", Q_NULLPTR));
        SelectWrongPlaces->setText(QApplication::translate("MainWindow", "Wrong places", Q_NULLPTR));
        Call12->setText(QApplication::translate("MainWindow", "1-2", Q_NULLPTR));
        Call23->setText(QApplication::translate("MainWindow", "2-3", Q_NULLPTR));
        Call34->setText(QApplication::translate("MainWindow", "3-4", Q_NULLPTR));
        Call45->setText(QApplication::translate("MainWindow", "4-5", Q_NULLPTR));
        Call56->setText(QApplication::translate("MainWindow", "5-6", Q_NULLPTR));
        Call67->setText(QApplication::translate("MainWindow", "6-7", Q_NULLPTR));
        Call78->setText(QApplication::translate("MainWindow", "7-8", Q_NULLPTR));
        Call89->setText(QApplication::translate("MainWindow", "8-9", Q_NULLPTR));
        Call90->setText(QApplication::translate("MainWindow", "9-0", Q_NULLPTR));
        Call0E->setText(QApplication::translate("MainWindow", "0-E", Q_NULLPTR));
        CallET->setText(QApplication::translate("MainWindow", "E-T", Q_NULLPTR));
        CallTA->setText(QApplication::translate("MainWindow", "T-A", Q_NULLPTR));
        CallAB->setText(QApplication::translate("MainWindow", "A-B", Q_NULLPTR));
        CallBC->setText(QApplication::translate("MainWindow", "B-C", Q_NULLPTR));
        CallCD->setText(QApplication::translate("MainWindow", "C-D", Q_NULLPTR));
        RingMethod->setText(QApplication::translate("MainWindow", "Method", Q_NULLPTR));
        RingTouch->setText(QApplication::translate("MainWindow", "Touch", Q_NULLPTR));
        MethodGo->setText(QApplication::translate("MainWindow", "Go", Q_NULLPTR));
        MethodGoAfterBox->setTitle(QString());
        MethodGoDelay->setText(QApplication::translate("MainWindow", "Go with delay", Q_NULLPTR));
        label_50->setText(QApplication::translate("MainWindow", "whole pulls", Q_NULLPTR));
        MethodTenorBehind->setText(QApplication::translate("MainWindow", "Tenor behind for odd numbers", Q_NULLPTR));
        label_22->setText(QApplication::translate("MainWindow", "Current method:", Q_NULLPTR));
        CurrentMethod->setText(QString());
        label_24->setText(QApplication::translate("MainWindow", "Next method:", Q_NULLPTR));
        NextMethod->setText(QString());
        SelectMethod->setText(QApplication::translate("MainWindow", "Select next method...", Q_NULLPTR));
        MethodCallRepeat->setText(QApplication::translate("MainWindow", "Repeat calls", Q_NULLPTR));
        MethodRandomCalls->setText(QApplication::translate("MainWindow", "Random calls", Q_NULLPTR));
        groupBox_9->setTitle(QString());
        MethodCall0->setText(QApplication::translate("MainWindow", "Bob", Q_NULLPTR));
        MethodCall1->setText(QApplication::translate("MainWindow", "Single", Q_NULLPTR));
        MethodCall2->setText(QApplication::translate("MainWindow", "Extreme", Q_NULLPTR));
        MethodCall3->setText(QApplication::translate("MainWindow", "Call 4", Q_NULLPTR));
        MethodSplice->setText(QApplication::translate("MainWindow", "Splice", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Keep going for", Q_NULLPTR));
        label_23->setText(QApplication::translate("MainWindow", "course(s)", Q_NULLPTR));
        MethodThatsAll->setText(QApplication::translate("MainWindow", "That's All", Q_NULLPTR));
        MethodStandAfterBox->setTitle(QApplication::translate("MainWindow", "Stand after", Q_NULLPTR));
        label_52->setText(QApplication::translate("MainWindow", "whole pulls", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MainWindow", "Striking", Q_NULLPTR));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "Statstics", Q_NULLPTR));
        label_49->setText(QApplication::translate("MainWindow", "Sensor:", Q_NULLPTR));
        label_42->setText(QApplication::translate("MainWindow", "Rows:", Q_NULLPTR));
        StatisticsRows->setText(QString());
        label_43->setText(QApplication::translate("MainWindow", "Average error:", Q_NULLPTR));
        StatisticsCombinedError->setText(QString());
        label_5->setText(QApplication::translate("MainWindow", "Last 10 blows:", Q_NULLPTR));
        StatisticsLastTen->setText(QString());
        label_44->setText(QApplication::translate("MainWindow", "Handstroke error:", Q_NULLPTR));
        StatisticsHandstrokeError->setText(QString());
        label_45->setText(QApplication::translate("MainWindow", "Backstroke error:", Q_NULLPTR));
        StatisticsBackstrokeError->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "Standard dev:", Q_NULLPTR));
        StatisticsSD->setText(QString());
        label_3->setText(QApplication::translate("MainWindow", "Handstroke SD:", Q_NULLPTR));
        StatisticsHandstrokeSD->setText(QString());
        label_4->setText(QApplication::translate("MainWindow", "Backstroke SD:", Q_NULLPTR));
        StatisticsBackstrokeSD->setText(QString());
        label->setText(QApplication::translate("MainWindow", "Inside 5%:", Q_NULLPTR));
        StatisticsInside5->setText(QString());
        label_46->setText(QApplication::translate("MainWindow", "Inside 10%:", Q_NULLPTR));
        StatisticsInside10->setText(QString());
        label_47->setText(QApplication::translate("MainWindow", "Inside 25%:", Q_NULLPTR));
        StatisticsInside25->setText(QString());
        label_48->setText(QApplication::translate("MainWindow", "Inside 50%:", Q_NULLPTR));
        StatisticsInside50->setText(QString());
        StatisticsAnalyze->setText(QApplication::translate("MainWindow", "Analyze...", Q_NULLPTR));
        SensorDock->setWindowTitle(QApplication::translate("MainWindow", "Sensors", Q_NULLPTR));
        AddSensorButton->setText(QString());
        RemoveSensorButton->setText(QString());
        SensorClear->setText(QApplication::translate("MainWindow", "Clear", Q_NULLPTR));
        label_17->setText(QApplication::translate("MainWindow", "Peal Speed:", Q_NULLPTR));
        PealSpeed->setDisplayFormat(QApplication::translate("MainWindow", "HH:mm:ss", Q_NULLPTR));
        label_18->setText(QApplication::translate("MainWindow", "Changes/minute:", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MainWindow", "Revert to last", Q_NULLPTR));
        label_6->setText(QApplication::translate("MainWindow", "Bell gap:", Q_NULLPTR));
        BellGap->setText(QApplication::translate("MainWindow", "100", Q_NULLPTR));
        AudibleCalls->setText(QApplication::translate("MainWindow", "Audible calls", Q_NULLPTR));
        toolBar_2->setWindowTitle(QApplication::translate("MainWindow", "toolBar_2", Q_NULLPTR));
        FeedbackDock->setWindowTitle(QApplication::translate("MainWindow", "Sensor feedback", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Match speed of ringing to sensors", Q_NULLPTR));
        label_29->setText(QApplication::translate("MainWindow", "Rate", Q_NULLPTR));
        spinBox->setSuffix(QApplication::translate("MainWindow", "%", Q_NULLPTR));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Match striking errors to sensors", Q_NULLPTR));
        label_26->setText(QApplication::translate("MainWindow", "Rate", Q_NULLPTR));
        SensorErrorMultiplier->setSuffix(QApplication::translate("MainWindow", "%", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_2D_TOWERSIM_2D_MAINWINDOW_H
