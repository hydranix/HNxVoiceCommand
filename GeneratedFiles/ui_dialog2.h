/********************************************************************************
** Form generated from reading UI file 'dialog2.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG2_H
#define UI_DIALOG2_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog2
{
public:
    QPushButton *pushButton_ok;
    QPushButton *pushButton_cancel;
    QLineEdit *lineEdit_prog;
    QLineEdit *lineEdit_param;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton_browse;
    QLineEdit *lineEdit_phrase;
    QLabel *label_3;

    void setupUi(QDialog *Dialog2)
    {
        if (Dialog2->objectName().isEmpty())
            Dialog2->setObjectName("Dialog2");
        Dialog2->resize(400, 250);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Dialog2->sizePolicy().hasHeightForWidth());
        Dialog2->setSizePolicy(sizePolicy);
        Dialog2->setMinimumSize(QSize(400, 250));
        Dialog2->setMaximumSize(QSize(400, 250));
        pushButton_ok = new QPushButton(Dialog2);
        pushButton_ok->setObjectName("pushButton_ok");
        pushButton_ok->setEnabled(false);
        pushButton_ok->setGeometry(QRect(220, 210, 80, 31));
        pushButton_cancel = new QPushButton(Dialog2);
        pushButton_cancel->setObjectName("pushButton_cancel");
        pushButton_cancel->setGeometry(QRect(310, 210, 80, 31));
        lineEdit_prog = new QLineEdit(Dialog2);
        lineEdit_prog->setObjectName("lineEdit_prog");
        lineEdit_prog->setGeometry(QRect(90, 100, 301, 31));
        lineEdit_param = new QLineEdit(Dialog2);
        lineEdit_param->setObjectName("lineEdit_param");
        lineEdit_param->setGeometry(QRect(150, 160, 241, 31));
        label = new QLabel(Dialog2);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 100, 61, 31));
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(Dialog2);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 160, 101, 31));
        label_2->setAlignment(Qt::AlignCenter);
        pushButton_browse = new QPushButton(Dialog2);
        pushButton_browse->setObjectName("pushButton_browse");
        pushButton_browse->setEnabled(false);
        pushButton_browse->setGeometry(QRect(10, 210, 80, 31));
        lineEdit_phrase = new QLineEdit(Dialog2);
        lineEdit_phrase->setObjectName("lineEdit_phrase");
        lineEdit_phrase->setGeometry(QRect(90, 20, 301, 31));
        label_3 = new QLabel(Dialog2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 20, 51, 31));
        label_3->setAlignment(Qt::AlignCenter);

        retranslateUi(Dialog2);

        QMetaObject::connectSlotsByName(Dialog2);
    } // setupUi

    void retranslateUi(QDialog *Dialog2)
    {
        Dialog2->setWindowTitle(QCoreApplication::translate("Dialog2", "Add New Command", nullptr));
        pushButton_ok->setText(QCoreApplication::translate("Dialog2", "Ok", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("Dialog2", "Cancel", nullptr));
        label->setText(QCoreApplication::translate("Dialog2", "Program", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog2", "Optional Parameters", nullptr));
        pushButton_browse->setText(QCoreApplication::translate("Dialog2", "Browse", nullptr));
        label_3->setText(QCoreApplication::translate("Dialog2", "Phrase", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog2: public Ui_Dialog2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG2_H
