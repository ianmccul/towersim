/********************************************************************************
** Form generated from reading UI file 'ui-select-method.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_2D_SELECT_2D_METHOD_H
#define UI_UI_2D_SELECT_2D_METHOD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
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
            SelectMethodDialog->setObjectName(QStringLiteral("SelectMethodDialog"));
        SelectMethodDialog->resize(790, 767);
        verticalLayout_2 = new QVBoxLayout(SelectMethodDialog);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        FilterPrinciples = new QCheckBox(SelectMethodDialog);
        FilterPrinciples->setObjectName(QStringLiteral("FilterPrinciples"));
        FilterPrinciples->setChecked(true);

        horizontalLayout->addWidget(FilterPrinciples);

        FilterPlain = new QCheckBox(SelectMethodDialog);
        FilterPlain->setObjectName(QStringLiteral("FilterPlain"));
        FilterPlain->setChecked(true);

        horizontalLayout->addWidget(FilterPlain);

        FilterTrebleBob = new QCheckBox(SelectMethodDialog);
        FilterTrebleBob->setObjectName(QStringLiteral("FilterTrebleBob"));
        FilterTrebleBob->setChecked(true);

        horizontalLayout->addWidget(FilterTrebleBob);

        FilterDelight = new QCheckBox(SelectMethodDialog);
        FilterDelight->setObjectName(QStringLiteral("FilterDelight"));
        FilterDelight->setChecked(true);

        horizontalLayout->addWidget(FilterDelight);

        FilterSurprise = new QCheckBox(SelectMethodDialog);
        FilterSurprise->setObjectName(QStringLiteral("FilterSurprise"));
        FilterSurprise->setChecked(true);

        horizontalLayout->addWidget(FilterSurprise);

        FilterOther = new QCheckBox(SelectMethodDialog);
        FilterOther->setObjectName(QStringLiteral("FilterOther"));
        FilterOther->setChecked(true);

        horizontalLayout->addWidget(FilterOther);

        FilterSelectAll = new QPushButton(SelectMethodDialog);
        FilterSelectAll->setObjectName(QStringLiteral("FilterSelectAll"));

        horizontalLayout->addWidget(FilterSelectAll);

        FilterSelectNone = new QPushButton(SelectMethodDialog);
        FilterSelectNone->setObjectName(QStringLiteral("FilterSelectNone"));

        horizontalLayout->addWidget(FilterSelectNone);


        verticalLayout_2->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        Select03 = new QPushButton(SelectMethodDialog);
        Select03->setObjectName(QStringLiteral("Select03"));
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
        Select04->setObjectName(QStringLiteral("Select04"));
        sizePolicy.setHeightForWidth(Select04->sizePolicy().hasHeightForWidth());
        Select04->setSizePolicy(sizePolicy);
        Select04->setCheckable(true);
        Select04->setAutoExclusive(true);
        Select04->setAutoDefault(false);

        verticalLayout->addWidget(Select04);

        Select05 = new QPushButton(SelectMethodDialog);
        Select05->setObjectName(QStringLiteral("Select05"));
        sizePolicy.setHeightForWidth(Select05->sizePolicy().hasHeightForWidth());
        Select05->setSizePolicy(sizePolicy);
        Select05->setCheckable(true);
        Select05->setAutoExclusive(true);
        Select05->setAutoDefault(false);

        verticalLayout->addWidget(Select05);

        Select06 = new QPushButton(SelectMethodDialog);
        Select06->setObjectName(QStringLiteral("Select06"));
        sizePolicy.setHeightForWidth(Select06->sizePolicy().hasHeightForWidth());
        Select06->setSizePolicy(sizePolicy);
        Select06->setCheckable(true);
        Select06->setAutoExclusive(true);
        Select06->setAutoDefault(false);

        verticalLayout->addWidget(Select06);

        Select07 = new QPushButton(SelectMethodDialog);
        Select07->setObjectName(QStringLiteral("Select07"));
        sizePolicy.setHeightForWidth(Select07->sizePolicy().hasHeightForWidth());
        Select07->setSizePolicy(sizePolicy);
        Select07->setCheckable(true);
        Select07->setAutoExclusive(true);
        Select07->setAutoDefault(false);

        verticalLayout->addWidget(Select07);

        Select08 = new QPushButton(SelectMethodDialog);
        Select08->setObjectName(QStringLiteral("Select08"));
        sizePolicy.setHeightForWidth(Select08->sizePolicy().hasHeightForWidth());
        Select08->setSizePolicy(sizePolicy);
        Select08->setCheckable(true);
        Select08->setChecked(true);
        Select08->setAutoExclusive(true);
        Select08->setAutoDefault(false);

        verticalLayout->addWidget(Select08);

        Select09 = new QPushButton(SelectMethodDialog);
        Select09->setObjectName(QStringLiteral("Select09"));
        sizePolicy.setHeightForWidth(Select09->sizePolicy().hasHeightForWidth());
        Select09->setSizePolicy(sizePolicy);
        Select09->setCheckable(true);
        Select09->setAutoExclusive(true);
        Select09->setAutoDefault(false);

        verticalLayout->addWidget(Select09);

        Select10 = new QPushButton(SelectMethodDialog);
        Select10->setObjectName(QStringLiteral("Select10"));
        sizePolicy.setHeightForWidth(Select10->sizePolicy().hasHeightForWidth());
        Select10->setSizePolicy(sizePolicy);
        Select10->setCheckable(true);
        Select10->setAutoExclusive(true);
        Select10->setAutoDefault(false);

        verticalLayout->addWidget(Select10);

        Select11 = new QPushButton(SelectMethodDialog);
        Select11->setObjectName(QStringLiteral("Select11"));
        sizePolicy.setHeightForWidth(Select11->sizePolicy().hasHeightForWidth());
        Select11->setSizePolicy(sizePolicy);
        Select11->setCheckable(true);
        Select11->setAutoExclusive(true);
        Select11->setAutoDefault(false);

        verticalLayout->addWidget(Select11);

        Select12 = new QPushButton(SelectMethodDialog);
        Select12->setObjectName(QStringLiteral("Select12"));
        sizePolicy.setHeightForWidth(Select12->sizePolicy().hasHeightForWidth());
        Select12->setSizePolicy(sizePolicy);
        Select12->setCheckable(true);
        Select12->setAutoExclusive(true);
        Select12->setAutoDefault(false);

        verticalLayout->addWidget(Select12);

        Select13 = new QPushButton(SelectMethodDialog);
        Select13->setObjectName(QStringLiteral("Select13"));
        sizePolicy.setHeightForWidth(Select13->sizePolicy().hasHeightForWidth());
        Select13->setSizePolicy(sizePolicy);
        Select13->setCheckable(true);
        Select13->setAutoExclusive(true);
        Select13->setAutoDefault(false);

        verticalLayout->addWidget(Select13);

        Select14 = new QPushButton(SelectMethodDialog);
        Select14->setObjectName(QStringLiteral("Select14"));
        sizePolicy.setHeightForWidth(Select14->sizePolicy().hasHeightForWidth());
        Select14->setSizePolicy(sizePolicy);
        Select14->setCheckable(true);
        Select14->setAutoExclusive(true);
        Select14->setAutoDefault(false);

        verticalLayout->addWidget(Select14);

        Select15 = new QPushButton(SelectMethodDialog);
        Select15->setObjectName(QStringLiteral("Select15"));
        sizePolicy.setHeightForWidth(Select15->sizePolicy().hasHeightForWidth());
        Select15->setSizePolicy(sizePolicy);
        Select15->setCheckable(true);
        Select15->setAutoExclusive(true);
        Select15->setAutoDefault(false);

        verticalLayout->addWidget(Select15);

        Select16 = new QPushButton(SelectMethodDialog);
        Select16->setObjectName(QStringLiteral("Select16"));
        sizePolicy.setHeightForWidth(Select16->sizePolicy().hasHeightForWidth());
        Select16->setSizePolicy(sizePolicy);
        Select16->setCheckable(true);
        Select16->setAutoExclusive(true);
        Select16->setAutoDefault(false);

        verticalLayout->addWidget(Select16);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        label = new QLabel(SelectMethodDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        MethodSearch = new LineEdit(SelectMethodDialog);
        MethodSearch->setObjectName(QStringLiteral("MethodSearch"));

        gridLayout->addWidget(MethodSearch, 1, 2, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        EditMethod = new QPushButton(SelectMethodDialog);
        EditMethod->setObjectName(QStringLiteral("EditMethod"));
        EditMethod->setEnabled(false);

        horizontalLayout_2->addWidget(EditMethod);

        NewMethod = new QPushButton(SelectMethodDialog);
        NewMethod->setObjectName(QStringLiteral("NewMethod"));
        NewMethod->setEnabled(false);

        horizontalLayout_2->addWidget(NewMethod);

        LoadCCCollection = new QPushButton(SelectMethodDialog);
        LoadCCCollection->setObjectName(QStringLiteral("LoadCCCollection"));

        horizontalLayout_2->addWidget(LoadCCCollection);

        buttonBox = new QDialogButtonBox(SelectMethodDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_2->addWidget(buttonBox);


        gridLayout->addLayout(horizontalLayout_2, 2, 2, 1, 1);

        MethodDisplay = new QStackedWidget(SelectMethodDialog);
        MethodDisplay->setObjectName(QStringLiteral("MethodDisplay"));

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
        SelectMethodDialog->setWindowTitle(QApplication::translate("SelectMethodDialog", "Select Method", Q_NULLPTR));
        FilterPrinciples->setText(QApplication::translate("SelectMethodDialog", "Principles", Q_NULLPTR));
        FilterPlain->setText(QApplication::translate("SelectMethodDialog", "Plain", Q_NULLPTR));
        FilterTrebleBob->setText(QApplication::translate("SelectMethodDialog", "Treble Bob", Q_NULLPTR));
        FilterDelight->setText(QApplication::translate("SelectMethodDialog", "Delight", Q_NULLPTR));
        FilterSurprise->setText(QApplication::translate("SelectMethodDialog", "Surprise", Q_NULLPTR));
        FilterOther->setText(QApplication::translate("SelectMethodDialog", "Other", Q_NULLPTR));
        FilterSelectAll->setText(QApplication::translate("SelectMethodDialog", "All", Q_NULLPTR));
        FilterSelectNone->setText(QApplication::translate("SelectMethodDialog", "None", Q_NULLPTR));
        Select03->setText(QApplication::translate("SelectMethodDialog", "Singles", Q_NULLPTR));
        Select04->setText(QApplication::translate("SelectMethodDialog", "Minimus", Q_NULLPTR));
        Select05->setText(QApplication::translate("SelectMethodDialog", "Doubles", Q_NULLPTR));
        Select06->setText(QApplication::translate("SelectMethodDialog", "Minor", Q_NULLPTR));
        Select07->setText(QApplication::translate("SelectMethodDialog", "Triples", Q_NULLPTR));
        Select08->setText(QApplication::translate("SelectMethodDialog", "Major", Q_NULLPTR));
        Select09->setText(QApplication::translate("SelectMethodDialog", "Caters", Q_NULLPTR));
        Select10->setText(QApplication::translate("SelectMethodDialog", "Royal", Q_NULLPTR));
        Select11->setText(QApplication::translate("SelectMethodDialog", "Cinques", Q_NULLPTR));
        Select12->setText(QApplication::translate("SelectMethodDialog", "Maximus", Q_NULLPTR));
        Select13->setText(QApplication::translate("SelectMethodDialog", "Sextuples", Q_NULLPTR));
        Select14->setText(QApplication::translate("SelectMethodDialog", "Fourteen", Q_NULLPTR));
        Select15->setText(QApplication::translate("SelectMethodDialog", "Septuples", Q_NULLPTR));
        Select16->setText(QApplication::translate("SelectMethodDialog", "Sixteen", Q_NULLPTR));
        label->setText(QApplication::translate("SelectMethodDialog", "Search:", Q_NULLPTR));
        MethodSearch->setPlaceholderText(QApplication::translate("SelectMethodDialog", "Search", Q_NULLPTR));
        EditMethod->setText(QApplication::translate("SelectMethodDialog", "Edit method...", Q_NULLPTR));
        NewMethod->setText(QApplication::translate("SelectMethodDialog", "New method...", Q_NULLPTR));
        LoadCCCollection->setText(QApplication::translate("SelectMethodDialog", "Load CC Collection", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SelectMethodDialog: public Ui_SelectMethodDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_2D_SELECT_2D_METHOD_H
