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

void closeLogFile()
{
	if (logFile) {
		CloseHandle(logFile);
	}
	logFile = NULL;
}

void writeLog(const char *msg, uint32_t len)
{                                       
	if (!logFile) {
		return;
	}
	DWORD nw;
	WriteFile(logFile, msg, len, &nw, 0);
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
