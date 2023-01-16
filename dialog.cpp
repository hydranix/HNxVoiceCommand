#include "dialog.hpp"
#include "ui_dialog.h"

using namespace HNx;

Dialog::Dialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::Dialog)
  , recog(new Recog)
  , trayicon(new QSystemTrayIcon(this))
  , settings(new QSettings())
{
  ui->setupUi(this);
  trayicon->hide();
  recog->initialize();
  
  connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(unhide()));
}

Dialog::~Dialog()
{
  this->hide();
  
  if(settings)
    delete settings;
  if(trayicon)
    delete trayicon;
  if(dialog2)
    delete dialog2;
  if(recog)
    delete recog;
  if(ui)
    delete ui;
}

void Dialog::on_pushButton_add_clicked()
{
  dialog2 = new Dialog2(this);
  if (dialog2)
  {
    // auto deletes dialog when finished?
    connect(dialog2, SIGNAL(finished(int)),
            dialog2, SLOT(deleteLater()));

    connect(dialog2, SIGNAL(AddPhraseData(QVector<QString>)),
            this, SLOT(updateTable(QVector<QString>)));

    dialog2->show();
    dialog2->exec();
    delete dialog2;
    dialog2 = nullptr;
  }
} 

void Dialog::on_pushButton_remove_clicked()
{
  auto selItemList = ui->tableWidget->selectedItems();
  if(selItemList.isEmpty())
  {
    return;
  }
  std::wstring selItemText = selItemList[0]->text().toStdWString();
  
  if(selItemList[0]->column() == 0)
  {
    recog->removeCommandByPhrase(selItemText);
  }
  else
  {
    recog->removeCommandByPhrase(selItemText);
  }
  settings->remove(ui->tableWidget->item(selItemList[0]->row(), 0)->text());
  this->blockSignals(true);
  ui->tableWidget->removeRow(selItemList[0]->row());
  this->blockSignals(false);
}

void Dialog::on_pushButton_hide_clicked()
{
  this->hide();
  trayicon->show();
}

void Dialog::on_pushButton_exit_clicked()
{
  this->accept();
}

// new voice command added
void Dialog::updateTable(QVector<QString> qvNew)
{
  this->blockSignals(true);
  // create workable data
  QString qsPhrase = qvNew[0];
  QString qsExe = qvNew[1];
  QString qsArgs = qvNew[2];

  // check if phrase already exists
  for(int i=0;i<ui->tableWidget->rowCount();++i)
  {
    if(ui->tableWidget->item(i, 0)->text() == qsPhrase)
    { // update existing
      recog->removeCommandByPhrase(qsPhrase.toStdWString());
      recog->addCommand(qvNew[0].toStdWString(),
                        qvNew[1].toStdWString(),
                        qvNew[2].toStdWString());

      ui->tableWidget->item(i, 1)->text() = qsExe;
      ui->tableWidget->item(i, 2)->text() = qsExe;
      ui->tableWidget->item(i, 3)->text() = qsArgs;


      //settings->setValue(qsPhrase, );

      this->blockSignals(false);
      return;
    }
  }

  // add to the recognition engine
  recog->addCommand(qvNew[0].toStdWString(),
                    qvNew[1].toStdWString(),
                    qvNew[2].toStdWString());

  // update gui with new phrase
  ui->tableWidget->insertRow(0);
  ui->tableWidget->setItem(0,1,new QTableWidgetItem(qsPhrase));
  ui->tableWidget->setItem(0,2,new QTableWidgetItem(qsExe));
  ui->tableWidget->setItem(0,3,new QTableWidgetItem(qsArgs));

  // save to registry
  //settings->setValue(qsPhrase, qsCmdline);

  this->blockSignals(false);
}

void Dialog::on_tableWidget_itemClicked( [[maybe_unused]] QTableWidgetItem* item)
{
  ui->pushButton_remove->setEnabled(true);
}

//void Dialog::on_tableWidget_itemChanged(QTableWidgetItem *item)
//{
//  QString newtext = item->text();
//  QString relatedtext;
//  if(item->column() == 0)
//  {// phrase changed
//    if(ui->tableWidget->item(item->row(), 1))
//    {
//      relatedtext = ui->tableWidget->item(item->row(), 1)->text();
//
//      recog->removeCommandByPhrase(relatedtext.toStdWString());
//
//      recog->AddString(newtext.toStdString(), relatedtext.toStdWString());
//
//
//      //auto keylist = settings->allKeys();
//      //for(int i=0;i<keylist.size();++i)
//      //{
//        //if(settings->value(keylist[i]).toString() == relatedtext)
//        //{
//          //settings->remove(keylist[i]);
//          //settings->setValue(newtext, relatedtext);
//          //break;
//        //}
//      //}
//
//    }
//  }
//  else
//  {// command changed
//    if(ui->tableWidget->item(item->row(), 0))
//    {
//      relatedtext = ui->tableWidget->item(item->row(), 0)->text();
//      recog->remove_by_phrase(relatedtext.toStdString());
//      recog->AddString(relatedtext.toStdString(), newtext.toStdString());
//
//      settings->setValue(relatedtext, newtext);
//
//    }
//  }
//}

void Dialog::unhide()
{
  trayicon->hide();
  this->show();
}

void Dialog::createTrayIcon()
{


}



//void Dialog::LoadSettings()
//{
//  this->blockSignals(true);
//
//  auto keylist = settings->allKeys();
//  for(int i=0;i<keylist.size();++i)
//  {
//    QString phrase = keylist[i];
//    QString cmdline = settings->value(keylist[i]).toString();
//    ui->tableWidget->insertRow(0);
//    ui->tableWidget->setItem(0,0,new QTableWidgetItem(phrase));
//    ui->tableWidget->setItem(0,1,new QTableWidgetItem(cmdline));
//    recog->AddString(phrase.toStdString(), cmdline.toStdString());
//  }
//
//  this->blockSignals(false);
//}
