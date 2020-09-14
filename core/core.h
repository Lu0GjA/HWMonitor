#ifndef CORE_H
#define CORE_H

#include <Windows.h>
#include <mi.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define HWMON_MAX_MI_OPERATION_NUMBER 16 

typedef struct _MI_CTX
{
	MI_Application miApplication;
	MI_Session miSession;
	int numberOfMiOps;
	MI_Operation miOps[HWMON_MAX_MI_OPERATION_NUMBER];
} MI_CTX;


typedef struct _PROCESSORS_USE_TIME
{
	double dbOldIdleTime;
	double dbOldCurrentTime;
	double dbIdleTime;
	double dbCurrentTime;
	float fUse;
} PROCESSORS_USE_TIME;


typedef struct _PROCESSOR_LOAD_INFO
{
	LARGE_INTEGER liOldIdleTime;
	LARGE_INTEGER liOldSystemTime;
	PROCESSORS_USE_TIME* puts;
	double dbIdleTime;
} PROCESSOR_LOAD_INFO;


typedef struct _HWMON_INFO
{
	OSVERSIONINFOEX osviex;
	SYSTEM_INFO si;
	MEMORYSTATUSEX msex;
	PROCESSOR_LOAD_INFO pli;
	MI_CTX mictx;

} HWMON_INFO, *PHWMON_INFO;

#ifndef HWMONAPI
#define HWMONAPI __declspec(dllimport)
#endif // !HWMONAPI

VOID HWMONAPI HWMON_INIT(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_INIT_MIAPP(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_CLOSE_MIAPP(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_CreateMISession(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_AllocCpus(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_FreeCpus(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_GetOsInfo(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_GetCpuInfo(PHWMON_INFO phi);
DWORD HWMONAPI HWMON_GetMemInfo(PHWMON_INFO phi);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !CORE_H

