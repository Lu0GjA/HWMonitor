#define _CRT_SECURE_NO_WARNINGS

#define HWMONAPI __declspec(dllexport)

#include "core.h"

#pragma comment(lib, "mi.lib")

#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3
#define SystemProcessorPerformanceInformation 8

#define MAX_ERROR_STRING_BUFFER_LEN 128
#define LARGE_INTEGER_TO_DOUBLE(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))


typedef struct _SYSTEM_BASIC_INFORMATION
{
	DWORD dwUnknown1;
	ULONG uKeMaximumIncrement;
	ULONG uPageSize;
	ULONG uMmNumberOfPhysicalPages;
	ULONG uMmLowestPhysicalPage;
	ULONG uMmHighestPhysicalPage;
	ULONG uAllocationGranularity;
	PVOID pLowestUserAddress;
	PVOID pMmHighestUserAddress;
	ULONG uKeActiveProcessors;
	BYTE bKeNumberProcessors;
	BYTE bUnknown2;
	WORD wUnknown3;
} SYSTEM_BASIC_INFORMATION;


typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER liIdleTime;
	DWORD dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;


typedef struct _SYSTEM_TIME_INFORMATION
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG uCurrentTimeZoneId;
	DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;


typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
	LARGE_INTEGER liIdleTime;
	LARGE_INTEGER liKernelTime;
	LARGE_INTEGER liUserTime;
	LARGE_INTEGER liDpcTime;
	LARGE_INTEGER liInterruptTime;
	ULONG uReserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;


typedef NTSTATUS (NTAPI *MyNtQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);
typedef NTSTATUS (NTAPI* MyRtlGetVersion)(PRTL_OSVERSIONINFOEXW);


BOOL WINAPI DllMain(
	HINSTANCE hInstDll,
	DWORD dwReason,
	LPVOID lpReserved
)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}

	return TRUE;
}


VOID HWMONAPI HWMON_INIT(PHWMON_INFO phi)
{
	ZeroMemory(&(phi->osviex), sizeof(OSVERSIONINFOEX));
	ZeroMemory(&(phi->si), sizeof(SYSTEM_INFO));
	ZeroMemory(&(phi->msex), sizeof(MEMORYSTATUSEX));
	ZeroMemory(&(phi->pli.liOldIdleTime), sizeof(LARGE_INTEGER));
	ZeroMemory(&(phi->pli.liOldSystemTime), sizeof(LARGE_INTEGER));
	phi->pli.puts = NULL;
	phi->pli.dbIdleTime = 0;
	ZeroMemory(&(phi->mictx.miApplication), sizeof(MI_Application));
	ZeroMemory(&(phi->mictx.miSession), sizeof(MI_Session));
	phi->mictx.numberOfMiOps = 0;
	ZeroMemory(&(phi->mictx.miOps), sizeof(MI_Operation) * HWMON_MAX_MI_OPERATION_NUMBER);
}


DWORD HWMONAPI HWMON_INIT_MIAPP(PHWMON_INFO phi)
{
	MI_Result miResult = MI_RESULT_OK;
	char errMessage[MAX_ERROR_STRING_BUFFER_LEN] = { 0 };

	miResult = MI_Application_Initialize(
		0,
		NULL,
		NULL,
		&phi->mictx.miApplication
	);

	if (miResult != MI_RESULT_OK)
	{
		snprintf(
			errMessage,
			MAX_ERROR_STRING_BUFFER_LEN,
			"MI application initialize failed, result: %ld",
			miResult
		);
		MessageBoxA(
			NULL,
			errMessage,
			NULL,
			MB_OK | MB_ICONERROR
		);
		return 1;
	}

	return 0;
}


DWORD HWMONAPI HWMON_CLOSE_MIAPP(PHWMON_INFO phi)
{
	MI_Result miResult = MI_RESULT_OK;
	char errMessage[MAX_ERROR_STRING_BUFFER_LEN] = { 0 };

	miResult = MI_Session_Close(&phi->mictx.miSession, NULL, NULL);

	if (miResult != MI_RESULT_OK)
	{
		snprintf(
			errMessage,
			MAX_ERROR_STRING_BUFFER_LEN,
			"MI session close failed, result: %ld",
			miResult
		);
		MessageBoxA(
			NULL,
			errMessage,
			NULL,
			MB_OK | MB_ICONERROR
		);
		return 1;
	}

	miResult = MI_Application_Close(&phi->mictx.miApplication);

	if (miResult != MI_RESULT_OK)
	{
		snprintf(
			errMessage,
			MAX_ERROR_STRING_BUFFER_LEN,
			"MI application close failed, result: %ld",
			miResult
		);
		MessageBoxA(
			NULL,
			errMessage,
			NULL,
			MB_OK | MB_ICONERROR
		);
		return 1;
	}

	return 0;
}


