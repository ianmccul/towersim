/********************************************************************************
** Form generated from reading UI file 'ui-select-method.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_2D_SELECT_2D_METHOD_H
#define UI_UI_2D_SELECT_2D_METHOD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#include "lineedit.h"

QT_BEGIN_NAMESPACE

class Ui_SelectMethodDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QCheckBox *FilterPrinciples;
    QCheckBox *FilterPlain;
    QCheckBox *FilterTrebleBob;
    QCheckBox *FilterDelight;
    QCheckBox *FilterSurprise;
    QCheckBox *FilterOther;
    QPushButton *FilterSelectAll;
    QPushButton *FilterSelectNone;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *Select03;
    QPushButton *Select04;
    QPushButton *Select05;
    QPushButton *Select06;
    QPushButton *Select07;
    QPushButton *Select08;
    QPushButton *Select09;
    QPushButton *Select10;
    QPushButton *Select11;
    QPushButton *Select12;
    QPushButton *Select13;
    QPushButton *Select14;
    QPushButton *Select15;
    QPushButton *Select16;
    QLabel *label;
    LineEdit *MethodSearch;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *EditMethod;
    QPushButton *NewMethod;
    QPushButton *LoadCCCollection;
    QDialogButtonBox *buttonBox;
    QStackedWidget *MethodDisplay;

    void setupUi(QDialog *SelectMethodDialog)
    {
        if (SelectMethodDialog->objectName().isEmpty())
            SelectMethodDialog->setObjectName(QString::fromUtf8("SelectMethodDialog"));
        SelectMethodDialog->resize(790, 767);
        verticalLayout_2 = new QVBoxLayout(SelectMethodDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        FilterPrinciples = new QCheckBox(SelectMethodDialog);
        FilterPrinciples->setObjectName(QString::fromUtf8("FilterPrinciples"));
        FilterPrinciples->setChecked(true);

        horizontalLayout->addWidget(FilterPrinciples);

        FilterPlain = new QCheckBox(SelectMethodDialog);
        FilterPlain->setObjectName(QString::fromUtf8("FilterPlain"));
        FilterPlain->setChecked(true);

        horizontalLayout->addWidget(FilterPlain);

        FilterTrebleBob = new QCheckBox(SelectMethodDialog);
        FilterTrebleBob->setObjectName(QString::fromUtf8("FilterTrebleBob"));
        FilterTrebleBob->setChecked(true);

        horizontalLayout->addWidget(FilterTrebleBob);

        FilterDelight = new QCheckBox(SelectMethodDialog);
        FilterDelight->setObjectName(QString::fromUtf8("FilterDelight"));
        FilterDelight->setChecked(true);

        horizontalLayout->addWidget(FilterDelight);

        FilterSurprise = new QCheckBox(SelectMethodDialog);
        FilterSurprise->setObjectName(QString::fromUtf8("FilterSurprise"));
        FilterSurprise->setChecked(true);

        horizontalLayout->addWidget(FilterSurprise);

        FilterOther = new QCheckBox(SelectMethodDialog);
        FilterOther->setObjectName(QString::fromUtf8("FilterOther"));
        FilterOther->setChecked(true);

        horizontalLayout->addWidget(FilterOther);

        FilterSelectAll = new QPushButton(SelectMethodDialog);
        FilterSelectAll->setObjectName(QString::fromUtf8("FilterSelectAll"));

        horizontalLayout->addWidget(FilterSelectAll);

        FilterSelectNone = new QPushButton(SelectMethodDialog);
        FilterSelectNone->setObjectName(QString::fromUtf8("FilterSelectNone"));

        horizontalLayout->addWidget(FilterSelectNone);


        verticalLayout_2->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        Select03 = new QPushButton(SelectMethodDialog);
        Select03->setObjectName(QString::fromUtf8("Select03"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Select03->sizePolicy().hasHeightForWidth());
        Select03->setSizePolicy(sizePolicy);
        Select03->setCheckable(true);
        Select03->setAutoExclusive(true);
        Select03->setAutoDefault(false);

        verticalLayout->addWidget(Select03);

        Select04 = new QPushButton(SelectMethodDialog);
        Select04->setObjectName(QString::fromUtf8("Select04"));
        sizePolicy.setHeightForWidth(Select04->sizePolicy().hasHeightForWidth());
        Select04->setSizePolicy(sizePolicy);
        Select04->setCheckable(true);
        Select04->setAutoExclusive(true);
        Select04->setAutoDefault(false);

        verticalLayout->addWidget(Select04);

        Select05 = new QPushButton(SelectMethodDialog);
        Select05->setObjectName(QString::fromUtf8("Select05"));
        sizePolicy.setHeightForWidth(Select05->sizePolicy().hasHeightForWidth());
        Select05->setSizePolicy(sizePolicy);
        Select05->setCheckable(true);
        Select05->setAutoExclusive(true);
        Select05->setAutoDefault(false);

        verticalLayout->addWidget(Select05);

        Select06 = new QPushButton(SelectMethodDialog);
        Select06->setObjectName(QString::fromUtf8("Select06"));
        sizePolicy.setHeightForWidth(Select06->sizePolicy().hasHeightForWidth());
        Select06->setSizePolicy(sizePolicy);
        Select06->setCheckable(true);
        Select06->setAutoExclusive(true);
        Select06->setAutoDefault(false);

        verticalLayout->addWidget(Select06);

        Select07 = new QPushButton(SelectMethodDialog);
        Select07->setObjectName(QString::fromUtf8("Select07"));
        sizePolicy.setHeightForWidth(Select07->sizePolicy().hasHeightForWidth());
        Select07->setSizePolicy(sizePolicy);
        Select07->setCheckable(true);
        Select07->setAutoExclusive(true);
        Select07->setAutoDefault(false);

        verticalLayout->addWidget(Select07);

        Select08 = new QPushButton(SelectMethodDialog);
        Select08->setObjectName(QString::fromUtf8("Select08"));
        sizePolicy.setHeightForWidth(Select08->sizePolicy().hasHeightForWidth());
        Select08->setSizePolicy(sizePolicy);
        Select08->setCheckable(true);
        Select08->setChecked(true);
        Select08->setAutoExclusive(true);
        Select08->setAutoDefault(false);

        verticalLayout->addWidget(Select08);

        Select09 = new QPushButton(SelectMethodDialog);
        Select09->setObjectName(QString::fromUtf8("Select09"));
        sizePolicy.setHeightForWidth(Select09->sizePolicy().hasHeightForWidth());
        Select09->setSizePolicy(sizePolicy);
        Select09->setCheckable(true);
        Select09->setAutoExclusive(true);
        Select09->setAutoDefault(false);

        verticalLayout->addWidget(Select09);

        Select10 = new QPushButton(SelectMethodDialog);
        Select10->setObjectName(QString::fromUtf8("Select10"));
        sizePolicy.setHeightForWidth(Select10->sizePolicy().hasHeightForWidth());
        Select10->setSizePolicy(sizePolicy);
        Select10->setCheckable(true);
        Select10->setAutoExclusive(true);
        Select10->setAutoDefault(false);

        verticalLayout->addWidget(Select10);

        Select11 = new QPushButton(SelectMethodDialog);
        Select11->setObjectName(QString::fromUtf8("Select11"));
        sizePolicy.setHeightForWidth(Select11->sizePolicy().hasHeightForWidth());
        Select11->setSizePolicy(sizePolicy);
        Select11->setCheckable(true);
        Select11->setAutoExclusive(true);
        Select11->setAutoDefault(false);

        verticalLayout->addWidget(Select11);

        Select12 = new QPushButton(SelectMethodDialog);
        Select12->setObjectName(QString::fromUtf8("Select12"));
        sizePolicy.setHeightForWidth(Select12->sizePolicy().hasHeightForWidth());
        Select12->setSizePolicy(sizePolicy);
        Select12->setCheckable(true);
        Select12->setAutoExclusive(true);
        Select12->setAutoDefault(false);

        verticalLayout->addWidget(Select12);

        Select13 = new QPushButton(SelectMethodDialog);
        Select13->setObjectName(QString::fromUtf8("Select13"));
        sizePolicy.setHeightForWidth(Select13->sizePolicy().hasHeightForWidth());
        Select13->setSizePolicy(sizePolicy);
        Select13->setCheckable(true);
        Select13->setAutoExclusive(true);
        Select13->setAutoDefault(false);

        verticalLayout->addWidget(Select13);

        Select14 = new QPushButton(SelectMethodDialog);
        Select14->setObjectName(QString::fromUtf8("Select14"));
        sizePolicy.setHeightForWidth(Select14->sizePolicy().hasHeightForWidth());
        Select14->setSizePolicy(sizePolicy);
        Select14->setCheckable(true);
        Select14->setAutoExclusive(true);
        Select14->setAutoDefault(false);

        verticalLayout->addWidget(Select14);

        Select15 = new QPushButton(SelectMethodDialog);
        Select15->setObjectName(QString::fromUtf8("Select15"));
        sizePolicy.setHeightForWidth(Select15->sizePolicy().hasHeightForWidth());
        Select15->setSizePolicy(sizePolicy);
        Select15->setCheckable(true);
        Select15->setAutoExclusive(true);
        Select15->setAutoDefault(false);

        verticalLayout->addWidget(Select15);

        Select16 = new QPushButton(SelectMethodDialog);
        Select16->setObjectName(QString::fromUtf8("Select16"));
        sizePolicy.setHeightForWidth(Select16->sizePolicy().hasHeightForWidth());
        Select16->setSizePolicy(sizePolicy);
        Select16->setCheckable(true);
        Select16->setAutoExclusive(true);
        Select16->setAutoDefault(false);

        verticalLayout->addWidget(Select16);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        label = new QLabel(SelectMethodDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        MethodSearch = new LineEdit(SelectMethodDialog);
        MethodSearch->setObjectName(QString::fromUtf8("MethodSearch"));

        gridLayout->addWidget(MethodSearch, 1, 2, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        EditMethod = new QPushButton(SelectMethodDialog);
        EditMethod->setObjectName(QString::fromUtf8("EditMethod"));
        EditMethod->setEnabled(false);

        horizontalLayout_2->addWidget(EditMethod);

        NewMethod = new QPushButton(SelectMethodDialog);
        NewMethod->setObjectName(QString::fromUtf8("NewMethod"));
        NewMethod->setEnabled(false);

        horizontalLayout_2->addWidget(NewMethod);

        LoadCCCollection = new QPushButton(SelectMethodDialog);
        LoadCCCollection->setObjectName(QString::fromUtf8("LoadCCCollection"));

        horizontalLayout_2->addWidget(LoadCCCollection);

        buttonBox = new QDialogButtonBox(SelectMethodDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_2->addWidget(buttonBox);


        gridLayout->addLayout(horizontalLayout_2, 2, 2, 1, 1);

        MethodDisplay = new QStackedWidget(SelectMethodDialog);
        MethodDisplay->setObjectName(QString::fromUtf8("MethodDisplay"));

        gridLayout->addWidget(MethodDisplay, 0, 2, 1, 1);


        verticalLayout_2->addLayout(gridLayout);


        retranslateUi(SelectMethodDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SelectMethodDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SelectMethodDialog, SLOT(reject()));

        MethodDisplay->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(SelectMethodDialog);
    } // setupUi

    void retranslateUi(QDialog *SelectMethodDialog)
    {
        SelectMethodDialog->setWindowTitle(QApplication::translate("SelectMethodDialog", "Select Method", 0, QApplication::UnicodeUTF8));
        FilterPrinciples->setText(QApplication::translate("SelectMethodDialog", "Principles", 0, QApplication::UnicodeUTF8));
        FilterPlain->setText(QApplication::translate("SelectMethodDialog", "Plain", 0, QApplication::UnicodeUTF8));
        FilterTrebleBob->setText(QApplication::translate("SelectMethodDialog", "Treble Bob", 0, QApplication::UnicodeUTF8));
        FilterDelight->setText(QApplication::translate("SelectMethodDialog", "Delight", 0, QApplication::UnicodeUTF8));
        FilterSurprise->setText(QApplication::translate("SelectMethodDialog", "Surprise", 0, QApplication::UnicodeUTF8));
        FilterOther->setText(QApplication::translate("SelectMethodDialog", "Other", 0, QApplication::UnicodeUTF8));
        FilterSelectAll->setText(QApplication::translate("SelectMethodDialog", "All", 0, QApplication::UnicodeUTF8));
        FilterSelectNone->setText(QApplication::translate("SelectMethodDialog", "None", 0, QApplication::UnicodeUTF8));
        Select03->setText(QApplication::translate("SelectMethodDialog", "Singles", 0, QApplication::UnicodeUTF8));
        Select04->setText(QApplication::translate("SelectMethodDialog", "Minimus", 0, QApplication::UnicodeUTF8));
        Select05->setText(QApplication::translate("SelectMethodDialog", "Doubles", 0, QApplication::UnicodeUTF8));
        Select06->setText(QApplication::translate("SelectMethodDialog", "Minor", 0, QApplication::UnicodeUTF8));
        Select07->setText(QApplication::translate("SelectMethodDialog", "Triples", 0, QApplication::UnicodeUTF8));
        Select08->setText(QApplication::translate("SelectMethodDialog", "Major", 0, QApplication::UnicodeUTF8));
        Select09->setText(QApplication::translate("SelectMethodDialog", "Caters", 0, QApplication::UnicodeUTF8));
        Select10->setText(QApplication::translate("SelectMethodDialog", "Royal", 0, QApplication::UnicodeUTF8));
        Select11->setText(QApplication::translate("SelectMethodDialog", "Cinques", 0, QApplication::UnicodeUTF8));
        Select12->setText(QApplication::translate("SelectMethodDialog", "Maximus", 0, QApplication::UnicodeUTF8));
        Select13->setText(QApplication::translate("SelectMethodDialog", "Sextuples", 0, QApplication::UnicodeUTF8));
        Select14->setText(QApplication::translate("SelectMethodDialog", "Fourteen", 0, QApplication::UnicodeUTF8));
        Select15->setText(QApplication::translate("SelectMethodDialog", "Septuples", 0, QApplication::UnicodeUTF8));
        Select16->setText(QApplication::translate("SelectMethodDialog", "Sixteen", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SelectMethodDialog", "Search:", 0, QApplication::UnicodeUTF8));
        MethodSearch->setPlaceholderText(QApplication::translate("SelectMethodDialog", "Search", 0, QApplication::UnicodeUTF8));
        EditMethod->setText(QApplication::translate("SelectMethodDialog", "Edit method...", 0, QApplication::UnicodeUTF8));
        NewMethod->setText(QApplication::translate("SelectMethodDialog", "New method...", 0, QApplication::UnicodeUTF8));
        LoadCCCollection->setText(QApplication::translate("SelectMethodDialog", "Load CC Collection", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SelectMethodDialog: public Ui_SelectMethodDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_2D_SELECT_2D_METHOD_H
