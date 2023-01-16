// This was the first C++ program I ever wrote
//   I first wrote it in 2016 and came back to
//  it in 2023 to fix my mistakes.
//
#include "dialog.hpp"
#include "SingleInstance.h"

#include <QApplication>

using namespace HNx;

int main(int argc, char* argv[])
{
  // there can be only one
  if (!isSingleInstance())
  {
    return 1;
  }

  // application name and version
  QApplication::setOrganizationName("HNx");
  QApplication::setApplicationName("Voice Command");
  QApplication::setApplicationVersion("0.2");

  // We will live in the tray
  QApplication::setQuitOnLastWindowClosed(false);

  QApplication a(argc, argv);

  Dialog w;
  //w.LoadSettings();
  w.show();
  
  return a.exec();
}