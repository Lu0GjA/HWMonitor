#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include "..\core\core.h"
#include <mi.h>
#include <locale.h>
#pragma comment(lib, "mi.lib")

HWMON_INFO hi;

int main(int argc, char** argv)
{
	HWMON_INIT(&hi);
	HWMON_GetOsInfo(&hi);
	HWMON_GetMemInfo(&hi);

	printf(
		"OSVersionInfoSize %d\n"
		"MajorVersion %d\n"
		"MinorVersion %d\n"
		"BuildNumber %d\n"
		"PlatformId %d\n"
		"CSDVersion %ws\n"
		"ServicePackMajor %d\n"
		"ServicePackMinor %d\n"
		"SuiteMask %d\n"
		"ProductType %d\n"
		"Reserved %d\n",
		hi.osviex.dwOSVersionInfoSize,
		hi.osviex.dwMajorVersion,
		hi.osviex.dwMinorVersion,
		hi.osviex.dwBuildNumber,
		hi.osviex.dwPlatformId,
		hi.osviex.szCSDVersion,
		hi.osviex.wServicePackMajor,
		hi.osviex.wServicePackMinor,
		hi.osviex.wSuiteMask,
		hi.osviex.wProductType,
		hi.osviex.wReserved
	);

	printf("\n\n");

	printf(
		"OEMId / ProcessorArchitecture %d\n"
		"PageSize %d\n"
		"MinAppAddress %p\n"
		"MaxAppAddress %p\n"
		"ActiveProcessorMask %lld\n"
		"NumberOfProcessors %d\n"
		"ProcessorType %d\n"
		"AllocationGranularity %d\n"
		"ProcessorLevel %d\n"
		"ProcessorRevision %d\n",
		hi.si.dwOemId,
		hi.si.dwPageSize,
		hi.si.lpMinimumApplicationAddress,
		hi.si.lpMaximumApplicationAddress,
		hi.si.dwActiveProcessorMask,
		hi.si.dwNumberOfProcessors,
		hi.si.dwProcessorType,
		hi.si.dwAllocationGranularity,
		hi.si.wProcessorLevel,
		hi.si.wProcessorRevision
	);

	printf("\n\n");

	printf(
		"Percent of memory in use %ld\n"
		"Total physical memory %I64d KB\n"
		"Total free physical memory %I64d KB\n"
		"Total paging file %I64d KB\n"
		"Total free paging file %I64d KB\n"
		"Total virtual memory %I64d KB\n"
		"Total free virtual memory %I64d KB\n"
		"Extended memory %I64d\n",
		hi.msex.dwMemoryLoad,
		hi.msex.ullTotalPhys / 1024,
		hi.msex.ullAvailPhys / 1024,
		hi.msex.ullTotalPageFile / 1024,
		hi.msex.ullAvailPageFile / 1024,
		hi.msex.ullTotalVirtual / 1024,
		hi.msex.ullAvailVirtual / 1024,
		hi.msex.ullAvailExtendedVirtual / 1024
	);

	printf("\n\n");

	HWMON_AllocCpus(&hi);
/*
	while (TRUE)
	{
		HWMON_GetCpuInfo(&hi);
		printf("%.0f%%\n", hi.dbIdleTime);
		printf("%d\n", hi.si.dwNumberOfProcessors);

		for (unsigned int i = 0; i < hi.si.dwNumberOfProcessors; i++)
		{
			printf("CPU%d %.0f%%\n", i,hi.puts[i].dbIdleTime);
		}

		printf("\n");

		Sleep(1000);
	}
*/

	HWMON_FreeCpus(&hi);

	//MI (Windows Management Infrastructure) Test

	setlocale(LC_ALL, "chs");

	MI_Application miApp = MI_APPLICATION_NULL;
	MI_Result miResult = MI_RESULT_OK;
	MI_Session miSession = MI_SESSION_NULL;
	MI_Operation miOp = MI_OPERATION_NULL;
	
	miResult = MI_Application_Initialize(
		0,
		NULL,
		NULL,
		&miApp);

	if (miResult != MI_RESULT_OK)
	{
		printf("MI_Application_Initialize failed. MI_Result: %ld\n", miResult);
		return -1;
	}

	miResult = MI_Application_NewSession(
		&miApp,
		L"WMIDCOM", //Currently supported protocols are L"WMIDCOM" and L"WINRM"
		NULL,
		NULL,
		NULL,
		NULL,
		&miSession);

	if (miResult != MI_RESULT_OK)
	{
		printf("MI_Application_NewSession failed. MI_Result: %ld\n", miResult);
		return -1;
	}

	MI_Session_EnumerateInstances(
		&miSession,
		0,
		NULL,
		L"root\\cimv2",
		L"Win32_Process",
		MI_FALSE,
		NULL,
		&miOp);

	miResult = MI_RESULT_OK;
	MI_Boolean moreResults;
	const MI_Char* errorString = NULL;
	MI_Uint32 instanceCount = 0;
	MI_Instance* miInstance;
	const MI_Instance* errorDetails = NULL;

	do
	{
		MI_Operation_GetInstance(
			&miOp,
			(const MI_Instance**) &miInstance,
			&moreResults,
			&miResult,
			&errorString,
			&errorDetails);

		if (miResult != MI_RESULT_OK)
		{
			printf("MI_Operation_GetInstance failed. MI_Result: %ld\n", miResult);
			break;
		}

		if (miInstance)
		{
			MI_Value value;
			MI_Type type;
			MI_Uint32 flags;

			miResult = MI_Instance_GetElement(
				miInstance,
				L"Name",
				&value,
				&type,
				&flags,
				NULL);

			if (miResult != MI_RESULT_OK)
			{
				printf("MI_Instance_GetElement failed. MI_Result: %ld\n", miResult);
				break;
			}

			wprintf(L"Process Name: %s\n", value.string);
			instanceCount++;
		}

	} while (miResult == MI_RESULT_OK && moreResults == MI_TRUE);

	if (miResult != MI_RESULT_OK)
	{
		wprintf(L"Operation failed: MI_Result=%ld, errorString=%s\n", miResult, errorString);
	}
	else
	{
		printf("Operation succeeded. Number of instances = %u\n", instanceCount);
	}

	miResult =  MI_Operation_Close(&miOp);

	if (miResult != MI_RESULT_OK)
	{
		printf("MI_Operation_Close failed. MI_Result: %ld\n", miResult);
		return -1;
	}

	miResult =  MI_Session_Close(&miSession, NULL, NULL);

	if (miResult != MI_RESULT_OK)
	{
		printf("MI_Session_Close failed. MI_Result: %ld\n", miResult);
		return -1;
	}

	miResult =  MI_Application_Close(&miApp);

	if (miResult != MI_RESULT_OK)
	{
		printf("MI_Application_Close failed. MI_Result: %ld\n", miResult);
		return -1;
	}

	return 0;
}