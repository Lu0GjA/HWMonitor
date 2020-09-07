#include "HWMonitorUI.h"
#include "monitorThread.h"


extern HWMON_INFO hi;


HWMonitorUI::HWMonitorUI(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this); //This must be put at start of this function
    thTimer = new TimerThread();
    sysTrayIcon = new QSystemTrayIcon(this);
    sysTrayMenu = new QMenu(this);
    sysTrayQuit = new QAction(QString("Exit"), sysTrayMenu);
    sysTrayTopMost = new QAction(QString("Set always top"), sysTrayMenu);
    sysTrayNoTopMost = new QAction(QString("Set not always top"), sysTrayMenu);

    sysTrayMenu->addAction(sysTrayTopMost);
    sysTrayMenu->addAction(sysTrayNoTopMost);
    sysTrayMenu->addSeparator();
    sysTrayMenu->addAction(sysTrayQuit);
    sysTrayIcon->setContextMenu(sysTrayMenu);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    mousePressed = false;
    setMovableArea(this->rect());
    ui.statusContainer->setStyleSheet("QListWidget{background-color: rgba(255, 0, 0, 50);}");
    ui.statusContainer->setFrameShape(QListWidget::NoFrame);
    
    ui.startButton->setStyleSheet("QPushButton{background-color: rgba(0, 255, 0, 100); color: rgb(255, 0,0); border: none}");
    ui.startButton->setText(QString("Start"));

    ui.stopButton->setStyleSheet("QPushButton{background-color: rgba(0, 255, 0, 100); color: rgb(255, 0,0); border: none}");
    ui.stopButton->setText(QString("Stop"));

    ui.fillLabel->setStyleSheet("QLabel{background-color: rgba(0, 255, 0, 100); color: rgb(255, 0, 0)}");
    ui.fillLabel->setText(QString("DragHere"));
    ui.fillLabel->setAlignment(Qt::AlignCenter);

    sysTrayIcon->setIcon(QIcon(":/HWMonitorUI/bitbug_favicon.ico"));
    sysTrayIcon->show();

    connect(ui.startButton, SIGNAL(clicked()), this,SLOT(startRefresh()));
    connect(ui.stopButton, SIGNAL(clicked()), this,SLOT(stopRefresh()));
    connect(thTimer, SIGNAL(startRefresh()), this, SLOT(refresh()));
    connect(sysTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(sysTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(sysTrayQuit, SIGNAL(triggered()), this, SLOT(quitApplication()));
    connect(sysTrayTopMost, SIGNAL(triggered()), this, SLOT(setTopmost()));
    connect(sysTrayNoTopMost, SIGNAL(triggered()), this, SLOT(setNoTopmost()));

    initRefresh();
}


HWMonitorUI::~HWMonitorUI()
{
    delete thTimer;
    delete sysTrayQuit;
    delete sysTrayTopMost;
    delete sysTrayNoTopMost;
    delete sysTrayMenu;
    delete sysTrayIcon;
}


void HWMonitorUI::setMovableArea(const QRect rect)
{
    if (movableArea != rect)
    {
        movableArea = rect;
    }
}


void HWMonitorUI::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousePressPos = event->pos();
        mousePressed = movableArea.contains(mousePressPos);
        setMovableArea(this->rect());
    }
}


void HWMonitorUI::mouseMoveEvent(QMouseEvent* event)
{
    if (mousePressed)
    {
        move(pos() + event->pos() - mousePressPos);
    }
}


void HWMonitorUI::mouseReleaseEvent(QMouseEvent* event)
{
    mousePressed = false;
}


