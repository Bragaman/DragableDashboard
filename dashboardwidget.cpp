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
    ui->comboBoxItems->addItem("-------------", "");
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
                }
        }
    }

    delete ui;
}


void DashboardWidget::addWidget(DashboardItem *item)
{
    auto name = item->getName();
    if (!mapItems.contains(name)) {
        connect(item, &DashboardItem::changeState, this, &DashboardWidget::onChangeItemState);
        connect(item, &DashboardItem::changeRowHeight, this,
                [this, item] (int newHeight){
            auto vLay = ui->verticalLayout;
            int row = findIndex(vLay, item->pos());
            auto hLay = qobject_cast<QHBoxLayout *>(vLay->itemAt(row)->layout());
            int count = hLay->count();
            for (int i = 0; i < count; ++i) {
                if (auto item = qobject_cast<DashboardItem*>(hLay->itemAt(i)->widget())) {
                    item->setCurHeight(newHeight);
                }
            }
        });

        initOnLayout(item);
        emit item->changeState(item->getState());
    }
}

void DashboardWidget::onChangeItemState(DashboardItem::State state)
{
    auto item = qobject_cast<DashboardItem*>(sender());
    if (state == DashboardItem::State::HIDDEN) {
        item->setVisible(false);
        ui->comboBoxItems->addItem(item->getLabelText(), item->getName());
    } else {
        item->setVisible(true);
        int count = ui->comboBoxItems->count();
        for (int i =0; i < count; ++i)
            if (ui->comboBoxItems->itemData(i) == item->getName()) {
                ui->comboBoxItems->removeItem(i);
                break;
            }
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
//    int border = 20;
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

    auto rect = hLay->contentsRect();
    int x,y,w,h;
    rect.getRect(&x, &y, &w, &h);

    int border = w/3;

    newIndexY = row;
    newIndexX = column;

    if (nX < x + border) {
//        if (nY > y + border and nY < y + h - border) {
            moveLeft(row, column);
            return true;
//        }
//        return false;
    }

    if (nX > x + w - border) {
//        if (nY > y + border and nY < y + h - border) {
            moveRight(column, row);
            return true;
//        }
//        return false;
    }

    auto hLayRect = hLay->geometry();
    int xL, yL, wL,hL;
    hLayRect.getRect(&xL, &yL, &wL, &hL);
//    int borderL = hL/5;
    int borderL =h/2;
    if (nY > yL + hL - borderL && nX > x + border && nX < x + w - border) {
        moveDown(row);
        return true;
    }

    if (nY < yL + borderL && nX > x + border && nX < x + w - border) {
        moveUp(row);
        return true;
    }
    return false;
}

void DashboardWidget::fixPos()
{
    if (oldIndexY == newIndexY) {
        int tmp = oldIndexX - newIndexX;
        if (tmp < -1)
            newIndexX--;

        if (tmp > 1)
            newIndexX++;
    }
}


int DashboardWidget::findIndex(QLayout *lay, const QPoint &pos)
{
    if (lay) {
        int count = lay->count();
        for (int i = 0; i < count; ++i) {
            auto item = lay->itemAt(i);
            QRect rect = item->geometry();
            if (rect.contains(pos))
                return i;
        }
    }
    return -1;
}

void DashboardWidget::resizeItems(QHBoxLayout *hLay, int xHeight)
{
    int maxSize = xHeight;
    int count = hLay->count();
    for (int i =0; i < count; ++i) {
        if (auto item = qobject_cast<DashboardItem*>(hLay->itemAt(i)->widget())) {
            int h = item->getCurHeight();
            if (h < maxSize)
                item->setCurHeight(maxSize);
            if (h > maxSize ) {
                maxSize = h;
                i = -1;
            }
        }
    }
}

void DashboardWidget::initOnLayout(DashboardItem *item)
{
    auto name = item->getName();
    auto vLay = ui->verticalLayout;
    int vCount = vLay->count();
    mapItems.insert(name , item);
    int posY = item->getCurPosY();
    int posX = item->getCurPosX();
    qDebug() << name << "pos y =  "<< posY <<" pos x = "<< posX;
    int widgPosY = 0;
    for (int i=0; i < vCount; ++i) {
        if (auto hLay = dynamic_cast<QHBoxLayout*>(vLay->itemAt(i))) {
            int hCount = hLay->count();
            if (hCount != 0) {
                for (int j =0; j < hCount; ++j ) {
                    if (auto obj = qobject_cast<DashboardItem* >(hLay->itemAt(j)->widget())) {
                        widgPosY = obj->getCurPosY();
                        int widgPosX = obj->getCurPosX();
                        if (posY == widgPosY)
                            if (posX <= widgPosX) {
                                insertWidget(vLay, i, j, item);
                                return;
                            }
                    }
                }
                if (posY == widgPosY) {
                    insertWidget(vLay, i,  hCount, item);
                    return;
                }
                if (posY < widgPosY) {
                    insertWidget(vLay, i,  -1, item);
                    return;
                }
            }
        }
    }
    insertWidget(vLay, vCount -1, -1, item);
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
        resizeItems(hLay, widget->height());
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
        fixPos();
        if (newIndexY == -1)
            newIndexY = 0;

        auto lay = currentLayout;

        insertWidget(lay, newIndexY, newIndexX, widget);
        return true;
    }
    return false;
}

void DashboardWidget::showDropPos(const QPoint &newPos)
{
    auto lay = currentLayout;
    if (findPos(newPos)){
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

void DashboardWidget::on_comboBoxItems_activated(int index)
{
    auto name = ui->comboBoxItems->itemData(index).toString();
    auto obj = mapItems.value(name, nullptr);
    if (obj)
        obj->setState(DashboardItem::State::NORMAL);
}


