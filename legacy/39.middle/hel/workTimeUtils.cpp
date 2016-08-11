#include "stdafx.h"
#include "hel/workTimeUtils.h"
#include <Windows.h>

namespace hel
{
	real getThreadTime()
	{
		FILETIME creationTime;
		FILETIME exitTime;
		FILETIME kernelTime;
		FILETIME userTime;

		GetThreadTimes(
			GetCurrentThread(),		// __in   HANDLE hThread,
			&creationTime,		// __out  LPFILETIME lpCreationTime,
			&exitTime,	 	// __out  LPFILETIME lpExitTime,
			&kernelTime,		// __out  LPFILETIME lpKernelTime,
			&userTime		// __out  LPFILETIME lpUserTime
			);

		LONGLONG sum = 0;
		sum += LONGLONG(kernelTime.dwHighDateTime)<<32;
		sum += LONGLONG(kernelTime.dwLowDateTime);
		sum += LONGLONG(userTime.dwHighDateTime)<<32;
		sum += LONGLONG(userTime.dwLowDateTime);

		return real(sum)/10000000;
	}

}