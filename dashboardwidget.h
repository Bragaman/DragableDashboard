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

    bool moveWidget(const QPoint &newPos, QWidget *widget);
    void showDropPos(const QPoint &newPos);

    bool findPos(const QPoint &newPos);
    void fixPos();

    int findIndex(QLayout *lay, const QPoint &pos);


    void insertWidget(QVBoxLayout *vLay, int yPos, int xPos, QWidget *widget);

    QMap<QString, QWidget*> mapItems;
    int newIndexY;
    int newIndexX;
    int oldIndexY;
    int oldIndexX;
    QWidget* currentDragedWidget;
    QVBoxLayout *currentLayout;
    QVBoxLayout *oldLayout;


    void hideItem(QWidget *widget);

    void moveDown(int row);
    
    void moveUp(int row);

    void moveLeft(int row, int column);

    void moveRight(int column, int row);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
};

#endif // WIDGET_H
