/********************************************************************************
** Form generated from reading UI file 'renderstatswidget.ui'
**
** Created: Mon Jun 27 20:16:26 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENDERSTATSWIDGET_H
#define UI_RENDERSTATSWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RenderStatsWidget
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *m_bestFrameTimeLabel;
    QLabel *m_worstFrameTimeLabel;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *m_lastFpsLabel;
    QLabel *m_averageFpsLabel;
    QLabel *m_bestFpsLabel;
    QLabel *m_worstFpsLabel;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_4;
    QLabel *m_triangleCountLabel;

    void setupUi(QWidget *RenderStatsWidget)
    {
        if (RenderStatsWidget->objectName().isEmpty())
            RenderStatsWidget->setObjectName(QString::fromUtf8("RenderStatsWidget"));
        RenderStatsWidget->resize(400, 300);
        gridLayout = new QGridLayout(RenderStatsWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox_2 = new QGroupBox(RenderStatsWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        m_bestFrameTimeLabel = new QLabel(groupBox_2);
        m_bestFrameTimeLabel->setObjectName(QString::fromUtf8("m_bestFrameTimeLabel"));

        verticalLayout_2->addWidget(m_bestFrameTimeLabel);

        m_worstFrameTimeLabel = new QLabel(groupBox_2);
        m_worstFrameTimeLabel->setObjectName(QString::fromUtf8("m_worstFrameTimeLabel"));

        verticalLayout_2->addWidget(m_worstFrameTimeLabel);


        gridLayout_3->addLayout(verticalLayout_2, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox_2, 0, 0, 1, 1);

        groupBox = new QGroupBox(RenderStatsWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        m_lastFpsLabel = new QLabel(groupBox);
        m_lastFpsLabel->setObjectName(QString::fromUtf8("m_lastFpsLabel"));

        verticalLayout->addWidget(m_lastFpsLabel);

        m_averageFpsLabel = new QLabel(groupBox);
        m_averageFpsLabel->setObjectName(QString::fromUtf8("m_averageFpsLabel"));

        verticalLayout->addWidget(m_averageFpsLabel);

        m_bestFpsLabel = new QLabel(groupBox);
        m_bestFpsLabel->setObjectName(QString::fromUtf8("m_bestFpsLabel"));

        verticalLayout->addWidget(m_bestFpsLabel);

        m_worstFpsLabel = new QLabel(groupBox);
        m_worstFpsLabel->setObjectName(QString::fromUtf8("m_worstFpsLabel"));

        verticalLayout->addWidget(m_worstFpsLabel);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 1, 2, 1);

        groupBox_3 = new QGroupBox(RenderStatsWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_4 = new QGridLayout(groupBox_3);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        m_triangleCountLabel = new QLabel(groupBox_3);
        m_triangleCountLabel->setObjectName(QString::fromUtf8("m_triangleCountLabel"));

        gridLayout_4->addWidget(m_triangleCountLabel, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox_3, 1, 0, 1, 1);


        retranslateUi(RenderStatsWidget);

        QMetaObject::connectSlotsByName(RenderStatsWidget);
    } // setupUi

    void retranslateUi(QWidget *RenderStatsWidget)
    {
        RenderStatsWidget->setWindowTitle(QApplication::translate("RenderStatsWidget", "Render Stats", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("RenderStatsWidget", "Frame Time", 0, QApplication::UnicodeUTF8));
        m_bestFrameTimeLabel->setText(QApplication::translate("RenderStatsWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        m_worstFrameTimeLabel->setText(QApplication::translate("RenderStatsWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("RenderStatsWidget", "FPS", 0, QApplication::UnicodeUTF8));
        m_lastFpsLabel->setText(QApplication::translate("RenderStatsWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        m_averageFpsLabel->setText(QApplication::translate("RenderStatsWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        m_bestFpsLabel->setText(QApplication::translate("RenderStatsWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        m_worstFpsLabel->setText(QApplication::translate("RenderStatsWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("RenderStatsWidget", "Scene", 0, QApplication::UnicodeUTF8));
        m_triangleCountLabel->setText(QApplication::translate("RenderStatsWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RenderStatsWidget: public Ui_RenderStatsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENDERSTATSWIDGET_H
