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
                    obj->setCurPosX(j);
//                    qDebug() << obj->getName() << "pos y =  "<< i <<" pos x = "<< j;
                }
        }
    }

    delete ui;
}


void DashboardWidget::addWidget(DashboardItem *item)
{
    auto name = item->getName();
    auto vLay = ui->verticalLayout;

    if (!mapItems.contains(name)) {
        int vCount = vLay->count();
        mapItems.insert(name , item);
        int posY = item->getCurPosY();
        int posX = item->getCurPosX();
//        qDebug() << name << "pos y =  "<< posY <<" pos x = "<< posX;
        int widgPosY = 0;
        for (int i=0; i < vCount; ++i) {
            if (auto hLay = dynamic_cast<QHBoxLayout*>(vLay->itemAt(i))) {
                int hCount = hLay->count();
                if (hCount != 0) {
                    for (int j =0; j < hCount; ++j ) {
                        if (auto obj = qobject_cast<DashboardItem* >(hLay->itemAt(j)->widget())) {
                            widgPosY = obj->getCurPosY();
                            int widgPosX = obj->getCurPosX();
                            if (posY <= widgPosY)
                                if (posX <= widgPosX) {
                                    insertWidget(vLay, i, j, item);
                                    return;
                                }
                        }
                    }
                    if (posY <= widgPosY) {
                            insertWidget(vLay, i,  hCount, item);
                            return;
                        }
                }
            }
        }
        insertWidget(vLay, vCount -1, -1, item);
    }
}

void DashboardWidget::moveDown(int row)
{
    newIndexY = row + 1;
    newIndexX = -1;
}

void DashboardWidget::moveUp(int row)
{
    newIndexY = row - 1;
    if (newIndexY == -1)
        newIndexY = 0;
    newIndexX = -1;
}

void DashboardWidget::moveLeft(int row, int column)
{
    newIndexY = row;
    newIndexX = column -1;
    if (newIndexX == -1)
        newIndexX = 0;
}

void DashboardWidget::moveRight(int column, int row)
{
    newIndexY = row;
    newIndexX = column + 1;
}

bool DashboardWidget::findPos(const QPoint &newPos)
{
    auto vLay = currentLayout;
    int border = 20;
    int nY = newPos.y();
    int nX = newPos.x();

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


    newIndexY = row;
    newIndexX = column;

    if (nX < x + border) {
        if (nY > y + border and nY < y + h - border) {
            moveLeft(row, column);
            return true;
        }
        return false;
    }

    if (nX > x + w - border) {
        if (nY > y + border and nY < y + h - border) {
            moveRight(column, row);
            return true;
        }
        return false;
    }

    auto hLayRect = hLay->geometry();
    int xL, yL, wL,hL;
    hLayRect.getRect(&xL, &yL, &wL, &hL);
    int borderL = hL/5;
    if (nY > yL + hL - borderL) {
        moveDown(row);
        return true;
    }

    if (nY < yL + borderL) {
        moveUp(row);
        return true;
    }
    return false;
}

void DashboardWidget::fixPos()
{
    if (oldIndexY == newIndexY) {
        int tmp = oldIndexX - newIndexX;
        if (tmp < 0 and tmp < -1)
            newIndexX--;

        if (tmp > 0 and tmp != 1)
            newIndexX++;
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
        hLay->removeWidget(widget);
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


bool DashboardWidget::moveWidget(const QPoint& newPos, QWidget* widget)
{
    if (findPos(newPos)) {
        if (newIndexY == -1)
            newIndexY = 0;

        qDebug()  << "drop pos: " << newIndexY << " : " << newIndexX;
        auto lay = currentLayout;

        insertWidget(lay, newIndexY, newIndexX, widget);
        return true;
    }
    return false;
}

void DashboardWidget::showDropPos(const QPoint &newPos)
{
    auto lay = currentLayout;
    if (findPos(newPos) ){
        fixPos();
        if (newIndexY == -1) {
            newIndexY = 0;
            insertWidget(lay, oldIndexY, newIndexX, currentDragedWidget);
            currentDragedWidget->show();
        } else {
            insertWidget(lay, newIndexY, newIndexX, currentDragedWidget);
            currentDragedWidget->show();
        }
        oldIndexX = newIndexX;
        oldIndexY = newIndexY;
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


        event->acceptProposedAction();
    }
}
