#include "HWMonitorUI.h"
#include <QtWidgets/QApplication>


HWMON_INFO hi;


int main(int argc, char *argv[])
{
    int retVal = 0;
    HWMON_INIT(&hi);
    HWMON_GetOsInfo(&hi);
    HWMON_GetMemInfo(&hi);
    HWMON_AllocCpus(&hi);

    QApplication a(argc, argv);
    HWMonitorUI w;
    w.show();
    retVal = a.exec();
    HWMON_FreeCpus(&hi);

    return retVal;
}
