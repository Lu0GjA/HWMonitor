#include "monitorThread.h"


TimerThread::TimerThread()
{
    bTimerStarted = false;
}


TimerThread::~TimerThread()
{

}


void TimerThread::setTimerDisabled(void)
{
    bTimerStarted = false;
}


void TimerThread::run(void)
{
    bTimerStarted = true;

    while (bTimerStarted)
    {
        emit startRefresh();
        sleep(1);
    }
}