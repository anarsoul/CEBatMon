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
#include "log.h"

static int uptime;

#define LINE_HEIGHT 20

static void printText(HDC hdc, int x, int y, RECT *rect, const char *str)
{
	RECT dstRect;
    wchar_t buf[1024];
    int len = mbstowcs(buf, str, ARRAY_SIZE(buf));

	dstRect.left = x;
	dstRect.top = y;
	dstRect.bottom = y + LINE_HEIGHT;
	dstRect.right = rect->right;

	DrawText(hdc, buf, len, &dstRect, DT_LEFT);
}

static int voltage, current, temperature, acStatus, percent;

static void doLogLine()
{
	SYSTEM_POWER_STATUS_EX2 stat2;
	memset(&stat2, 0, sizeof(stat2));
	int ret = GetSystemPowerStatusEx2(&stat2, sizeof(stat2), false);
	if (!ret) {
		doLog("; can't get power status!\n");
	}
	else {
		voltage = stat2.BatteryVoltage;
		current = stat2.BatteryCurrent;
		temperature = stat2.BatteryTemperature;
		acStatus = stat2.ACLineStatus;
		percent = stat2.BatteryLifePercent;
		doLog("; %d: %d%%, %s\n", uptime,
			percent,
			acStatus ? "AC Online" : "AC Offline");
		doLog("; U    |  I    |  T\n");
		doLog("%7.d,%7.d,%7.d\n",
				voltage,
				current,
				temperature);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:
			{
				char outBuf[128];
				PAINTSTRUCT ps;
				HDC hdc;
				RECT rect;
				int y = 10;

				GetClientRect(hWnd, &rect);

				hdc = BeginPaint(hWnd, &ps);

				FillRect(hdc, &rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

				SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
				SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));

				printText(hdc, 10, y, &rect, "CE Battery Monitor");
				y += LINE_HEIGHT;

				snprintf(outBuf, sizeof(outBuf), "Uptime: %d secs", uptime);
				printText(hdc, 10, y, &rect, outBuf);
				y += LINE_HEIGHT;
				snprintf(outBuf, sizeof(outBuf), "AC Status: %s", acStatus ? "Online" : "Offline");
				printText(hdc, 10, y, &rect, outBuf);
				y += LINE_HEIGHT;
				snprintf(outBuf, sizeof(outBuf), "Battery level: %d%%", percent);
				printText(hdc, 10, y, &rect, outBuf);
				y += LINE_HEIGHT;
				snprintf(outBuf, sizeof(outBuf), "Voltage: %d mV", voltage);
				printText(hdc, 10, y, &rect, outBuf);
				y += LINE_HEIGHT;
				snprintf(outBuf, sizeof(outBuf), "Current: %d mA", current);
				printText(hdc, 10, y, &rect, outBuf);
				y += LINE_HEIGHT;
				snprintf(outBuf, sizeof(outBuf), "Temperature: %d * 0.1C", temperature);
				printText(hdc, 10, y, &rect, outBuf);
				y += LINE_HEIGHT;

				EndPaint(hWnd, &ps);
			}
			break;
		case WM_TIMER:
			{
				uptime++;
				if ((uptime % 30) == 0) {
					doLogLine();
				}
				RECT rect;
				GetClientRect(hWnd, &rect);
				InvalidateRect(hWnd, &rect, false);
			}
			break;
		case WM_DESTROY:
			KillTimer(hWnd, 1);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

static int createWindow(HINSTANCE hInst)
{
	HWND hWnd;
	WNDCLASS wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW ;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= 0;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= L"CEBatMon";

	RegisterClass(&wc);


	hWnd = CreateWindow(L"CEBatMon", L"CE battery monitor", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, 0, NULL);

	if (!hWnd)
	{
		return -1;
	}

	SetTimer(hWnd, 1, 1000, NULL);

	ShowWindow(hWnd, 1);
	UpdateWindow(hWnd);

	return 0;
}

static void eventLoop(void)
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPTSTR lpCmdLine, int nCmdShow)
{
	char buf[1024];

	memset(buf, 0, sizeof(buf));

	openLogFile(hInstance, "CEBatMon.txt");
	doLogLine();
	if (createWindow(hInstance) == 0) {
		eventLoop();
		UnregisterClass(L"CEBatMon", hInstance);
	}
	else {
		doLog("Failed to create window!\n");
	}
	closeLogFile();
	
	return 0;

/*
	SYSTEM_POWER_STATUS_EX2 stat2;
	memset(&stat2, 0, sizeof(stat2));
	int ret = GetSystemPowerStatusEx2(&stat2, sizeof(stat2), false);
	if (!ret) {
		doLog("GetSystemPowerStatusEx2!\n");
	}
	else {
		doLog("U: %d, I: %d, T: %d, Iavg: %d, AC status: %d\n",
			(int)stat2.BatteryVoltage,
			(int)stat2.BatteryCurrent,
			(int)stat2.BatteryTemperature,
			(int)stat2.BatteryAverageCurrent,
			(int)stat2.ACLineStatus);
	}
*/
}
