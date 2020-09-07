#pragma once

#include <QtWidgets/QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QRect>
#include <QSystemTrayIcon>
#include <QMenu>
#include "ui_HWMonitorUI.h"
#include "..\core\core.h"

#define MAX_INFO_BUFFER 128

class TimerThread;

class HWMonitorUI : public QWidget
{
    Q_OBJECT

public:
    HWMonitorUI(QWidget *parent = Q_NULLPTR);
    ~HWMonitorUI();

private:
    Ui::HWMonitorUIClass ui;
    TimerThread* thTimer;
    bool mousePressed;
    QRect movableArea;
    QPoint mousePressPos;
    QSystemTrayIcon* sysTrayIcon;
    QMenu* sysTrayMenu;
    QAction* sysTrayQuit;
    QAction* sysTrayTopMost;
    QAction* sysTrayNoTopMost;

    void initRefresh(void);
    void setMovableArea(const QRect rect);

private slots:
    void refresh(void);
    void startRefresh(void);
    void stopRefresh(void);
    void sysTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setTopmost(void);
    void setNoTopmost(void);
    void quitApplication(void);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

};