void HWMonitorUI::initRefresh(void)
{
    char infoBuffer[MAX_INFO_BUFFER];
    QListWidgetItem* newItem = NULL;

    //OS Information
    //item0
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "OS Major Version: %d", hi.osviex.dwMajorVersion);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item1
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Build Number: %d", hi.osviex.dwBuildNumber);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item2
    newItem = new QListWidgetItem();
    newItem->setText(QString(""));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //Memory Information
    //item3
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "MEM Usage: %d%%", hi.msex.dwMemoryLoad);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item4
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Phys MEM: %I64d MB", hi.msex.ullTotalPhys / 1024 / 1024);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item5
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Free Phys MEM: %I64d MB", hi.msex.ullAvailPhys / 1024 / 1024);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item6
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Paging file: %I64d MB", hi.msex.ullTotalPageFile / 1024 / 1024);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item7
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Free Paging file: %I64d MB", hi.msex.ullAvailPageFile / 1024 / 1024);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item8
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Virt MEM: %I64d MB", hi.msex.ullTotalVirtual / 1024  /1024);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item9
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Free Virt MEM: %I64d MB", hi.msex.ullAvailVirtual / 1024 / 1024);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //item10
    newItem = new QListWidgetItem();
    newItem->setText(QString(""));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    //CPU Information
    HWMON_GetCpuInfo(&hi);

    //item11
    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "CPU Usage: %.0f%%", hi.dbIdleTime);
    newItem = new QListWidgetItem();
    newItem->setText(QString(infoBuffer));
    newItem->setTextColor(QColor(0, 255, 0));
    ui.statusContainer->addItem(newItem);

    for (unsigned int i = 0; i < hi.si.dwNumberOfProcessors; i++)
    {
        //item12 + k
        ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
        snprintf(infoBuffer, MAX_INFO_BUFFER, "CPU[%d] Usage: %.0f%%", i, hi.puts[i].dbIdleTime);
        newItem = new QListWidgetItem();
        newItem->setText(QString(infoBuffer));
        newItem->setTextColor(QColor(0, 255, 0));
        ui.statusContainer->addItem(newItem);
    }

}


void HWMonitorUI::refresh(void)
{
    char infoBuffer[MAX_INFO_BUFFER];
    HWMON_GetMemInfo(&hi);
    HWMON_GetCpuInfo(&hi);

    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "MEM Usage: %d%%", hi.msex.dwMemoryLoad);
    ui.statusContainer->item(3)->setText(QString(infoBuffer));

    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Free Phys MEM: %I64d MB", hi.msex.ullAvailPhys / 1024 / 1024);
    ui.statusContainer->item(5)->setText(QString(infoBuffer));

    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Free Paging file: %I64d MB", hi.msex.ullAvailPageFile / 1024 / 1024);
    ui.statusContainer->item(7)->setText(QString(infoBuffer));

    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "Total Free Virt MEM: %I64d MB", hi.msex.ullAvailVirtual / 1024 / 1024);
    ui.statusContainer->item(9)->setText(QString(infoBuffer));

    ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
    snprintf(infoBuffer, MAX_INFO_BUFFER, "CPU Usage: %.0f%%", hi.dbIdleTime);
    ui.statusContainer->item(11)->setText(QString(infoBuffer));

    for (unsigned int i = 0; i < hi.si.dwNumberOfProcessors; i++)
    {
        //item12 + k
        ZeroMemory(infoBuffer, MAX_INFO_BUFFER);
        snprintf(infoBuffer, MAX_INFO_BUFFER, "CPU[%d] Usage: %.0f%%", i, hi.puts[i].dbIdleTime);
        ui.statusContainer->item(12 + i)->setText(QString(infoBuffer));
    }
}


void HWMonitorUI::startRefresh(void)
{
    thTimer->start();
}


void HWMonitorUI::stopRefresh(void)
{
    thTimer->setTimerDisabled();
}


void HWMonitorUI::sysTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    //case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (isHidden())
        {
            show();
        }
        else
        {
            activateWindow();
        }
        break;
    default:
        break;
    }
}


void HWMonitorUI::setTopmost(void)
{
    Qt::WindowFlags oldFlags = windowFlags();
    setWindowFlags(oldFlags | Qt::WindowStaysOnTopHint);
    show();
}


void HWMonitorUI::setNoTopmost(void)
{
    Qt::WindowFlags oldFlags = windowFlags();
    setWindowFlags(oldFlags & (~Qt::WindowStaysOnTopHint));
    show();
}


void HWMonitorUI::quitApplication(void)
{
    thTimer->setTimerDisabled();
    thTimer->wait();
    qApp->quit();
}
