#ifndef DASHBOARDITEM_H
#define DASHBOARDITEM_H

#include <QWidget>
#include <QMouseEvent>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QPoint>
#include <QDebug>
#include <QSettings>

namespace Ui {
class DashboardItem;
}

class DashboardItem : public QWidget
{
    Q_OBJECT

public:

    static const QString DASHBOARD_GROUP;
    static const QString MODULE_POS;
    static const QString MODULE_HEIGHT;
    static const QString MODULE_STATE;

    enum State {
        NORMAL,
        CLOSED,
        HIDDEN
    };

    explicit DashboardItem(QWidget *parent = 0);
    explicit DashboardItem(QWidget *central, const QString &name, QSettings *settings, QWidget *parent = 0);

    ~DashboardItem();

    void setCentralWidget(QWidget *widget);

    QString getName() const;
    void setName(const QString &value);

    State getState() const;
    void setState(const State &value);

    int getCurHeight() const;
    void setCurHeight(int value);

    int getCurPos() const;
    void setCurPos(int value);

    QSettings *getSettings() const;
    void setSettings(QSettings *value);

private:
    Ui::DashboardItem *ui;

    QString name;
    State state;
    int curHeight;
    int curPos;

    QSettings *settings;

    bool resizedNow;
    QPoint oldPoint;

    void setPropertyIsDraged(bool is);

private slots:
    void on_push_arrow_clicked(bool checked);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // DASHBOARDITEM_H