DWORD HWMONAPI HWMON_CreateMISession(PHWMON_INFO phi)
{
	MI_Result miResult = MI_RESULT_OK;
	char errMessage[MAX_ERROR_STRING_BUFFER_LEN] = { 0 };

	miResult = MI_Application_NewSession(
		&phi->mictx.miApplication,
		L"WMIDCOM",
		NULL,
		NULL,
		NULL,
		NULL,
		&phi->mictx.miSession
	);

	if (miResult != MI_RESULT_OK)
	{
		snprintf(
			errMessage,
			MAX_ERROR_STRING_BUFFER_LEN,
			"MI creating session failed, result: %ld",
			miResult
		);
		MessageBoxA(
			NULL,
			errMessage,
			NULL,
			MB_OK | MB_ICONERROR
		);
		return 1;
	}

	return 0;
}


static BOOL _stdcall GetVersionEx2(LPOSVERSIONINFOEX lpOsviex)
{
	HMODULE hDll = GetModuleHandle(L"ntdll.dll");
	MyRtlGetVersion pRtlGetVersion = NULL;

	if (hDll)
	{
		pRtlGetVersion = (MyRtlGetVersion)GetProcAddress(hDll, "RtlGetVersion");
	}

	if (pRtlGetVersion)
	{
		return pRtlGetVersion((PRTL_OSVERSIONINFOEXW)lpOsviex) >= 0;
	}

	return FALSE;
}


DWORD HWMONAPI HWMON_AllocCpus(PHWMON_INFO phi)
{
	DWORD dwStatus = ERROR_SUCCESS;
	DWORD dwNumCpus = phi->si.dwNumberOfProcessors;

	phi->pli.puts = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwNumCpus * sizeof(PROCESSORS_USE_TIME));
	if (phi->pli.puts == NULL)
	{
		return -1;
	}

	return dwStatus;
}


DWORD HWMONAPI HWMON_FreeCpus(PHWMON_INFO phi)
{
	DWORD dwStatus = ERROR_SUCCESS;

	if (phi->pli.puts != NULL)
	{
		dwStatus = HeapFree(GetProcessHeap(), 0, phi->pli.puts);
		if (dwStatus == 0)
		{
			return GetLastError();
		}

		phi->pli.puts = NULL;
	}

	return dwStatus;
}


DWORD HWMONAPI HWMON_GetOsInfo(PHWMON_INFO phi)
{
	DWORD dwStatus = ERROR_SUCCESS;

	phi->osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx2(&(phi->osviex));
	GetSystemInfo(&(phi->si));


	return dwStatus;
}


