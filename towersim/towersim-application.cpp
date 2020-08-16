
#include <QApplication>
 
#include "towersim-mainwindow.h"
 
int main (int argc, char *argv[])
{
   QApplication app(argc, argv);
 
   TowerSimMainWindow* MainWindow = new TowerSimMainWindow;

   MainWindow->show();
   return app.exec();
}

