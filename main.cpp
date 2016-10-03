#include "dashboardwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DashboardWidget w;
    w.show();


    return a.exec();
}


