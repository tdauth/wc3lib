/********************************************************************************
** Form generated from reading UI file 'modeleditor.ui'
**
** Created: Mon Jun 27 20:16:26 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODELEDITOR_H
#define UI_MODELEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ModelEditor
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *m_horizontalLayout;

    void setupUi(QWidget *ModelEditor)
    {
        if (ModelEditor->objectName().isEmpty())
            ModelEditor->setObjectName(QString::fromUtf8("ModelEditor"));
        ModelEditor->resize(784, 489);
        gridLayout = new QGridLayout(ModelEditor);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_horizontalLayout = new QHBoxLayout();
        m_horizontalLayout->setObjectName(QString::fromUtf8("m_horizontalLayout"));

        gridLayout->addLayout(m_horizontalLayout, 0, 0, 1, 1);


        retranslateUi(ModelEditor);

        QMetaObject::connectSlotsByName(ModelEditor);
    } // setupUi

    void retranslateUi(QWidget *ModelEditor)
    {
        ModelEditor->setWindowTitle(QApplication::translate("ModelEditor", "Model Editor", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ModelEditor: public Ui_ModelEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODELEDITOR_H
