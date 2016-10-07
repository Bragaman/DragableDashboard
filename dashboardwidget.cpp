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
    for (int i=0; i <  ui->verticalLayout->count(); ++i) {
        if (auto obj = qobject_cast<DashboardItem* >(ui->verticalLayout->itemAt(i)->widget()))
            obj->setCurPosY(i);
    }

    delete ui;
}


void DashboardWidget::addWidget(DashboardItem *item)
{
    auto name = item->getName();
    auto lay = ui->verticalLayout;

    if (!mapItems.contains(name)) {
        int count = lay->count();
        mapItems.insert(name , item);
        int pos = item->getCurPosY();

        for (int i=0; i < count; ++i) {
            if (auto obj = qobject_cast<DashboardItem* >(lay->itemAt(i)->widget())) {
                int widgPos = obj->getCurPosY();
                if (pos <= widgPos) {
                    lay->insertWidget(i, item);
                    return;
                }
            }
        }
        lay->insertWidget(count - 1, item);
    }
}



bool DashboardWidget::moveWidget(const QPoint& newPos, QWidget* widget)
{
    findPos(newPos);
    if (newIndex == -1)
        newIndex = oldIndex;

    auto lay = currentLayout;

    lay->insertWidget(newIndex, widget);
    lay->update();
    return true;
}

void DashboardWidget::findPos(const QPoint &newPos)
{
    updateCurLayout(newPos.x());
    auto lay = currentLayout;

    int rowCount = lay->count();
    int y =0;

    int newY = newPos.y();
    for (int row = 0; row < rowCount; ++row) {
        auto item = lay->itemAt(row);
        auto widget = item->widget();
        if (widget) {
            QRect rect = item->widget()->rect();
            int x1=0,  x2=0, y1=0, y2=0;
            rect.getCoords(&x1,&y1,&x2,&y2);
            int tmp = newY-y;
            if (tmp-1 < y2) {
                int l = (y2 - y1)/4;
                if (widget == currentDragedWidget)
                    return ;
                newIndex = -1;
                if (tmp > l*3)
                    newIndex = row +1;
                if (tmp <l)
                    newIndex = row;
                return;
            }

            y+=y2;
        }
    }

    newIndex = 0;
}

void DashboardWidget::updateCurLayout(int x)
{
    if (x > width()/2) {
        currentLayout = ui->verticalLayout;
    } else
        currentLayout = ui->verticalLayout;
}

void DashboardWidget::showDropPos(const QPoint &newPos)
{
    auto lay = currentLayout;

    findPos(newPos);
    if (newIndex == -1) {
        hideItem(currentDragedWidget);
    } else {
        lay->insertWidget(newIndex, currentDragedWidget);
        lay->update();
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
    auto lay = currentLayout;

    widget->hide();
    lay->removeWidget(widget);
    lay->update();
}

void DashboardWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("dashboard/item")) {
        auto mimeData = event->mimeData();
        QString nameWidget(mimeData->data("dashboard/item"));
        currentDragedWidget = mapItems.value(nameWidget);
        oldIndex = ui->verticalLayout->indexOf(currentDragedWidget);
        oldLayout = ui->verticalLayout;
        if (oldIndex == -1) {
            oldIndex = ui->verticalLayout->indexOf(currentDragedWidget);
            oldLayout = ui->verticalLayout;
        }
        currentLayout = oldLayout;
        hideItem(currentDragedWidget);


        event->acceptProposedAction();
    }
}
