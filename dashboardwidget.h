#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDropEvent>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayoutItem>
#include <dashboarditem.h>
namespace Ui {
class Widget;
}

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = 0);
    ~DashboardWidget();

    void addWidget(DashboardItem *item);

private:
    Ui::Widget *ui;

    void setWidgetOnLayout(QWidget *widget, int index = 0);

    bool moveWidget(const QPoint &newPos, QWidget *widget);
    void showDropPos(const QPoint &newPos);
    void findPos(const QPoint &newPos);
    void updateCurLayout(int x);

    QMap<QString, QWidget*> mapItems;
    int newIndex;
    int oldIndex;
    QWidget* currentDragedWidget;
    QVBoxLayout *currentLayout;
    QVBoxLayout *oldLayout;


    void hideItem(QWidget *widget);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
};

#endif // WIDGET_H
