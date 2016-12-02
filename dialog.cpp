#include "dialog.hpp"
#include "ipcsm.hpp"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::Dialog)
  , recog(new Recog) // takes a second or two
  , ipcsm(nullptr)
  , dialog2(new Dialog2(this))
  , trayicon(new QSystemTrayIcon(this))
  , shmem_timer(new QTimer(this))
  , settings(new QSettings())
{
  ui->setupUi(this);
  trayicon->hide();
  shared_memory.setKey("HNxVC");
  shared_memory.create(sizeof(IPCSM));
  shared_memory.lock();
  ipcsm = reinterpret_cast<IPCSM*>(shared_memory.data());
  ipcsm->flag = false;
  shared_memory.unlock();
  recog->initialize();
  connect(dialog2, SIGNAL(AddPhraseData(QVector<QString>)), this, SLOT(updateTable(QVector<QString>)));
  connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(unhide()));
  connect(shmem_timer, SIGNAL(timeout()), this, SLOT(check_shmem()));
  shmem_timer->start(500);
}

Dialog::~Dialog()
{
  this->hide();
  if (shmem_timer)
  {
    shmem_timer->stop();
    delete shmem_timer;
  }
  
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
  dialog2->show();
  dialog2->exec();
}

void Dialog::on_pushButton_remove_clicked()
{
  auto qlItems = ui->tableWidget->selectedItems();
  if(qlItems.isEmpty())
  {
    return;
  }
  std::string selecteditemtext = qlItems[0]->text().toStdString();
  if(qlItems[0]->column() == 0)
  {
    recog->remove_by_phrase(selecteditemtext);
  }
  else
  {
    recog->remove_by_command(selecteditemtext);
  }
  settings->remove(ui->tableWidget->item(qlItems[0]->row(), 0)->text());
  this->blockSignals(true);
  ui->tableWidget->removeRow(qlItems[0]->row());
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
  QString qsCmdline = qvNew[1];
  if(!qvNew[2].isEmpty())
  {
    qsCmdline += " " + qvNew[2];
  }

  // check if phrase already exists
  for(int i=0;i<ui->tableWidget->rowCount();++i)
  {
    if(ui->tableWidget->item(i, 0)->text() == qsPhrase)
    { // update existing
      recog->remove_by_phrase(qsPhrase.toStdString());
      recog->AddString(qsPhrase.toStdString(), qsCmdline.toStdString());
      ui->tableWidget->item(i, 1)->text() = qsCmdline;

      settings->setValue(qsPhrase, qsCmdline);

      this->blockSignals(false);
      return;
    }
  }

  // add to the recognition engine
  std::string phrase = qvNew[0].toStdString();
  std::string prog   = qvNew[1].toStdString();
  std::string param  = qvNew[2].toStdString();
  recog->AddString(phrase, prog, param);

  // update gui with new phrase
  ui->tableWidget->insertRow(0);
  ui->tableWidget->setItem(0,0,new QTableWidgetItem(qsPhrase));
  ui->tableWidget->setItem(0,1,new QTableWidgetItem(qsCmdline));

  // save to registry

  settings->setValue(qsPhrase, qsCmdline);

  this->blockSignals(false);
}

void Dialog::on_tableWidget_itemClicked(QTableWidgetItem* item)
{
  ui->pushButton_remove->setDisabled(false);
}

void Dialog::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
  QString newtext = item->text();
  QString relatedtext;
  if(item->column() == 0)
  {// phrase changed
    if(ui->tableWidget->item(item->row(), 1))
    {
      relatedtext = ui->tableWidget->item(item->row(), 1)->text();
      recog->remove_by_command(relatedtext.toStdString());
      recog->AddString(newtext.toStdString(), relatedtext.toStdString());
      auto keylist = settings->allKeys();

      for(int i=0;i<keylist.size();++i)
      {
        if(settings->value(keylist[i]).toString() == relatedtext)
        {
          settings->remove(keylist[i]);
          settings->setValue(newtext, relatedtext);

          break;
        }
      }

    }
  }
  else
  {// command changed
    if(ui->tableWidget->item(item->row(), 0))
    {
      relatedtext = ui->tableWidget->item(item->row(), 0)->text();
      recog->remove_by_phrase(relatedtext.toStdString());
      recog->AddString(relatedtext.toStdString(), newtext.toStdString());

      settings->setValue(relatedtext, newtext);

    }
  }
}

void Dialog::unhide()
{
  trayicon->hide();
  this->show();
}

void Dialog::check_shmem()
{
  shared_memory.lock();
  if(ipcsm->flag)
  {
    ipcsm->flag = false;
    unhide();
  }
  shared_memory.unlock();
}

void Dialog::LoadSettings()
{
  this->blockSignals(true);

  auto keylist = settings->allKeys();
  for(int i=0;i<keylist.size();++i)
  {
    QString phrase = keylist[i];
    QString cmdline = settings->value(keylist[i]).toString();
    ui->tableWidget->insertRow(0);
    ui->tableWidget->setItem(0,0,new QTableWidgetItem(phrase));
    ui->tableWidget->setItem(0,1,new QTableWidgetItem(cmdline));
    recog->AddString(phrase.toStdString(), cmdline.toStdString());
  }

  this->blockSignals(false);
}
