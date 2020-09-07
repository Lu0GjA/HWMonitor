#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include "..\core\core.h"

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
	return 0;
}