#include "dashboardwidget.h"
#include <QApplication>
#include <QTableWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DashboardWidget w;
    w.show();

    auto table1 = new QTableWidget();
    w.addWidget(new DashboardItem(table1, "Test  1", nullptr));
    w.addWidget(new DashboardItem(new QTableWidget(), "Test  2", nullptr));


    return a.exec();
}


