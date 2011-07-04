/********************************************************************************
** Form generated from reading UI file 'triggertopwidget.ui'
**
** Created: Mon Jun 27 20:16:26 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIGGERTOPWIDGET_H
#define UI_TRIGGERTOPWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include "ktextedit.h"

QT_BEGIN_NAMESPACE

class Ui_TriggerTopWidget
{
public:
    QGridLayout *gridLayout;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QLabel *label;
    KTextEdit *m_triggerCommentTextEdit;
    QLabel *label_2;

    void setupUi(QWidget *TriggerTopWidget)
    {
        if (TriggerTopWidget->objectName().isEmpty())
            TriggerTopWidget->setObjectName(QString::fromUtf8("TriggerTopWidget"));
        TriggerTopWidget->resize(400, 300);
        gridLayout = new QGridLayout(TriggerTopWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        checkBox = new QCheckBox(TriggerTopWidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 0, 0, 1, 1);

        checkBox_2 = new QCheckBox(TriggerTopWidget);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        gridLayout->addWidget(checkBox_2, 0, 1, 1, 1);

        label = new QLabel(TriggerTopWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        m_triggerCommentTextEdit = new KTextEdit(TriggerTopWidget);
        m_triggerCommentTextEdit->setObjectName(QString::fromUtf8("m_triggerCommentTextEdit"));

        gridLayout->addWidget(m_triggerCommentTextEdit, 2, 0, 1, 2);

        label_2 = new QLabel(TriggerTopWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);


        retranslateUi(TriggerTopWidget);

        QMetaObject::connectSlotsByName(TriggerTopWidget);
    } // setupUi

    void retranslateUi(QWidget *TriggerTopWidget)
    {
        TriggerTopWidget->setWindowTitle(QApplication::translate("TriggerTopWidget", "Form", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("TriggerTopWidget", "Enabled", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("TriggerTopWidget", "On at beginning", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TriggerTopWidget", "Trigger comment", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("TriggerTopWidget", "Trigger functions", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TriggerTopWidget: public Ui_TriggerTopWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIGGERTOPWIDGET_H
