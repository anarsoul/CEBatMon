/*  CEBatMon - Battery monitor for WinCE with logging capability
    Copyright (C) 2010 Vasily Khoruzhick (anarsoul@gmail.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <windows.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"

static HANDLE logFile;
static char sourcePath[200];

static void preparePath(HINSTANCE hInst)
{
    // Locate the directory containing the haret executable.
    wchar_t sp[200];
    GetModuleFileName(hInst, sp, ARRAY_SIZE(sp));
    int len = wcstombs(sourcePath, sp, sizeof(sourcePath));
    char *x = sourcePath + len;
    while ((x > sourcePath) && (x[-1] != '\\'))
        x--;
    *x = 0;
}

void openLogFile(HINSTANCE hInstance, const char *fn)
{
	if (logFile) {
		CloseHandle(logFile);
	}
	preparePath(hInstance);
	char full_fn[200];
	
	strncpy(full_fn, sourcePath, sizeof(full_fn));
	strncat(full_fn, fn, sizeof(full_fn));

	wchar_t wfn[200];
	mbstowcs(wfn, full_fn, ARRAY_SIZE(wfn));
	logFile = CreateFile(wfn, GENERIC_WRITE, FILE_SHARE_READ, 
						0, OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL /*| FILE_FLAG_WRITE_THROUGH*/,
						0);
}

static char logCache[32768];
static unsigned logCacheSize;

void closeLogFile()
{
	if (logFile) {
		/* Flushing cache */
		DWORD nw;
		WriteFile(logFile, logCache, logCacheSize, &nw, 0);
		logCacheSize = 0;
		CloseHandle(logFile);
	}
	logFile = NULL;
}

void writeLog(const char *msg, uint32_t len)
{
	unsigned cached;
	if (!logFile) {
		return;
	}

	while (len) {
		cached = MIN(len, sizeof(logCache) - logCacheSize);
		memcpy(logCache + logCacheSize, msg, cached);
		logCacheSize += cached;
		msg += cached;
		len -= cached;

		if (logCacheSize == sizeof(logCache)) {
			/* Flushing cache... */
			DWORD nw;
			WriteFile(logFile, logCache, logCacheSize, &nw, 0);
			logCacheSize = 0;
		}
	}
}

void doLog(const char *fmt, ...)
{
	char str[2048];
	va_list arg;

	va_start(arg, fmt);
	vsnprintf(str, sizeof(str), fmt, arg);
	va_end(arg);

	writeLog(str, strlen(str));
}
