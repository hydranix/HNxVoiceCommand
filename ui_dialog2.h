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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog2
{
public:
    QWidget *widget;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *lineEdit_param;
    QWidget *widget1;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QLineEdit *lineEdit_phrase;
    QWidget *widget2;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_browse;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_ok;
    QPushButton *pushButton_cancel;
    QWidget *widget3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_prog;

    void setupUi(QDialog *Dialog2)
    {
        if (Dialog2->objectName().isEmpty())
            Dialog2->setObjectName("Dialog2");
        Dialog2->resize(300, 150);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Dialog2->sizePolicy().hasHeightForWidth());
        Dialog2->setSizePolicy(sizePolicy);
        Dialog2->setMinimumSize(QSize(300, 150));
        Dialog2->setMaximumSize(QSize(500, 150));
        widget = new QWidget(Dialog2);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 78, 248, 23));
        horizontalLayout_3 = new QHBoxLayout(widget);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(label_2);

        lineEdit_param = new QLineEdit(widget);
        lineEdit_param->setObjectName("lineEdit_param");

        horizontalLayout_3->addWidget(lineEdit_param);

        widget1 = new QWidget(Dialog2);
        widget1->setObjectName("widget1");
        widget1->setGeometry(QRect(10, 10, 176, 23));
        horizontalLayout_4 = new QHBoxLayout(widget1);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(widget1);
        label_3->setObjectName("label_3");
        label_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(label_3);

        lineEdit_phrase = new QLineEdit(widget1);
        lineEdit_phrase->setObjectName("lineEdit_phrase");

        horizontalLayout_4->addWidget(lineEdit_phrase);

        widget2 = new QWidget(Dialog2);
        widget2->setObjectName("widget2");
        widget2->setGeometry(QRect(10, 112, 280, 28));
        horizontalLayout = new QHBoxLayout(widget2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_browse = new QPushButton(widget2);
        pushButton_browse->setObjectName("pushButton_browse");
        pushButton_browse->setEnabled(false);

        horizontalLayout->addWidget(pushButton_browse);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_ok = new QPushButton(widget2);
        pushButton_ok->setObjectName("pushButton_ok");
        pushButton_ok->setEnabled(false);

        horizontalLayout->addWidget(pushButton_ok);

        pushButton_cancel = new QPushButton(widget2);
        pushButton_cancel->setObjectName("pushButton_cancel");

        horizontalLayout->addWidget(pushButton_cancel);

        widget3 = new QWidget(Dialog2);
        widget3->setObjectName("widget3");
        widget3->setGeometry(QRect(10, 44, 186, 23));
        horizontalLayout_2 = new QHBoxLayout(widget3);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget3);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label);

        lineEdit_prog = new QLineEdit(widget3);
        lineEdit_prog->setObjectName("lineEdit_prog");

        horizontalLayout_2->addWidget(lineEdit_prog);


        retranslateUi(Dialog2);

        QMetaObject::connectSlotsByName(Dialog2);
    } // setupUi

    void retranslateUi(QDialog *Dialog2)
    {
        Dialog2->setWindowTitle(QCoreApplication::translate("Dialog2", "Add New Command", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog2", "Optional Parameters", nullptr));
        label_3->setText(QCoreApplication::translate("Dialog2", "Phrase", nullptr));
        pushButton_browse->setText(QCoreApplication::translate("Dialog2", "Browse", nullptr));
        pushButton_ok->setText(QCoreApplication::translate("Dialog2", "Ok", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("Dialog2", "Cancel", nullptr));
        label->setText(QCoreApplication::translate("Dialog2", "Program", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog2: public Ui_Dialog2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG2_H
