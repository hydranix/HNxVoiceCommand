#pragma once
#include "Recog.hpp"
#include "dialog2.hpp"
#include <QDialog>
#include <QTableWidget>
#include <QSystemTrayIcon>
#include <QSharedMemory>
#include <QTimer>
#include <QSettings>

namespace Ui {
  class Dialog;
}

class Dialog : public QDialog
{
  Q_OBJECT

public:
  explicit Dialog(QWidget* parent = nullptr);
  ~Dialog();

  // Adds strings to recognition engine
  void AddString(QString phrase,
                 QString prog,
                 QString param = "");

  void LoadSettings();

private slots:
  void on_pushButton_add_clicked();

  void on_pushButton_remove_clicked();

  void on_pushButton_hide_clicked();

  void on_pushButton_exit_clicked();

  void on_tableWidget_itemClicked(QTableWidgetItem* item);

  //void on_tableWidget_itemChanged(QTableWidgetItem* item);

public slots:
  void updateTable(QVector<QString> qvNew);

  void unhide();

private: // vars
  Ui::Dialog* ui{nullptr};

  HNx::Recog* recog{nullptr};

  Dialog2* dialog2{nullptr};

  QStringList horzHeadLabels{};

  QSystemTrayIcon* trayicon{nullptr};

  QSettings* settings{nullptr};

private: // funcs
  void createTrayIcon();

};

