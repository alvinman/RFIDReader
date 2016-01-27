/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	Application.cpp - Application layer of an RFID reader application,
--							handling the initializing and termination of user sessions.
--
--	PROGRAM:        RFID Reader Application
--
--	FUNCTIONS:
--					int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
-- 						  LPSTR lspszCmdParam, int nCmdShow)
--					LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
--                        WPARAM wParam, LPARAM lParam)
--					void DrawToStatusBar(char statusText[1000])
--					HWND CreateSimpleToolbar(HINSTANCE hInst, HWND hWndParent)
--					HWND CreateListView(HINSTANCE hInst, HWND hWndParent) 
--					HWND CreateStatusBar(HINSTANCE hInst, HWND hWndParent)
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	NOTES:			Application.cpp is part of an RFID reader application, that uses the
--					SkeyeTek API to connect to an RFID device, and allows for the
--					reading of RFID tags and printing the tag ID and type onto the
--					screen.
--
--					This program utilizes the layered (OSI) approach. This file makes
--					up the Application layer of this model, responsible for handling
--					all user facing functionality, including window layouts, UI, toolbar,
--					and status bar.
-----------------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#pragma warning (disable: 4096)

// function prototype
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateSimpleToolbar(HINSTANCE hInst, HWND hWndParent);
HWND CreateListView(HINSTANCE hInst, HWND hWndParent);
HWND CreateStatusBar(HINSTANCE hInst, HWND hWndParent);


// declared variables
static TCHAR Name[] = TEXT("RFID Reader Application");
static TCHAR HelpMessage[] = TEXT("This program allows you to connect to an RFID reader ")
TEXT("and display scanned RFID tag information to the display.  Use the 'Start' button ")
TEXT("to connect to a reader to read tags, and 'Stop' to stop reading.  You can press 'Clear' to ")
TEXT("erase all existing Tag information displayed on the screen.");
HWND hwnd;     
HWND hwndStatus;
HWND hwndListView;
HWND hWndToolbar;
HANDLE tagThreadHandle;
DWORD threadId;
RECT rcWindow;
LVCOLUMN lvc;
LVITEM   lv;

/*-----------------------------------------------------------------------------------
--	FUNCTION: WinMain
--
--	DATE:			October 19, 2015
--					
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
-- 						LPSTR lspszCmdParam, int nCmdShow)
--
--	RETURNS:		int
--
--	NOTES:			This is the initial entry point for the program.  It is
--					responsible for the message retrieval and dispatch loop that
--					that provides top-level control for the remainder of execution.
-----------------------------------------------------------------------------------*/
int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
 						  LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX WndClsEx;

	// Define a Window class
	WndClsEx.cbSize = sizeof (WNDCLASSEX);
	WndClsEx.style = 0; // default style
	WndClsEx.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MYICON)); // large icon 
	WndClsEx.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MYICON)); // use small version of large icon
	WndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	WndClsEx.lpfnWndProc = WndProc; // window function
	WndClsEx.hInstance = hInst; // handle to this instance
	WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClsEx.lpszClassName = "Firstclass"; // window class name
	
	WndClsEx.lpszMenuName = TEXT("MYMENU"); // no class menu 
	WndClsEx.cbClsExtra = 0;      // no extra memory needed
	WndClsEx.cbWndExtra = 0;
		
	// Register the class
	if (!RegisterClassEx (&WndClsEx))
		return 0;

	// Create main window
	hwnd = CreateWindowEx(
		0,									// Optional window styles.
		"Firstclass",                       // Window class
		"RFID Reader",						// Window text
		WS_OVERLAPPEDWINDOW,                // Window style
		CW_USEDEFAULT, CW_USEDEFAULT,       // Coordinates
		800, 600,							// Dimensions
		NULL,								// Parent window    
		NULL,							    // Menu
		hInst,								// Instance handle
		NULL								// Additional application data
		);

	GetWindowRect(hwnd, &rcWindow);

	// setup user interface
	CreateListView(hInst, hwnd);
	CreateStatusBar(hInst, hwnd);
	CreateSimpleToolbar(hInst, hwnd);

	// set application icon
	HANDLE icon = LoadImage(NULL, "menu_icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
	
	// Display the window
	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);

	// Create the message loop
	while (GetMessage (&Msg, NULL, 0, 0))
	{
   		TranslateMessage (&Msg); // translate keyboard messages
		DispatchMessage (&Msg); // dispatch message and return control to windows
	}

	return Msg.wParam;
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: WndProc
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
--						WPARAM wParam, LPARAM lParam)
--
--	RETURNS:		LRESULT
--
--	NOTES:			Handles all messages sent to the window.
-----------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
                          WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintstruct;

	switch (Message)
	{
		case WM_COMMAND: 
			switch (LOWORD(wParam))
			{ 
				case IDM_HELP_BUTTON:
					MessageBox(hwnd, HelpMessage, "Help", MB_OK);
					break;
				case IDM_START_BUTTON:
					tagThreadHandle = CreateThread(NULL, 0, DiscoverDevices, NULL, 0, &threadId);
					break;
				case IDM_STOP_BUTTON:
					StopScanning(tagThreadHandle);
					break;
				case IDM_CLEAR_BUTTON:
					ListView_DeleteAllItems(hwndListView);
					listCounter = 0;
					DrawToStatusBar("Tags cleared");
					break;
				case IDM_EXIT_BUTTON:
					PostQuitMessage(0);
					break;
				}
			break;
		case WM_SIZE:
		{
			// Auto-resize statusbar, toolbar and listview
			GetWindowRect(hwnd, &rcWindow);
			SendMessage(GetDlgItem(hwnd, IDC_MAIN_STATUS), WM_SIZE, 0, 0);
			SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 100);
			MoveWindow(hWndToolbar, 0, 18, LOWORD(lParam), rcWindow.bottom - rcWindow.top, TRUE);
			MoveWindow(hwndListView, 0, 55, LOWORD(lParam), rcWindow.bottom - rcWindow.top, TRUE);
			break;
		}
		case WM_PAINT:		// Process a repaint message
			hdc = BeginPaint(hwnd, &paintstruct); // Acquire DC

			RECT windowDimension;
			GetWindowRect(hwnd, &windowDimension);

			EndPaint(hwnd, &paintstruct); // Release DC
			break;
		case WM_DESTROY:		// message to terminate the program
			PostQuitMessage (0);
		break;
		default: // Let Win32 process all other messages
			return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	return 0;	
}
	
