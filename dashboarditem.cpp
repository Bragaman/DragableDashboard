#include "dashboarditem.h"
#include "ui_dashboarditem.h"

#include <QPainter>

DashboardItem::DashboardItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardItem)
{
    ui->setupUi(this);
}

DashboardItem::~DashboardItem()
{
    delete ui;
}

QString DashboardItem::getName() const
{
    return name;
}

void DashboardItem::setName(const QString &value)
{
    name = value;
    ui->moduleNameLabel->setText(name);
}

void DashboardItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
            && ui->widgetPanel->geometry().contains(event->pos())) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        QPixmap* pixmap =new QPixmap(size());
        render(pixmap);
        mimeData->setData("dashboard/item", name.toUtf8() );
        drag->setMimeData(mimeData);
        drag->setPixmap(*pixmap);
        Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
    }
}