DWORD HWMONAPI HWMON_GetCpuInfo(PHWMON_INFO phi)
{
	DWORD dwStatus = ERROR_SUCCESS;
	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* sppis = NULL;
	SYSTEM_PERFORMANCE_INFORMATION spi = { 0 };
	SYSTEM_TIME_INFORMATION sti = { 0 };
	SYSTEM_BASIC_INFORMATION sbi = { 0 };
	MyNtQuerySystemInformation pNtQuerySystemInformation = NULL;

	double dbIdleTime = 0;
	double dbSystemTime = 0;
	double dbAllIdleTime = 0;

	HMODULE hDll = GetModuleHandle(L"ntdll.dll");
	if (!hDll)
	{
		return GetLastError();
	}

	pNtQuerySystemInformation = (MyNtQuerySystemInformation)GetProcAddress(hDll, "NtQuerySystemInformation");
	if (!pNtQuerySystemInformation)
	{
		return GetLastError();
	}

	// New system time
	dwStatus = pNtQuerySystemInformation(
		SystemTimeInformation,
		&sti,
		sizeof(SYSTEM_TIME_INFORMATION),
		NULL
	);
	if (dwStatus != NO_ERROR)
	{
		return dwStatus;
	}

	// New CPU's idle time
	dwStatus = pNtQuerySystemInformation(
		SystemPerformanceInformation,
		&spi,
		sizeof(SYSTEM_PERFORMANCE_INFORMATION),
		NULL
	);
	if (dwStatus != NO_ERROR)
	{
		return dwStatus;
	}

	sppis = HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		phi->si.dwNumberOfProcessors * sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)
	);
	if (sppis == NULL)
	{
		return -1;
	}

	// Per processor time
	dwStatus = pNtQuerySystemInformation(
		SystemProcessorPerformanceInformation,
		sppis,
		sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * phi->si.dwNumberOfProcessors,
		NULL
	);
	if (dwStatus != NO_ERROR)
	{
		return dwStatus;
	}

	// If it's the first call - skip
	if (phi->pli.liOldIdleTime.QuadPart != 0)
	{
		// CurrentValue = NewValue - OldValue
		dbIdleTime = LARGE_INTEGER_TO_DOUBLE(spi.liIdleTime) - LARGE_INTEGER_TO_DOUBLE(phi->pli.liOldIdleTime);
		dbSystemTime = LARGE_INTEGER_TO_DOUBLE(sti.liKeSystemTime) - LARGE_INTEGER_TO_DOUBLE(phi->pli.liOldSystemTime);

		// CurrentCpuIdle = IdleTime / SystemTime
		dbIdleTime = dbIdleTime / dbSystemTime;

		// CurrentCpuUsage% = 100.0 - (CurrentCpuIdle * 100.0) / NumberOfProcessors
		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)phi->si.dwNumberOfProcessors + 0.5;
		phi->pli.dbIdleTime = dbIdleTime;

		// Calc processors
		for (unsigned int i = 0; i < phi->si.dwNumberOfProcessors; i++)
		{
			phi->pli.puts[i].dbCurrentTime = LARGE_INTEGER_TO_DOUBLE(sppis[i].liKernelTime) + LARGE_INTEGER_TO_DOUBLE(sppis[i].liUserTime) +
				LARGE_INTEGER_TO_DOUBLE(sppis[i].liDpcTime) + LARGE_INTEGER_TO_DOUBLE(sppis[i].liInterruptTime) - phi->pli.puts[i].dbOldCurrentTime;
			phi->pli.puts[i].dbIdleTime = LARGE_INTEGER_TO_DOUBLE(sppis[i].liIdleTime) - phi->pli.puts[i].dbOldIdleTime;

			// CurrentCpuIdle = IdleTime / SystemTime
			phi->pli.puts[i].dbIdleTime = phi->pli.puts[i].dbIdleTime / phi->pli.puts[i].dbCurrentTime;

			// CurrentCpuUsage% = 100.0 - (CurrentCpuIdle * 100.0) / NumberOfProcessors
			phi->pli.puts[i].dbIdleTime = 100.0 - phi->pli.puts[i].dbIdleTime * 100.0 + 0.5;
		}
	}

	// Store new CPU's idle and system time
	phi->pli.liOldIdleTime = spi.liIdleTime;
	phi->pli.liOldSystemTime = sti.liKeSystemTime;

	for (unsigned int i = 0; i < phi->si.dwNumberOfProcessors; i++)
	{
		phi->pli.puts[i].dbOldCurrentTime = LARGE_INTEGER_TO_DOUBLE(sppis[i].liKernelTime) + LARGE_INTEGER_TO_DOUBLE(sppis[i].liUserTime) +
			LARGE_INTEGER_TO_DOUBLE(sppis[i].liDpcTime) + LARGE_INTEGER_TO_DOUBLE(sppis[i].liInterruptTime);
		phi->pli.puts[i].dbOldIdleTime = LARGE_INTEGER_TO_DOUBLE(sppis[i].liIdleTime);
	}

	dwStatus = HeapFree(GetProcessHeap(), 0, sppis);
	if (dwStatus == 0)
	{
		return GetLastError();
	}
	
	dwStatus = ERROR_SUCCESS;
	return dwStatus;
}


DWORD HWMONAPI HWMON_GetMemInfo(PHWMON_INFO phi)
{
	DWORD dwStatus = ERROR_SUCCESS;

	phi->msex.dwLength = sizeof(MEMORYSTATUSEX);
	if (!GlobalMemoryStatusEx(&(phi->msex)))
	{
		return GetLastError();
	}

	return dwStatus;
}
