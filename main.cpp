#include "dashboardwidget.h"
#include <QApplication>
#include <QTableWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DashboardWidget w;
    w.show();
    QSettings *settings = new QSettings("settings.ini", QSettings::NativeFormat);
    auto table1 = new QTableWidget();
    w.addWidget(new DashboardItem(table1, "Test1", "Name 1", settings));
    w.addWidget(new DashboardItem(new QTableWidget(), "Test2", "Name 2", settings));
    w.addWidget(new DashboardItem(new QTableWidget(), "Test3", "Name 3", settings));
    w.addWidget(new DashboardItem(new QTableWidget(), "Test4", "Name 4", settings));
    settings->sync();

    return a.exec();
}


