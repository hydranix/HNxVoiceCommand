/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPushButton *pushButton_add;
    QPushButton *pushButton_remove;
    QPushButton *pushButton_hide;
    QPushButton *pushButton_exit;
    QTableWidget *tableWidget;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(470, 440);
        pushButton_add = new QPushButton(Dialog);
        pushButton_add->setObjectName("pushButton_add");
        pushButton_add->setGeometry(QRect(380, 10, 80, 30));
        pushButton_remove = new QPushButton(Dialog);
        pushButton_remove->setObjectName("pushButton_remove");
        pushButton_remove->setEnabled(false);
        pushButton_remove->setGeometry(QRect(380, 60, 80, 30));
        pushButton_hide = new QPushButton(Dialog);
        pushButton_hide->setObjectName("pushButton_hide");
        pushButton_hide->setGeometry(QRect(380, 360, 80, 30));
        pushButton_exit = new QPushButton(Dialog);
        pushButton_exit->setObjectName("pushButton_exit");
        pushButton_exit->setGeometry(QRect(380, 400, 80, 30));
        tableWidget = new QTableWidget(Dialog);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setGeometry(QRect(10, 10, 361, 421));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tableWidget->sizePolicy().hasHeightForWidth());
        tableWidget->setSizePolicy(sizePolicy);
        tableWidget->setSizeIncrement(QSize(0, 0));
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setDragDropOverwriteMode(false);
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidget->setWordWrap(false);
        tableWidget->setCornerButtonEnabled(false);
        tableWidget->setColumnCount(2);
        tableWidget->horizontalHeader()->setVisible(false);
        tableWidget->verticalHeader()->setVisible(false);

        retranslateUi(Dialog);

        pushButton_add->setDefault(true);


        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "HNx Voice Commands", nullptr));
        pushButton_add->setText(QCoreApplication::translate("Dialog", "Add", nullptr));
        pushButton_remove->setText(QCoreApplication::translate("Dialog", "Remove", nullptr));
        pushButton_hide->setText(QCoreApplication::translate("Dialog", "Hide", nullptr));
        pushButton_exit->setText(QCoreApplication::translate("Dialog", "Exit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
