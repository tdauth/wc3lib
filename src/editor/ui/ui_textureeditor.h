/********************************************************************************
** Form generated from reading UI file 'textureeditor.ui'
**
** Created: Mon Jun 27 20:16:26 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTUREEDITOR_H
#define UI_TEXTUREEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TextureEditor
{
public:
    QGridLayout *gridLayout_2;
    QLabel *m_imageLabel;

    void setupUi(QWidget *TextureEditor)
    {
        if (TextureEditor->objectName().isEmpty())
            TextureEditor->setObjectName(QString::fromUtf8("TextureEditor"));
        TextureEditor->resize(620, 474);
        gridLayout_2 = new QGridLayout(TextureEditor);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        m_imageLabel = new QLabel(TextureEditor);
        m_imageLabel->setObjectName(QString::fromUtf8("m_imageLabel"));

        gridLayout_2->addWidget(m_imageLabel, 0, 0, 1, 1);


        retranslateUi(TextureEditor);

        QMetaObject::connectSlotsByName(TextureEditor);
    } // setupUi

    void retranslateUi(QWidget *TextureEditor)
    {
        TextureEditor->setWindowTitle(QApplication::translate("TextureEditor", "Texture Editor", 0, QApplication::UnicodeUTF8));
        m_imageLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TextureEditor: public Ui_TextureEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTUREEDITOR_H
