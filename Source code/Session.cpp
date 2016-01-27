/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	Session.cpp - Session layer of an RFID reader application, handling
--								  the initializing and termination of user sessions.
--
--	PROGRAM:        RFID Reader Application
--
--	FUNCTIONS:
--					void DiscoverDevices(LPVOID lpParameter)
--					void StopScanning(HANDLE thread)
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	NOTES:			Session.cpp is part of an RFID reader application, that uses the
--					SkeyeTek API to connect to an RFID device, and allows for the
--					reading of RFID tags and printing the tag ID and type onto the
--					screen.
--
--					This program utilizes the layered (OSI) approach. This file makes
--					up the Session layer of this model, responsible for handling
--					initializing and terminating user sessions, allowing users to
--					start and stop scanning from the RFID reader.
-----------------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

LPSKYETEK_DEVICE *devices = NULL;
LPSKYETEK_READER *readers = NULL;
int numDevices = 0;
int numReaders = 0;

/*-----------------------------------------------------------------------------------
--	FUNCTION: DiscoverDevices
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		DWORD WINAPI DiscoverDevices(LPVOID lpParameter)
--
--	RETURNS:		DWORD
--
--	NOTES:			Uses the SkyeTek API to search for an RFID reader.  If found, the
--					program enters a loop to scan for RFID tags in the vicinity. When
--					a tag is found, the SelectLoopCallback function is called.
--					
--					This function is called inside a new thread when the user initiates
--					a new scanning session.
-----------------------------------------------------------------------------------*/
DWORD WINAPI DiscoverDevices(LPVOID lpParameter) {
	int ix = 0;
	
	DrawToStatusBar("Discovering devices..... (Takes around 5 seconds)");
	numDevices = SkyeTek_DiscoverDevices(&devices);

	if (numDevices == 0) {
		DrawToStatusBar("No devices found.....");
	}

	DrawToStatusBar("Discovering readers..... (Takes around 5 seconds)");
	numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers);

	if (numReaders == 0) {
		DrawToStatusBar("No readers found.....");
		SkyeTek_FreeDevices(devices, numDevices);
	}

	if (numReaders) {
		DrawToStatusBar("Reader found, ready to start reading tags.....");
		SkyeTek_SelectTags(readers[0], AUTO_DETECT, SelectLoopCallback, 0, 1, NULL);
	}

	SkyeTek_FreeDevices(devices, numDevices);
	SkyeTek_FreeReaders(readers, numReaders);

	return 1;
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: StopScanning
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		StopScanning(HANDLE thread)
--
--	RETURNS:		void
--
--	NOTES:			Suspends the thread for reading RFID tags.  Called when the user
--					clicks the 'Stop Scanning' button.
-----------------------------------------------------------------------------------*/
void StopScanning(HANDLE thread) {
	TerminateThread(thread, 0);
	DrawToStatusBar("Scanning stopped. Click Start to start scanning again.");

	SkyeTek_FreeDevices(devices, numDevices);
	SkyeTek_FreeReaders(readers, numReaders);
}