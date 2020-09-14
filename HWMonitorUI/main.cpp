#include "HWMonitorUI.h"
#include <QtWidgets/QApplication>


HWMON_INFO hi;


int main(int argc, char *argv[])
{
    int retVal = 0;
    HWMON_INIT(&hi);
    
    if (HWMON_INIT_MIAPP(&hi) != 0)
    {
        return -1;
    }

    if (HWMON_CreateMISession(&hi) != 0)
    {
        return -1;
    }

    HWMON_GetOsInfo(&hi);
    HWMON_GetMemInfo(&hi);
    HWMON_AllocCpus(&hi);

    QApplication a(argc, argv);
    HWMonitorUI w(Q_NULLPTR);
    w.show();
    retVal = a.exec();
    HWMON_FreeCpus(&hi);

    if (HWMON_CLOSE_MIAPP(&hi) != 0)
    {
        return -1;
    }

    return retVal;
}
