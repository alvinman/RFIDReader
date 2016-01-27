/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	Physical.cpp - Physical layer of an RFID reader application,
--							handling the tag reading functionality.
--
--	PROGRAM:        RFID Reader Application
--
--	FUNCTIONS:
--					unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user)
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	NOTES:			Physical.cpp is part of an RFID reader application, that uses the
--					SkeyeTek API to connect to an RFID device, and allows for the
--					reading of RFID tags and printing the tag ID and type onto the
--					screen.
--
--					This program utilizes the layered (OSI) approach. This file makes
--					up the Physical layer of this model, responsible for handling
--					system level functionality, including the reading of the RFID tag
--					information.
-----------------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

// declared variables
HDC hdc;
char tagTypeBuffer[2];
char idBuffer[2];
bool isStop = FALSE;
int listCounter = 0;

/*-----------------------------------------------------------------------------------
--	FUNCTION: SelectLoopCallback
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user)
--
--	RETURNS:		unsigned char
--
--	NOTES:			Callback function called by SelectTags when a tag has been found.  This 
--					function handles retrieving the tag data and inserting it into the
--					listview for display.
-----------------------------------------------------------------------------------*/
unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user) {
	char counterBuffer[40];
	LPSTR friendlyLPSTR, typeLPSTR;
	string friendlyString, typeString;

	if (!isStop) {
		if (lpTag != NULL) {
			DrawToStatusBar("Reading tags.....");
			hdc = GetDC(hwnd);

			// get friendly text from tag
			for (int i = 0; i < sizeof(lpTag->friendly); i++) {
				sprintf_s(idBuffer, "%s", lpTag->friendly+i);
				if (idBuffer[0] == '\0') {
					continue;
				}

				// convert char buffer and add to string
				friendlyString += idBuffer;
				// convert string to lpstr to pass into ListView_SetItemText
				friendlyLPSTR = const_cast<char *>(friendlyString.c_str());
			}

			// get type text from tag
			for (int i = 0; i < (sizeof(SkyeTek_GetTagTypeNameFromType(lpTag->type)) * 16); i++) {
				sprintf_s(tagTypeBuffer, "%s", SkyeTek_GetTagTypeNameFromType(lpTag->type) + i);

				if (tagTypeBuffer[0] == '\0') {
					continue;
				}

				// convert char buffer and add to string
				typeString += tagTypeBuffer;

				// convert string to lpstr to pass into ListView_SetItemText
				typeLPSTR = const_cast<char *>(typeString.c_str());
			}

			lv.iItem = listCounter;
			sprintf_s(counterBuffer, "%d", listCounter);
			ListView_InsertItem(hwndListView, &lv);

			// sets counter, id, and type (displays on screen)
			ListView_SetItemText(hwndListView, listCounter, 0, counterBuffer);
			ListView_SetItemText(hwndListView, listCounter, 1, friendlyLPSTR);
			ListView_SetItemText(hwndListView, listCounter, 2, typeLPSTR);

			// increment counter (row)
			listCounter++;
			
			ReleaseDC((HWND)hwnd, hdc);
			SkyeTek_FreeTag(lpTag);
		}
	}
	return (!isStop);
}