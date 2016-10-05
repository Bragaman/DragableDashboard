#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include <QDebug>
DashboardWidget::DashboardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    newIndex = -1;
    oldIndex = -1;
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}


void DashboardWidget::addWidget(DashboardItem *item)
{
    auto name = item->getName();
    if (!mapItems.contains(name)) {
        mapItems.insert(name , item);
        int pos = item->getCurPos();
        if (ui->verticalLayout->isEmpty())
            ui->verticalLayout->insertWidget(0, item);
        else
            for (int i=0; i < ui->verticalLayout->count(); ++i)
                if (pos <= qobject_cast<DashboardItem* >(ui->verticalLayout->itemAt(i)->widget())->getCurPos()) {
                    ui->verticalLayout->insertWidget(i, item);
                    break;
                }
    }
}



bool DashboardWidget::moveWidget(const QPoint& newPos, QWidget* widget)
{
    findPos(newPos);
    if (newIndex == -1)
        newIndex = oldIndex;

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
            //            qDebug() << "Y pos: " <<  tmp << "; Y2 =  " << y2 << "; index = " << newIndex;
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
    } else {
        ui->verticalLayout->insertWidget(newIndex, currentDragedWidget);
        ui->verticalLayout->update();
        currentDragedWidget->show();
    }

}

void DashboardWidget::dropEvent(QDropEvent *event)
{
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
