//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <tuple>
#include <algorithm>
#include "windows.h"
#include <climits>
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
MyImage			first_Image;				// The first frame
MyImage			second_Image;				// The second frame
MyImage			reconstructed_Image;		// The reconstructed frame predicted by motion vector
MyImage			difference_Image;			// The difference between first_Image and reconstructed_Image

HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text


// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// Parse the input command
	char commandline[1000];
	char firstFrame_fname[1000];
	char secondFrame_fname[1000];
	char parameter_k[3];
	int k;

	if (lpCmdLine[0] == 0) {
		wprintf(L"No command line arguement.");
		return -1;
	}
	int cnt=0;
	while (lpCmdLine[cnt]!= ' '&& lpCmdLine[cnt] !=0) {
		cnt++;
	}
	
	strcpy(commandline, lpCmdLine);
	printf("The whole command: %s\n", commandline);

	// parse the first frame
	strncpy(firstFrame_fname, commandline, cnt);
	firstFrame_fname[cnt] = '\0';
	printf("The firstFrame was: %s\n", firstFrame_fname);

	//parse the second frame
	int cnt1 = cnt+1;
	while (lpCmdLine[cnt1] != ' ') {
		cnt1++;
	}
	strncpy(secondFrame_fname, commandline+cnt+1, cnt1-cnt-1);
	secondFrame_fname[cnt1 - cnt - 1] = '\0';
	printf("The secondFrame was: %s\n", secondFrame_fname);

	//parse the k parameter: searching area size from 1~32
	int totalLen = strlen(lpCmdLine);
	strncpy(parameter_k, commandline+cnt1+1, totalLen-(cnt1+1));
	parameter_k[totalLen - (cnt1 + 1)] = '\0';
	k = atoi(parameter_k);
	printf("The k parameter was: %d\n", k);

	// Set up the images
	int w = 640;
	int h = 480;
	first_Image.setWidth(w);
	first_Image.setHeight(h);
	first_Image.setImagePath(firstFrame_fname);
	first_Image.ReadImage();
	first_Image.convert();
	unsigned char* first_YData = new unsigned char[640 * 480];
	first_YData = first_Image.getImageYData();

	second_Image.setWidth(w);
	second_Image.setHeight(h);
	second_Image.setImagePath(secondFrame_fname);
	second_Image.ReadImage();
	second_Image.convert(); 
	unsigned char* second_YData = new unsigned char[640 * 480];
	second_YData = second_Image.getImageYData();

	reconstructed_Image.setWidth(w);
	reconstructed_Image.setHeight(h);
	unsigned char* reconstructed_YData = new unsigned char[640 * 480];

	difference_Image.setWidth(w);
	difference_Image.setHeight(h);
	unsigned char* difference_YData = new unsigned char[640 * 480];

	unsigned char* first_block = new unsigned char[16 * 16];
	unsigned char* second_block = new unsigned char[16 * 16];
	unsigned char* min_block = new unsigned char[16 * 16];
	int counter_ = 0;
	int leftk, topk, rightk, bottomk;

	// There would be (640/16) * (480/16) = 40 * 30 = 1200 macro blocks in second_frame
	for (int y = 0; y < h; y += 16) {
		for (int x = 0; x < w; x += 16) {
			//cout << counter_ << ": " << x << ", " << y << endl;
			counter_ += 1;

			// Calculate the searching area
			leftk = x - 0;
			topk = y - 0;
			rightk = 639 - (x + 15);
			bottomk = 479 - (y + 15);
			if (leftk > k)
				leftk = k;
			if (topk > k)
				topk = k;
			if (rightk > k)
				rightk = k;
			if (bottomk > k)
				bottomk = k;
			//cout << leftk << ", " << topk << ", " << rightk << ", " << bottomk << endl;


			// Get the data of the target Macroblock in second frame
			for (int j = 0; j < 16; j++) {
				for (int i = 0; i < 16; i++) {
					second_block[j * 16 + i] = second_YData[(y + j) * 640 + (x + i)];
				}
			}

			// Co-located Macroblecks in the searching area of first frame
			// Run through all the searching area and find the min_block that could give the minimum difference
			double min_sad = LONG_MAX;
			for (int co_y = y - topk; co_y <= y + bottomk; co_y++) {
				for (int co_x = x - leftk; co_x <= x + rightk; co_x++) {
					
					// For each block in searching area, 
					// calculate the Sum Absolute Difference(SAD) between the block and the second_block of second frame
					double sad = 0.0;
					for (int j = 0; j < 16; j++) {
						for (int i = 0; i < 16; i++) {
							first_block[j * 16 + i] = first_YData[(co_y + j) * 640 + (co_x + i)];

							sad += abs(second_block[j * 16 + i] - first_block[j * 16 + i]);
						}
					}

					// If the sad value of this block is smaller than the current_minimum_sad, then update
					if (sad < min_sad) {
						for (int j = 0; j < 16; j++) {
							for (int i = 0; i < 16; i++) {
								min_block[j * 16 + i] = first_block[j * 16 + i];
								min_sad = sad;
							}
						}
					}
				}
			}
			//cout << "min_sad: " << min_sad << endl << endl;
			// After run through all the searching area, I got the min_block that gave me the minimum difference
			// Then store the value of this min_block to reconstruct_frame
			for (int j = 0; j < 16; j++) {
				for (int i = 0; i < 16; i++) {
					reconstructed_YData[(y + j) * 640 + (x + i)] = min_block[j * 16 + i];
				}
			}
		}
	}

	// For each block in second frame, we find a blcok in first frame that give the SAD
	// Then reconstructed the reconstruced_frame
	reconstructed_Image.setYData(reconstructed_YData);
	//difference_Image.operator=(second_Image);
	difference_Image.difference(reconstructed_YData, second_YData);
	


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
				   InvalidateRect(hWnd, &rt, false); 
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TO DO: Add any drawing code here...
				char text[1000];
				strcpy(text, "Left: Predicted Y channel for framen+1 \n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				strcpy(text, "\nRight: Error difference for Y channel \n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi, 0, sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = first_Image.getWidth();
				bmi.bmiHeader.biHeight = -first_Image.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = first_Image.getWidth() * first_Image.getHeight();

				//// Print out the first Image
				//SetDIBitsToDevice(hdc,
				//	100, 100, first_Image.getWidth(), first_Image.getHeight(),
				//	0, 0, 0, first_Image.getHeight(),
				//	first_Image.getImageData(), &bmi, DIB_RGB_COLORS);

				//// Print out the second Image
				//SetDIBitsToDevice(hdc,
				//	800, 100, second_Image.getWidth(), second_Image.getHeight(),
				//	0, 0, 0, second_Image.getHeight(),
				//	second_Image.getImageData(), &bmi, DIB_RGB_COLORS);

				// Print out reconstructed Image
				SetDIBitsToDevice(hdc,
					100, 100, reconstructed_Image.getWidth(), reconstructed_Image.getHeight(),
					0, 0, 0, reconstructed_Image.getHeight(),
					reconstructed_Image.getImageData(), &bmi, DIB_RGB_COLORS);

				// Print out the error difference
				SetDIBitsToDevice(hdc,
					800, 100, difference_Image.getWidth(), difference_Image.getHeight(),
					0, 0, 0, difference_Image.getHeight(),
					difference_Image.getImageData(), &bmi, DIB_RGB_COLORS);

				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


