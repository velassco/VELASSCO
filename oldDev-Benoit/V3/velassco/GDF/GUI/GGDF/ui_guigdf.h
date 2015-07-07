/********************************************************************************
** Form generated from reading UI file 'guigdf.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUIGDF_H
#define UI_GUIGDF_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GuiGDF
{
public:
    QAction *actionOpen;
    QAction *action_Close;
    QAction *actionExit;
    QWidget *centralWidget;
    QPushButton *ping;
    QWidget *layoutWidget;
    QHBoxLayout *cmd;
    QPushButton *write;
    QPushButton *read;
    QPushButton *grep;
    QPushButton *ls;
    QFrame *frame;
    QLabel *status;
    QWidget *layoutWidget1;
    QHBoxLayout *services;
    QPushButton *startAll;
    QPushButton *stopAll;
    QPushButton *statusAll;
    QTextEdit *contentText;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QLineEdit *path;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_3;
    QLineEdit *fileName;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QComboBox *type;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GuiGDF)
    {
        if (GuiGDF->objectName().isEmpty())
            GuiGDF->setObjectName(QStringLiteral("GuiGDF"));
        GuiGDF->resize(543, 507);
        actionOpen = new QAction(GuiGDF);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        action_Close = new QAction(GuiGDF);
        action_Close->setObjectName(QStringLiteral("action_Close"));
        actionExit = new QAction(GuiGDF);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        centralWidget = new QWidget(GuiGDF);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ping = new QPushButton(centralWidget);
        ping->setObjectName(QStringLiteral("ping"));
        ping->setGeometry(QRect(500, 20, 31, 32));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 60, 511, 33));
        cmd = new QHBoxLayout(layoutWidget);
        cmd->setSpacing(6);
        cmd->setContentsMargins(11, 11, 11, 11);
        cmd->setObjectName(QStringLiteral("cmd"));
        cmd->setContentsMargins(0, 0, 0, 0);
        write = new QPushButton(layoutWidget);
        write->setObjectName(QStringLiteral("write"));

        cmd->addWidget(write);

        read = new QPushButton(layoutWidget);
        read->setObjectName(QStringLiteral("read"));

        cmd->addWidget(read);

        grep = new QPushButton(layoutWidget);
        grep->setObjectName(QStringLiteral("grep"));

        cmd->addWidget(grep);

        ls = new QPushButton(layoutWidget);
        ls->setObjectName(QStringLiteral("ls"));

        cmd->addWidget(ls);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(20, 150, 341, 301));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        status = new QLabel(frame);
        status->setObjectName(QStringLiteral("status"));
        status->setGeometry(QRect(0, 0, 341, 351));
        status->setLayoutDirection(Qt::LeftToRight);
        status->setScaledContents(false);
        status->setMargin(1);
        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(20, 20, 260, 32));
        services = new QHBoxLayout(layoutWidget1);
        services->setSpacing(6);
        services->setContentsMargins(11, 11, 11, 11);
        services->setObjectName(QStringLiteral("services"));
        services->setContentsMargins(0, 0, 0, 0);
        startAll = new QPushButton(layoutWidget1);
        startAll->setObjectName(QStringLiteral("startAll"));

        services->addWidget(startAll);

        stopAll = new QPushButton(layoutWidget1);
        stopAll->setObjectName(QStringLiteral("stopAll"));

        services->addWidget(stopAll);

        statusAll = new QPushButton(layoutWidget1);
        statusAll->setObjectName(QStringLiteral("statusAll"));

        services->addWidget(statusAll);

        contentText = new QTextEdit(centralWidget);
        contentText->setObjectName(QStringLiteral("contentText"));
        contentText->setGeometry(QRect(370, 150, 161, 301));
        layoutWidget2 = new QWidget(centralWidget);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(20, 96, 461, 51));
        horizontalLayout = new QHBoxLayout(layoutWidget2);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_2 = new QLabel(layoutWidget2);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_2->addWidget(label_2);

        path = new QLineEdit(layoutWidget2);
        path->setObjectName(QStringLiteral("path"));

        verticalLayout_2->addWidget(path);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_3 = new QLabel(layoutWidget2);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_3->addWidget(label_3);

        fileName = new QLineEdit(layoutWidget2);
        fileName->setObjectName(QStringLiteral("fileName"));

        verticalLayout_3->addWidget(fileName);


        horizontalLayout->addLayout(verticalLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(layoutWidget2);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);


        horizontalLayout->addLayout(verticalLayout);

        type = new QComboBox(centralWidget);
        type->setObjectName(QStringLiteral("type"));
        type->setGeometry(QRect(320, 20, 104, 26));
        GuiGDF->setCentralWidget(centralWidget);
        layoutWidget->raise();
        ping->raise();
        layoutWidget->raise();
        frame->raise();
        layoutWidget->raise();
        contentText->raise();
        type->raise();
        menuBar = new QMenuBar(GuiGDF);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 543, 22));
        menuBar->setFocusPolicy(Qt::TabFocus);
        menuBar->setAcceptDrops(false);
        menuBar->setDefaultUp(true);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        GuiGDF->setMenuBar(menuBar);
        statusBar = new QStatusBar(GuiGDF);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        GuiGDF->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(action_Close);
        menuFile->addAction(actionExit);

        retranslateUi(GuiGDF);

        QMetaObject::connectSlotsByName(GuiGDF);
    } // setupUi

    void retranslateUi(QMainWindow *GuiGDF)
    {
        GuiGDF->setWindowTitle(QApplication::translate("GuiGDF", "GuiGDF", 0));
        actionOpen->setText(QApplication::translate("GuiGDF", "Connect", 0));
        action_Close->setText(QApplication::translate("GuiGDF", "Close", 0));
        actionExit->setText(QApplication::translate("GuiGDF", "Exit", 0));
        ping->setText(QString());
        write->setText(QApplication::translate("GuiGDF", "Write", 0));
        read->setText(QApplication::translate("GuiGDF", "Read", 0));
        grep->setText(QApplication::translate("GuiGDF", "Grep", 0));
        ls->setText(QApplication::translate("GuiGDF", "List", 0));
        status->setText(QString());
        startAll->setText(QApplication::translate("GuiGDF", "StartAll", 0));
        stopAll->setText(QApplication::translate("GuiGDF", "StopAll", 0));
        statusAll->setText(QApplication::translate("GuiGDF", "Status", 0));
        label_2->setText(QApplication::translate("GuiGDF", "Path", 0));
        label_3->setText(QApplication::translate("GuiGDF", "File", 0));
        label->setText(QApplication::translate("GuiGDF", "Content", 0));
        type->clear();
        type->insertItems(0, QStringList()
         << QApplication::translate("GuiGDF", "FS", 0)
         << QApplication::translate("GuiGDF", "Hadoop", 0)
         << QApplication::translate("GuiGDF", "Hive", 0)
         << QApplication::translate("GuiGDF", "Hbase", 0)
        );
        menuFile->setTitle(QApplication::translate("GuiGDF", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class GuiGDF: public Ui_GuiGDF {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUIGDF_H
