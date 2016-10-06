#include "dashboarditem.h"
#include "ui_dashboarditem.h"

#include <QPainter>

const QString DashboardItem::DASHBOARD_GROUP      = "dashboard/";
const QString DashboardItem::MODULE_POS           = "module_%1_pos";
const QString DashboardItem::MODULE_HEIGHT        = "module_%1_height";
const QString DashboardItem::MODULE_STATE         = "module_%1_state";

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
    setCurHeight(155);
    state = State::NORMAL;
    resizedNow = false;
    setPropertyIsDraged(false);
    setSettings(nullptr);
}

DashboardItem::DashboardItem(QWidget *central, const QString &name, QSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardItem)

{
    setStyleSheet("DashboardItem "
                  "{background: #2F2F2F;"
                  "}"
                  "DashboardItem[isDraged=true] "
                  "{border: 3px solid red;}");
    ui->setupUi(this);
    setName(name);
    setCentralWidget(central);
    setPropertyIsDraged(false);
    resizedNow = false;
    setSettings(settings);
}

DashboardItem::~DashboardItem()
{
    if (settings) {
        settings->setValue(DASHBOARD_GROUP +MODULE_HEIGHT.arg(name), curHeight);
        settings->setValue(DASHBOARD_GROUP +MODULE_POS.arg(name), curPos);
        settings->setValue(DASHBOARD_GROUP +MODULE_STATE.arg(name), state);
        settings->sync();
    }
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
    ui->itemLayout->addWidget(widget, 0,0,1,3);
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
    ui->push_arrow->blockSignals(true);
    switch (state) {
    case State::NORMAL:
        setFixedHeight(curHeight);
        ui->widget_hideable->setVisible(true);
        ui->push_arrow->setChecked(false);
        break;
    case State::CLOSED:
        setFixedHeight(44);
        ui->widget_hideable->setVisible(false);
        ui->push_arrow->setChecked(true);
    default:
        break;
    }
    ui->push_arrow->blockSignals(false);
}

int DashboardItem::getCurHeight() const
{
    return curHeight;
}

void DashboardItem::setCurHeight(int value)
{
    curHeight = value;
    setFixedHeight(curHeight);
}

int DashboardItem::getCurPos() const
{
    return curPos;
}

void DashboardItem::setCurPos(int value)
{
    curPos = value;
}

QSettings *DashboardItem::getSettings() const
{
    return settings;
}

void DashboardItem::setSettings(QSettings *value)
{
    settings = value;
    if (settings != nullptr) {
        setCurHeight(settings->value(DASHBOARD_GROUP +MODULE_HEIGHT.arg(name), 200).toInt());
        curPos = settings->value(DASHBOARD_GROUP +MODULE_POS.arg(name), 0).toInt();
        setState(State(settings->value(DASHBOARD_GROUP +MODULE_STATE.arg(name), State::NORMAL).toInt()));
    } else {
        setStyleSheet("DashboardItem "
                      "{background: #2F2F2F;"
                      "}"
                      "DashboardItem[isDraged=true] "
                      "{background: red;}");
        curPos = 0;
        setCurHeight(200);
        setState(State::NORMAL);
    }
}

void DashboardItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton ) {
        auto point = event->pos();
        if (ui->widgetPanel->geometry().contains(point)) {
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
}

void DashboardItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void DashboardItem::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}


void DashboardItem::on_push_arrow_clicked(bool checked)
{
    if (checked) {
        setState(State::CLOSED);
    } else
        setState(State::NORMAL);
}
