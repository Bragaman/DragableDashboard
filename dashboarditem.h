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
    static const QString MODULE_POS_Y;
    static const QString MODULE_POS_X;
    static const QString MODULE_HEIGHT;
    static const QString MODULE_STATE;

    static const int MIN_HEIGHT;
    static const int MAX_HEIGHT;

    enum State {
        NORMAL,
        CLOSED,
        HIDDEN
    };

    explicit DashboardItem(QWidget *parent = 0);
    explicit DashboardItem(QWidget *central, const QString &name, const QString &trName,
                           QSettings *settings, QWidget *parent = 0);

    ~DashboardItem();

    void setCentralWidget(QWidget *widget);

    QString getName() const;
    void setName(const QString &value);

    State getState() const;
    void setState(const State &value);

    int getCurHeight() const;
    void setCurHeight(int value);

    int getCurPosY() const;
    void setCurPosY(int value);

    QSettings *getSettings() const;
    void setSettings(QSettings *value);

    int getCurPosX() const;
    void setCurPosX(int value);

    QString getLabelText() const;
    void setLabelText(const QString &value);

private:
    Ui::DashboardItem *ui;

    QString name;
    QString labelText;
    State state;
    int curHeight;
    int curPosY;
    int curPosX;
    int lastPosY;

    QSettings *settings;

    bool resizedNow;
    QPoint oldPoint;

    void setPropertyIsDraged(bool is);

private slots:
    void on_push_arrow_clicked(bool checked);

    // QWidget interface
    void on_btnClose_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void changeState(State );
    void changeRowHeight(int);
};

#endif // DASHBOARDITEM_H
