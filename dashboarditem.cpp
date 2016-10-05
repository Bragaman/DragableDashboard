#include "dashboarditem.h"
#include "ui_dashboarditem.h"

#include <QPainter>

DashboardItem::DashboardItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardItem)
{
    ui->setupUi(this);
    setStyleSheet("DashboardItem "
                  "{background: #2F2F2F;"
                  "}"
                  "DashboardItem[isDraged=true] "
                  "{background: red;}");
    curPos = 0;
    curHeight = 150;
    state = State::NORMAL;
    setPropertyIsDraged(false);

}

DashboardItem::DashboardItem(QWidget *central, const QString &name, QSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardItem)

{
    ui->setupUi(this);
    setName(name);
    setCentralWidget(central);
    setPropertyIsDraged(false);

    if (settings != nullptr) {
        //TODO get settings from settings file
    } else {
        setStyleSheet("DashboardItem "
                      "{background: #2F2F2F;"
                      "}"
                      "DashboardItem[isDraged=true] "
                      "{background: red;}");
        curPos = 0;
        curHeight = 150;
        state = State::NORMAL;
    }
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

void DashboardItem::setCentralWidget(QWidget *widget)
{
    ui->itemLayout->addWidget(widget, 0,0,1,1);
}

void DashboardItem::setPropertyIsDraged(bool is)
{
    setProperty("isDraged", is);
    style()->unpolish(this);
    style()->polish(this);
}

DashboardItem::State DashboardItem::getState() const
{
    return state;
}

void DashboardItem::setState(const DashboardItem::State &value)
{
    state = value;
}

int DashboardItem::getCurHeight() const
{
    return curHeight;
}

void DashboardItem::setCurHeight(int value)
{
    curHeight = value;
}

int DashboardItem::getCurPos() const
{
    return curPos;
}

void DashboardItem::setCurPos(int value)
{
    curPos = value;
}

void DashboardItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
            && ui->widgetPanel->geometry().contains(event->pos())) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        QPixmap* pixmap =new QPixmap(size());
        render(pixmap);
        setPropertyIsDraged(true);
        mimeData->setData("dashboard/item", name.toUtf8() );
        drag->setMimeData(mimeData);
        drag->setPixmap(*pixmap);
        Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
        setPropertyIsDraged(false);
    }
}

void DashboardItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}


void DashboardItem::on_push_arrow_clicked(bool checked)
{
    ui->widget_hideable->setVisible(!checked);
}
