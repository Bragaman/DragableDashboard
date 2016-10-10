#include "dashboarditem.h"
#include "ui_dashboarditem.h"

#include <QPainter>

const QString DashboardItem::DASHBOARD_GROUP      = "dashboard/";
const QString DashboardItem::MODULE_POS_Y         = "module_%1_pos_y";
const QString DashboardItem::MODULE_POS_X         = "module_%1_pos_x";
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
    curPosY = 0;
    curPosX = 0;
    setCurHeight(155);
    state = State::NORMAL;
    resizedNow = false;
    setPropertyIsDraged(false);
    setSettings(nullptr);
}

DashboardItem::DashboardItem(QWidget *central, const QString &name, const QString &trName, QSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardItem)

{
    setStyleSheet("DashboardItem "
                  "{background: #2F2F2F;"
                  "}"
                  "DashboardItem[isDraged=true] "
                  "{border: 3px solid red;}");
    ui->setupUi(this);
    setLabelText(trName);
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
        settings->setValue(DASHBOARD_GROUP +MODULE_POS_Y.arg(name), curPosY);
        settings->setValue(DASHBOARD_GROUP +MODULE_POS_X.arg(name), curPosX);
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
        ui->resizerWidget->setVisible(true);
        setVisible(true);
        break;
    case State::CLOSED:
        setFixedHeight(44);
        ui->widget_hideable->setVisible(false);
        ui->push_arrow->setChecked(true);
        ui->resizerWidget->setVisible(false);
        setVisible(true);
    case State::HIDDEN:
        setVisible(false);
    default:
        break;
    }
    ui->push_arrow->blockSignals(false);
    emit changeState(state);
}

int DashboardItem::getCurHeight() const
{
    return curHeight;
}

void DashboardItem::setCurHeight(int value)
{
    if (value > 42)
        if (state != State::CLOSED) {
            curHeight = value;
            setFixedHeight(curHeight);
        }

}

int DashboardItem::getCurPosY() const
{
    return curPosY;
}

void DashboardItem::setCurPosY(int value)
{
    curPosY = value;
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
        curPosY = settings->value(DASHBOARD_GROUP +MODULE_POS_Y.arg(name), 0).toInt();
        curPosX = settings->value(DASHBOARD_GROUP +MODULE_POS_X.arg(name), 0).toInt();
        setState(State(settings->value(DASHBOARD_GROUP +MODULE_STATE.arg(name), State::NORMAL).toInt()));
    } else {
        setStyleSheet("DashboardItem "
                      "{background: #2F2F2F;"
                      "}"
                      "DashboardItem[isDraged=true] "
                      "{background: red;}");
        curPosY = 0;
        curPosX = 0;
        setCurHeight(200);
        setState(State::NORMAL);
    }
}

int DashboardItem::getCurPosX() const
{
    return curPosX;
}

void DashboardItem::setCurPosX(int value)
{
    curPosX = value;
}

QString DashboardItem::getLabelText() const
{
    return labelText;
}

void DashboardItem::setLabelText(const QString &value)
{
    labelText = value;
    ui->moduleNameLabel->setText(labelText);

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

        if (ui->resizerWidget->geometry().contains(point)) {
            resizedNow = true;
            oldPoint = point;
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
    if (resizedNow) {
        setCurHeight(curHeight + event->pos().y() - oldPoint.y());
        oldPoint = event->pos();
    } else
        QWidget::mouseMoveEvent(event);
}

void DashboardItem::mouseReleaseEvent(QMouseEvent *event)
{
    resizedNow = false;
    QWidget::mouseReleaseEvent(event);
}


void DashboardItem::on_push_arrow_clicked(bool checked)
{
    if (checked) {
        setState(State::CLOSED);
    } else
        setState(State::NORMAL);
}

void DashboardItem::on_pushButton_clicked()
{
    setState(State::HIDDEN);
}
