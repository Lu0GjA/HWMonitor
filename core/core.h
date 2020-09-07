#ifndef CORE_H
#define CORE_H

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef struct _PROCESSORS_USE_TIME
{
	double dbOldIdleTime;
	double dbOldCurrentTime;
	double dbIdleTime;
	double dbCurrentTime;
	float fUse;
} PROCESSORS_USE_TIME;


typedef struct _HWMON_INFO
{
	OSVERSIONINFOEX osviex;
	SYSTEM_INFO si;
	MEMORYSTATUSEX msex;
	LARGE_INTEGER liOldIdleTime;
	LARGE_INTEGER liOldSystemTime;
	PROCESSORS_USE_TIME* puts;
	double dbIdleTime;

} HWMON_INFO, *PHWMON_INFO;

#ifndef HWMONAPI
#define HWMONAPI __declspec(dllimport)
#endif // !HWMONAPI

VOID HWMONAPI HWMON_INIT(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_AllocCpus(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_FreeCpus(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_GetOsInfo(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_GetCpuInfo(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_GetMemInfo(PHWMON_INFO phi);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !CORE_H

