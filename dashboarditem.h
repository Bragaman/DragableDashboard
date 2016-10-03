#ifndef DASHBOARDITEM_H
#define DASHBOARDITEM_H

#include <QWidget>
#include <QMouseEvent>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QPoint>
#include <QDebug>
namespace Ui {
class DashboardItem;
}

class DashboardItem : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardItem(QWidget *parent = 0);
    ~DashboardItem();


    QString getName() const;
    void setName(const QString &value);

private:
    Ui::DashboardItem *ui;
    QString name;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DASHBOARDITEM_H
