#pragma once
#ifndef DIALOG2_HPP

#define Q_OBJECT Q_OBJECT

#include <QDialog>
#include <QVector>

namespace Ui {
  class Dialog2;
}

class Dialog2 : public QDialog
{
  Q_OBJECT

public:
  explicit Dialog2(QWidget *parent = 0);
  ~Dialog2();

private slots:
  void on_pushButton_browse_clicked();

  void on_pushButton_ok_clicked();

  void on_pushButton_cancel_clicked();

  void on_lineEdit_phrase_textEdited(const QString &arg1);

  void on_lineEdit_prog_textEdited(const QString &arg1);

signals:
  void AddPhraseData(QVector<QString> qvNew);

private:
  Ui::Dialog2 *ui;
};

#define DIALOG2_HPP
#endif // DIALOG2_HPP
