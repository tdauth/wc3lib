/********************************************************************************
** Form generated from reading UI file 'teamcolordialog.ui'
**
** Created: Mon Jun 27 20:16:26 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEAMCOLORDIALOG_H
#define UI_TEAMCOLORDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include "kcolorcombo.h"
#include "kdialogbuttonbox.h"

QT_BEGIN_NAMESPACE

class Ui_TeamColorDialog
{
public:
    QGridLayout *gridLayout;
    KColorCombo *m_colorComboBox;
    KDialogButtonBox *m_dialogButtonBox;

    void setupUi(QDialog *TeamColorDialog)
    {
        if (TeamColorDialog->objectName().isEmpty())
            TeamColorDialog->setObjectName(QString::fromUtf8("TeamColorDialog"));
        TeamColorDialog->resize(167, 62);
        gridLayout = new QGridLayout(TeamColorDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_colorComboBox = new KColorCombo(TeamColorDialog);
        m_colorComboBox->setObjectName(QString::fromUtf8("m_colorComboBox"));

        gridLayout->addWidget(m_colorComboBox, 0, 0, 1, 1);

        m_dialogButtonBox = new KDialogButtonBox(TeamColorDialog);
        m_dialogButtonBox->setObjectName(QString::fromUtf8("m_dialogButtonBox"));
        m_dialogButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(m_dialogButtonBox, 1, 0, 1, 1);


        retranslateUi(TeamColorDialog);
        QObject::connect(m_dialogButtonBox, SIGNAL(accepted()), TeamColorDialog, SLOT(accept()));
        QObject::connect(m_dialogButtonBox, SIGNAL(rejected()), TeamColorDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(TeamColorDialog);
    } // setupUi

    void retranslateUi(QDialog *TeamColorDialog)
    {
        TeamColorDialog->setWindowTitle(QApplication::translate("TeamColorDialog", "Team Color", 0, QApplication::UnicodeUTF8));
        m_colorComboBox->clear();
        m_colorComboBox->insertItems(0, QStringList()
         << QApplication::translate("TeamColorDialog", "Red", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Blue", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Teal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Purple", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Yellow", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Orange", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Green", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Pink", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Gray", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Light Blue", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Dark Green", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Brown", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("TeamColorDialog", "Black", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class TeamColorDialog: public Ui_TeamColorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEAMCOLORDIALOG_H
