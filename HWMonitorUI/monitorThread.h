#ifndef MONTHREAD_H
#define MONTHREAD_H

#include <QThread>

class TimerThread : public QThread
{
	Q_OBJECT
public:
	TimerThread();
	~TimerThread();
	void setTimerDisabled(void);

private:
	bool bTimerStarted;

protected:
	void run(void);

signals:
	void startRefresh(void);
};

#endif