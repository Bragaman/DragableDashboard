#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include <QDebug>

DashboardWidget::DashboardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    newIndexY = -1;
    oldIndexY = -1;
}

DashboardWidget::~DashboardWidget()
{
    for (int i=0; i <  ui->verticalLayout->count(); ++i) {
        if (auto hLay = dynamic_cast<QHBoxLayout*>(ui->verticalLayout->itemAt(i))) {
            int count = hLay->count();
            for (int j=0; j < count; ++j)
                if (auto obj = qobject_cast<DashboardItem* >(hLay->itemAt(j)->widget())) {
                    obj->setCurPosY(i);
                    obj->setCurPosX(j-1);
                }
        }
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
        qDebug() << name << ": "<< pos;
        for (int i=0; i < count; ++i) {
            if (auto hLay = dynamic_cast<QHBoxLayout*>(lay->itemAt(i))) {
                if (hLay->count() != 0)
                    if (auto obj = qobject_cast<DashboardItem* >(hLay->itemAt(0)->widget())) {
                        int widgPos = obj->getCurPosY();
                        if (pos <= widgPos) {
                            insertWidget(lay, i, -1, item);
                            return;
                        }
                    }
            }
        }
        insertWidget(lay, count -1, -1, item);
    }
}



bool DashboardWidget::moveWidget(const QPoint& newPos, QWidget* widget)
{
    findPos(newPos);
    if (newIndexY == -1)
        newIndexY = 0;

    auto lay = currentLayout;

    insertWidget(lay, newIndexY, newIndexX, widget);

    return true;
}

bool DashboardWidget::findPos(const QPoint &newPos)
{
    auto vLay = currentLayout;

    int row = findIndex(vLay, newPos);
    if (row == -1)
        return false;
    if (row == vLay->count() - 1)
        return false;
    auto hLay = vLay->itemAt(row)->layout();

    int column = findIndex(hLay, newPos);
    if (column == -1)
        return false;
    auto item = hLay->itemAt(column);
    if (item->widget() == currentDragedWidget)
        return false;

    auto rect = item->geometry();
    int x,y,w,h;
    rect.getRect(&x, &y, &w, &h);
    int nY = newPos.y();
    int nX = newPos.x();

    qDebug() << row << " : " << column;

    newIndexY = row;
    newIndexX = column;

    if (nY > y + h*3/4) {
        if (nX > x + w/4 and nX < x + w*3/4) {
            qDebug() << "down";
            newIndexY = row + 1;
            newIndexX = -1;
            return true;
        }
        return false;
    }

    if (nY < y + h/4) {
        qDebug() << "up";
        if (nX > x + w/4 and nX < x + w*3/4) {
            newIndexY = row - 1;
            newIndexX = -1;
            return true;
        }
        return false;
    }

    if (nX < x + w/4) {
        qDebug() << "left";
        if (nY > y + h/4 and nY < y + h*3/4) {
            newIndexY = row;
            newIndexX = column -1;
            if (newIndexX == -1)
                newIndexX = 0;
            return true;
        }
        return false;
    }

    if (nX > x + w*3/4) {
        qDebug() << "right";
        if (nY > y + h/4 and nY < y + h*3/4) {
            newIndexY = row;
            newIndexX = column + 1;
            return true;
        }
        return false;
    }
}

int DashboardWidget::findIndex(QLayout *lay, const QPoint &pos)
{
    if (lay) {
        int count = lay->count();
        for (int i =0; i < count; ++i) {
            auto item = lay->itemAt(i);
            QRect rect = item->geometry();
            if (rect.contains(pos))
                return i;
        }
    }
    return -1;
}


void DashboardWidget::insertWidget(QVBoxLayout *vLay, int yPos, int xPos, QWidget *widget)
{

    if (xPos == -1) {
        vLay->insertLayout(yPos, new QHBoxLayout());
        xPos = 0;
    }
    if (auto hLay = dynamic_cast<QHBoxLayout*>(vLay->itemAt(yPos))) {
        hLay->insertWidget(xPos, widget);
        hLay->update();
    }

    for (int i=0; i <  vLay->count(); ++i) {
        if (auto hLay = dynamic_cast<QHBoxLayout*>(vLay->itemAt(i))) {
            if (hLay->count() == 0)
                vLay->removeItem(hLay);
        }
    }

    vLay->update();
}

void DashboardWidget::showDropPos(const QPoint &newPos)
{
    auto lay = currentLayout;
    if (findPos(newPos) ){
        if (newIndexY == -1) {
            newIndexY = 0;
            insertWidget(lay, oldIndexY, newIndexX, currentDragedWidget);
            currentDragedWidget->show();
        } else {
            insertWidget(lay, newIndexY, newIndexX, currentDragedWidget);
            currentDragedWidget->show();
        }
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

    for (int i=0; i <  lay->count(); ++i) {
        if (auto hLay = dynamic_cast<QHBoxLayout*>(lay->itemAt(i))) {
            hLay->removeWidget(widget);
            if (hLay->count() == 0)
                lay->removeItem(hLay);
        }
    }

    lay->removeWidget(widget);

    lay->update();
}

void DashboardWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("dashboard/item")) {
        auto mimeData = event->mimeData();
        QString nameWidget(mimeData->data("dashboard/item"));
        currentDragedWidget = mapItems.value(nameWidget);
        currentLayout = ui->verticalLayout;
        auto point = event->pos();
        int tmp = findIndex(currentLayout, point);
        if (tmp != -1) {
            oldIndexY = tmp;
            oldIndexX = findIndex(currentLayout->itemAt(oldIndexY)->layout(), point);
            newIndexY = oldIndexY;
            newIndexX = oldIndexX;
        }
        //        hideItem(currentDragedWidget);


        event->acceptProposedAction();
    }
}
