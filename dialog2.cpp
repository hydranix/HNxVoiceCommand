#include "dialog2.hpp"
#include "ui_dialog2.h"

Dialog2::Dialog2(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::Dialog2)
{
  ui->setupUi(this);
}

Dialog2::~Dialog2()
{
  delete ui;
}

void Dialog2::on_pushButton_browse_clicked()
{
  // open file dialog
}

void Dialog2::on_pushButton_ok_clicked()
{
  QVector<QString> qvNew;
  qvNew.append(ui->lineEdit_phrase->text());
  ui->lineEdit_phrase->setText("");
  qvNew.append(ui->lineEdit_prog->text());
  ui->lineEdit_prog->setText("");
  qvNew.append(ui->lineEdit_param->text());
  ui->lineEdit_param->setText("");
  emit AddPhraseData(qvNew);
  this->accept();
}

void Dialog2::on_pushButton_cancel_clicked()
{
  ui->lineEdit_phrase->setText("");
  ui->lineEdit_prog->setText("");
  ui->lineEdit_param->setText("");
  ui->pushButton_ok->setDisabled(true);
  this->reject();
}

void Dialog2::on_lineEdit_phrase_textEdited(const QString &arg1)
{
    if(!arg1.isEmpty())
    {
      ui->pushButton_ok->setDisabled(false);
    }
}

void Dialog2::on_lineEdit_prog_textEdited(const QString &arg1)
{
    if(!arg1.isEmpty())
    {
      ui->pushButton_ok->setDisabled(false);
    }
}
