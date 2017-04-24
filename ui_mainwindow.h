/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *replaceTooltips;
    QPushButton *languageTip;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *createExe;
    QPushButton *exeTip;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *hotkeyChoice;
    QPushButton *hotkeyTip;
    QPushButton *runButton;
    QLabel *label;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 300);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayoutWidget = new QWidget(centralWidget);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(30, 10, 291, 141));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        replaceTooltips = new QCheckBox(gridLayoutWidget);
        replaceTooltips->setObjectName(QStringLiteral("replaceTooltips"));

        horizontalLayout_3->addWidget(replaceTooltips);

        languageTip = new QPushButton(gridLayoutWidget);
        languageTip->setObjectName(QStringLiteral("languageTip"));
        languageTip->setMaximumSize(QSize(25, 16777215));
        languageTip->setToolTipDuration(0);

        horizontalLayout_3->addWidget(languageTip);


        gridLayout->addLayout(horizontalLayout_3, 5, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        createExe = new QCheckBox(gridLayoutWidget);
        createExe->setObjectName(QStringLiteral("createExe"));

        horizontalLayout_2->addWidget(createExe);

        exeTip = new QPushButton(gridLayoutWidget);
        exeTip->setObjectName(QStringLiteral("exeTip"));
        exeTip->setMaximumSize(QSize(25, 16777215));
        exeTip->setToolTipDuration(0);

        horizontalLayout_2->addWidget(exeTip);


        gridLayout->addLayout(horizontalLayout_2, 6, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, -1, -1, 0);
        hotkeyChoice = new QComboBox(gridLayoutWidget);
        hotkeyChoice->setObjectName(QStringLiteral("hotkeyChoice"));

        horizontalLayout_4->addWidget(hotkeyChoice);

        hotkeyTip = new QPushButton(gridLayoutWidget);
        hotkeyTip->setObjectName(QStringLiteral("hotkeyTip"));
        hotkeyTip->setMaximumSize(QSize(25, 16777215));
        hotkeyTip->setToolTipDuration(0);

        horizontalLayout_4->addWidget(hotkeyTip);


        gridLayout->addLayout(horizontalLayout_4, 4, 0, 1, 1);

        runButton = new QPushButton(centralWidget);
        runButton->setObjectName(QStringLiteral("runButton"));
        runButton->setGeometry(QRect(160, 230, 75, 23));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 160, 401, 61));
        label->setAlignment(Qt::AlignCenter);
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        replaceTooltips->setText(QApplication::translate("MainWindow", "Improve tooltips and tech tree descriptions", Q_NULLPTR));
        languageTip->setText(QString());
        createExe->setText(QApplication::translate("MainWindow", "Create an offline installation", Q_NULLPTR));
        exeTip->setText(QString());
        hotkeyChoice->clear();
        hotkeyChoice->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Choose which hotkeys to use...", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Use current AoC/Voobly hotkeys", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Use HD hotkeys for this mod only", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Use HD hotkeys for this mod and AoC", Q_NULLPTR)
        );
        hotkeyTip->setText(QString());
        runButton->setText(QApplication::translate("MainWindow", "Run", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
