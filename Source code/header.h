/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	header.h - Header file of the RFID Reader Application, defining menu
--							   IDs, global variables, and functions shared between
--							   classes.
--
--	PROGRAM:        RFID Reader Application
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	NOTES:			Header.h is part of an RFID reader application, that uses the
--					SkeyeTek API to connect to an RFID device, and allows for the
--					reading of RFID tags and printing the tag ID and type onto the
--					screen.
--
--					This program utilizes the layered (OSI) approach. This file defines
--					the packages and APIs to include, as well as global variables and
--					functions used in the layers.
-----------------------------------------------------------------------------------*/

#ifndef HEADER_H
#define HEADER_H

#include <windows.h>
#include <string>
#include "API\SkyeTekAPI.h"
#include "API\SkyeTekProtocol.h"
#include <algorithm>
#include <iostream>
#include <commctrl.h>
using namespace std;

#define IDI_MYICON		101
#define IDC_MAIN_STATUS	102

#define IDM_START_BUTTON	103
#define IDM_STOP_BUTTON		104
#define IDM_CLEAR_BUTTON	105
#define IDM_HELP_BUTTON		106
#define IDM_EXIT_BUTTON		107

// Global variables
extern HANDLE tagThreadHandle; // handle for tag read thread
extern HWND hwnd;            // handle for window
extern HWND hwndListView;
extern HDC hdc;
extern LVCOLUMN lvc;
extern LVITEM   lv;
extern int listCounter;

// Function prototypes
DWORD WINAPI DiscoverDevices(LPVOID lpParameter);
int CallSelectTags(LPSKYETEK_READER lpReader);
unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user);
void DrawToStatusBar(char statusText[1000]);
void StopScanning(HANDLE thread);

#endif
