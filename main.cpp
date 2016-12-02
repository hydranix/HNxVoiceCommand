#include "dialog.hpp"
#include "ipcsm.hpp"
#include <QApplication>
int main(int argc, char *argv[])
{
  QCoreApplication::setOrganizationName("HNx");
  QCoreApplication::setApplicationName("Voice Commands");
  QCoreApplication::setApplicationVersion("0.1");

  QApplication a(argc, argv);
  {
    IPCSM * ipcsm = nullptr;
    QSharedMemory shared_memory("HNxVC");
    if (shared_memory.attach())
    {
      shared_memory.lock();
      ipcsm = reinterpret_cast<IPCSM*>(shared_memory.data());
      ipcsm->flag = true;
      shared_memory.unlock();
      shared_memory.detach();
      return 0;
    }
  }
  Dialog w;
  w.LoadSettings();
  w.show();
  return a.exec();
}
