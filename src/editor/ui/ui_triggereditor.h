/********************************************************************************
** Form generated from reading UI file 'triggereditor.ui'
**
** Created: Mon Jun 27 20:16:26 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIGGEREDITOR_H
#define UI_TRIGGEREDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TriggerEditor
{
public:
    QGridLayout *gridLayout;
    QTreeWidget *treeWidget;
    QWidget *m_triggerWidget;

    void setupUi(QWidget *TriggerEditor)
    {
        if (TriggerEditor->objectName().isEmpty())
            TriggerEditor->setObjectName(QString::fromUtf8("TriggerEditor"));
        TriggerEditor->resize(491, 367);
        gridLayout = new QGridLayout(TriggerEditor);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        treeWidget = new QTreeWidget(TriggerEditor);
        new QTreeWidgetItem(treeWidget);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));

        gridLayout->addWidget(treeWidget, 0, 0, 1, 1);

        m_triggerWidget = new QWidget(TriggerEditor);
        m_triggerWidget->setObjectName(QString::fromUtf8("m_triggerWidget"));

        gridLayout->addWidget(m_triggerWidget, 0, 1, 1, 1);


        retranslateUi(TriggerEditor);

        QMetaObject::connectSlotsByName(TriggerEditor);
    } // setupUi

    void retranslateUi(QWidget *TriggerEditor)
    {
        TriggerEditor->setWindowTitle(QApplication::translate("TriggerEditor", "Form", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("TriggerEditor", "Triggers", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = treeWidget->isSortingEnabled();
        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("TriggerEditor", "Map script", 0, QApplication::UnicodeUTF8));
        treeWidget->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class TriggerEditor: public Ui_TriggerEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIGGEREDITOR_H