/*-----------------------------------------------------------------------------------
--	FUNCTION: DrawToStatusBar
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		void DrawToStatusBar(char statusText[1000])
--
--	RETURNS:		void
--
--	NOTES:			Sets the text of the status bar.
-----------------------------------------------------------------------------------*/
void DrawToStatusBar(char statusText[1000]) {
	SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)statusText);
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: CreateSimpleToolbar
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		HWND CreateSimpleToolbar(HINSTANCE hInst, HWND hWndParent)
--
--	RETURNS:		HWND
--
--	NOTES:			Initializes the toolbar and creates the appropriate user control
--					buttons.
-----------------------------------------------------------------------------------*/
HWND CreateSimpleToolbar(HINSTANCE hInst, HWND hWndParent)
{
	HIMAGELIST g_hImageList = NULL;

	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 5;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		CCS_NOPARENTALIGN | WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_LIST | TBSTYLE_FLAT, 0, 20, 0, 0,
		hWndParent, NULL, hInst, NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Create the image list.
	g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		numButtons, 0);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.
	TBBUTTON tbButtons[numButtons] =
	{
		{ MAKELONG(STD_FIND,  ImageListID), IDM_START_BUTTON,  TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)"Start" },
		{ MAKELONG(STD_UNDO, ImageListID), IDM_STOP_BUTTON, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)"Stop" },
		{ MAKELONG(STD_REPLACE, ImageListID), IDM_CLEAR_BUTTON, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)"Clear" },
		{ MAKELONG(STD_HELP, ImageListID), IDM_HELP_BUTTON, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)"Help" },
		{ MAKELONG(STD_DELETE, ImageListID), IDM_EXIT_BUTTON, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)"Exit" }
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);
	SendMessage(hWndToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(20, 20));
	SendMessage(hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(20, 20));

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_BUTTON);
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 20);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;	
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: CreateListView
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		HWND CreateListView(HINSTANCE hInst, HWND hWndParent)
--
--	RETURNS:		HWND
--
--	NOTES:			Initializes the listview and sets up the appropriate columns and
--					column headings.
-----------------------------------------------------------------------------------*/
HWND CreateListView(HINSTANCE hInst, HWND hWndParent) {
	// Listview setup
	INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	// Listview row and column setup
	lvc = { 0 };
	lv = { 0 };
	lv.mask = LVIF_IMAGE | LVIF_STATE;
	lv.state = 0;
	lv.stateMask = 0;
	lv.iSubItem = 0;

	hwndListView = CreateWindow(
		WC_LISTVIEW,
		"Listview",
		WS_CHILD | LVS_REPORT | WS_VISIBLE,
		0, 55,
		rcWindow.right - rcWindow.left, (rcWindow.bottom - rcWindow.top),
		hWndParent,
		NULL,
		hInst,
		NULL);

	lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT;
	lvc.fmt = LVCFMT_LEFT;

	lvc.iSubItem = 0;
	lvc.cx = 50;
	lvc.pszText = TEXT("Tag Count");
	ListView_InsertColumn(hwndListView, 0, &lvc);

	lvc.iSubItem = 1;
	lvc.cx = 250;
	lvc.pszText = TEXT("Tag ID");
	ListView_InsertColumn(hwndListView, 1, &lvc);

	lvc.iSubItem = 2;
	lvc.cx = 350;
	lvc.pszText = TEXT("Tag Type");
	ListView_InsertColumn(hwndListView, 2, &lvc);

	return hwndListView;
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: CreateStatusBar
--
--	DATE:			October 19, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man / Oscar Kwan
--
--	PROGRAMMER:		Alvin Man / Oscar Kwan
--
--	INTERFACE:		HWND CreateStatusBar(HINSTANCE hInst, HWND hWndParent)
--
--	RETURNS:		HWND
--
--	NOTES:			Initializes the status bar for displaying status messages.
-----------------------------------------------------------------------------------*/
HWND CreateStatusBar(HINSTANCE hInst, HWND hWndParent) {
	// Create the status bar.
	hwndStatus = CreateWindowEx(
		0,								   // no extended styles
		STATUSCLASSNAME,			       // name of status bar class
		(PCTSTR)NULL,				       // no text when first created       
		CCS_TOP | WS_CHILD | WS_VISIBLE,   // creates a visible child window
		0, 0, 0, 0,						   // ignores size and position
		hWndParent,							   // handle to parent window
		(HMENU)IDC_MAIN_STATUS,			   // child window identifier
		hInst,							   // handle to application instance
		NULL);							   // no window creation data

	SetWindowPos(hwndStatus, HWND_TOP, 0, 10, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return hwndStatus;
}