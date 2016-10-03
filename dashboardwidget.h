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

    void initDashboard();

private:
    Ui::Widget *ui;
    QMap<QString, QWidget*> mapItems;
    // QWidget interface
    bool moveWidget(const QPoint &newPos, QWidget *widget);
    void showDropPos(const QPoint &newPos);
    // QWidget interface
    void findPos(const QPoint &newPos);


    int newIndex;
    int oldIndex;
    QWidget* currentDragedWidget;
    QWidget* helperWideget;

    void addItemToList(DashboardItem* item2);

    // QWidget interface
    void hideItem(QWidget *widget);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
};

#endif // WIDGET_H
