/********************************************************************************
** Form generated from reading UI file 'integerdialog.ui'
**
** Created: Mon Jun 27 20:16:26 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTEGERDIALOG_H
#define UI_INTEGERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include "kdialogbuttonbox.h"
#include "knuminput.h"

QT_BEGIN_NAMESPACE

class Ui_IntegerDialog
{
public:
    QGridLayout *gridLayout;
    KDialogButtonBox *IntegerDialog_2;
    KIntSpinBox *m_intSpinBox;

    void setupUi(QDialog *IntegerDialog)
    {
        if (IntegerDialog->objectName().isEmpty())
            IntegerDialog->setObjectName(QString::fromUtf8("IntegerDialog"));
        IntegerDialog->resize(167, 61);
        gridLayout = new QGridLayout(IntegerDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        IntegerDialog_2 = new KDialogButtonBox(IntegerDialog);
        IntegerDialog_2->setObjectName(QString::fromUtf8("IntegerDialog_2"));
        IntegerDialog_2->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(IntegerDialog_2, 2, 0, 1, 1);

        m_intSpinBox = new KIntSpinBox(IntegerDialog);
        m_intSpinBox->setObjectName(QString::fromUtf8("m_intSpinBox"));

        gridLayout->addWidget(m_intSpinBox, 1, 0, 1, 1);


        retranslateUi(IntegerDialog);
        QObject::connect(IntegerDialog_2, SIGNAL(accepted()), IntegerDialog, SLOT(accept()));
        QObject::connect(IntegerDialog_2, SIGNAL(rejected()), IntegerDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(IntegerDialog);
    } // setupUi

    void retranslateUi(QDialog *IntegerDialog)
    {
        IntegerDialog->setWindowTitle(QApplication::translate("IntegerDialog", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class IntegerDialog: public Ui_IntegerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTEGERDIALOG_H
