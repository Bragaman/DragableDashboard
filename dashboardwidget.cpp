#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include <QDebug>
DashboardWidget::DashboardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QFrame* line = new QFrame();
    line->setGeometry(QRect(/* ... */));
    line->setFrameShape(QFrame::HLine); // Replace by VLine for vertical line
    line->setFrameShadow(QFrame::Sunken);
    helperWideget = line;
    newIndex = -1;
    oldIndex = -1;
    initDashboard();
}

DashboardWidget::~DashboardWidget()
{
    delete helperWideget;
    delete ui;
}

void DashboardWidget::initDashboard()
{
    DashboardItem* item = new DashboardItem(this);
    item->setName("test 1");
    ui->verticalLayout->addWidget(item);
    DashboardItem* item2 = new DashboardItem(this);
    item2->setName("AGAAAAAAAARh");
    ui->verticalLayout->addWidget(item2);
    mapItems.insert(item2->getName(), item2);
    ui->widget->setName("test dfghjk");
    mapItems.insert(item->getName(), item);
    mapItems.insert(ui->widget->getName(), ui->widget);
}



bool DashboardWidget::moveWidget(const QPoint& newPos, QWidget* widget)
{
    findPos(newPos);
    if (newIndex == -1)
        newIndex = oldIndex;
//        return  false;

//    ui->verticalLayout->removeWidget(widget);
    ui->verticalLayout->insertWidget(newIndex, widget);
    ui->verticalLayout->update();
    return true;
}

void DashboardWidget::findPos(const QPoint &newPos)
{
    int rowCount = ui->verticalLayout->count();
    int y =0;
    int newY = newPos.y();
    for (int row = 0; row < rowCount; ++row) {
        auto item = ui->verticalLayout->itemAt(row);

        QRect rect = item->widget()->rect();
        int x1=0,  x2=0, y1=0, y2=0;
        rect.getCoords(&x1,&y1,&x2,&y2);
        int tmp = newY-y;
        if (tmp-1 < y2) {
            int l = (y2 - y1)/4;
            if (item->widget() == currentDragedWidget)
                return ;
            newIndex = -1;
            if (tmp > l*3)
                newIndex = row +1;
            if (tmp <l)
                newIndex = row;

            qDebug() << "Y pos: " <<  tmp << "; Y2 =  " << y2 << "; index = " << newIndex;

            break;
        }

        y+=y2;
    }
}

void DashboardWidget::showDropPos(const QPoint &newPos)
{
    findPos(newPos);
    if (newIndex == -1) {
        hideItem(currentDragedWidget);
//        ui->verticalLayout->removeWidget(helperWideget);
    } else {
        ui->verticalLayout->insertWidget(newIndex, currentDragedWidget);
        ui->verticalLayout->update();
        currentDragedWidget->show();
    }
//        ui->verticalLayout->insertWidget(newIndex, helperWideget);
}

void DashboardWidget::dropEvent(QDropEvent *event)
{
    ui->verticalLayout->removeWidget(helperWideget);
    auto mimeData = event->mimeData();


    QString nameWidget(mimeData->data("dashboard/item"));

    if (!nameWidget.isEmpty()) {
        auto widget = mapItems.value(nameWidget);
        auto newPos = event->pos();
        moveWidget(newPos, widget);
        widget->show();
    }
}

void DashboardWidget::dragMoveEvent(QDragMoveEvent *event)
{
    auto newPos = event->pos();
    showDropPos(newPos);
}

void DashboardWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    qDebug() << "drag leave event";
    ui->verticalLayout->removeWidget(helperWideget);

    QWidget::dragLeaveEvent(event);
}

void DashboardWidget::hideItem(QWidget* widget)
{
    widget->hide();
    ui->verticalLayout->removeWidget(widget);
    ui->verticalLayout->update();
}

void DashboardWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("dashboard/item")) {
        auto mimeData = event->mimeData();
        QString nameWidget(mimeData->data("dashboard/item"));
        currentDragedWidget = mapItems.value(nameWidget);
        oldIndex = ui->verticalLayout->indexOf(currentDragedWidget);
        hideItem(currentDragedWidget);


        event->acceptProposedAction();
    }
}
