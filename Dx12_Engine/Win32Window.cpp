/* Start Header -------------------------------------------------------
Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen
Institute of Technology is prohibited.
Language: C++
Author: Antoine.micaelian
End Header --------------------------------------------------------*/

#include "Win32Window.h"
#include "InputSystem.h"

Win32Window * gWindow = NULL;

void Win32Window::SetCursorPosition(float x, float y)
{
	POINT p;
	p.x = (LONG)(gWindowWidth_ / 2.0f + x);
	p.y = (LONG)(gWindowHeight_ / 2.0f + y);
	ClientToScreen(gWindowHandle_, &p);
	SetCursorPos(p.x, p.y);
}

void Win32Window::ShowMouse(bool show)
{
	ShowCursor(show);
}

/******************************************************************************/
/*!
\brief
Window Manager Constructor

\param hinstance
instance of the application

\param show


\param WindowWidth
the width of the window

\param WindowHeight
the height of the window

\param WindowTitle
the name of the window

*/
/******************************************************************************/
Win32Window::Win32Window(HINSTANCE hinstance, int show, int WindowWidth, int WindowHeight, const char * WindowTitle)
{
	gWindowHandle_ = NULL;
	gAppInstance_ = NULL;
	// create a window class and fill it up with the appropriate parameters
	// http://msdn.microsoft.com/en-us/library/ms633577.aspx

	// Store the window width & height
	gWindowWidth_ = WindowWidth;
	gWindowHeight_ = WindowHeight;

	gWindowClass_.cbSize = sizeof(WNDCLASSEX);					// The default size of our window class structure
	gWindowClass_.style = CS_HREDRAW | CS_VREDRAW;				// the style of the window
	gWindowClass_.lpfnWndProc = MessageHandler;					// Pointer to our message handler function (declared above, but defined below)
	gWindowClass_.cbClsExtra = 0;									// The number of extra bytes you want to allocate for this window class structure. the default is 0
	gWindowClass_.cbWndExtra = 0;									// The number of extra bytes you want to allocate for the window instance.
	gWindowClass_.hInstance = hinstance;							// Handle to the instance that contains the window procedure for the class
	gWindowClass_.hIcon = NULL;									// Let the system provide the default icon.
	gWindowClass_.hCursor = LoadCursor(NULL, IDC_ARROW);		// Use default cursor. The value of hinstance in that function is null in order to use one of the predefined cursors.
	gWindowClass_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Handle tot eh calss background brush. It can be used as a color
	gWindowClass_.lpszMenuName = NULL;								// No menu for that window
	gWindowClass_.hIconSm = NULL;								// A handle to a small icon that is associated with the window class. We will use the same icon as the default one.
	gWindowClass_.lpszClassName = WindowTitle;							// A string that specifies the window class name. The name must be less than 255.

																		// Register the class - Show a message if the registration failed
	if (!RegisterClassEx(&gWindowClass_))
	{
		MessageBox(NULL, "Call to RegisterClassEx Failed", "", NULL);
		return;
	}

	// IMPORTANT STEP - The width you pass to the create window function is 
	// the width of the window including the title bar and the borders
	// to get the actual size of a window given the desired CLIENT area
	// use AdjustWindowRect function 
	// http://msdn.microsoft.com/en-us/library/ms632665(v=vs.85).aspx
	RECT winrect = { 0,0,WindowWidth, WindowHeight };
	AdjustWindowRect(&winrect, WS_OVERLAPPEDWINDOW, FALSE);

	// Register was successful->Create the window - http://msdn.microsoft.com/en-us/library/windows/desktop/ms632679(v=vs.85).aspx
	gWindowHandle_ = CreateWindow(
		WindowTitle,			// The name of the window class - we used the same name as the window title 
		WindowTitle,			// The window title - This will appear in the title bar and can be changed at run time
		WS_OVERLAPPEDWINDOW,	// The style of the window, this will create a classic window, see msdn for more styles
		0,						// position of the window
		0,						// position of the window
		winrect.right - winrect.left,	// Width of the window in window size NOT client size
		winrect.bottom - winrect.top,	// Height of the windowin window size NOT client size
		NULL,					// Parent Window
		NULL,					// Handle to the menu
		hinstance,				// Instance of the application to be associated with that window
		NULL);					// Pointer to the value to be passed to the window when it's created and when it receives the WM_CREATE message. No need here.

								// NOTE: If the window was not created successfully, then the handle returned by CreateWindow should be null. 

								// Post an error message if the window was not created successfully
	if (!gWindowHandle_)
	{
		MessageBox(NULL, "Failed to Create the window!", "", NULL);
		return;
	}


	PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)  
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD  
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class  
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window  
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels  
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)  
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)  
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD 

	HDC hdc = GetDC(gWindowHandle_);

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd); // Check if our PFD is valid and get a pixel format back  
	if (nPixelFormat == 0) // If it fails  
	{
		MessageBox(NULL, "Failed to Create the window!", "", NULL);
		return;
	}

	bool bResult = SetPixelFormat(hdc, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD  
	if (!bResult) // If it fails  
	{
		MessageBox(NULL, "Failed to Create the window!", "", NULL);
		return;
	}

	// Creating the window was successful -> Show the window
	ShowWindow(gWindowHandle_, show);
	UpdateWindow(gWindowHandle_);				// Updates the clien area of the window by sending a paint message to the window.

												// Keep a copy of the App Instance for unregistering the class
	gAppInstance_ = hinstance;


	gWindow = this;
	quit_ = false;

}



/******************************************************************************/
/*!
\brief
Unregisters the window class

*/
/******************************************************************************/
void Win32Window::Shutdown()
{
	UnregisterClass(gWindowClass_.lpszClassName, gAppInstance_);
}



/******************************************************************************/
/*!
\brief
Translates and dispatches messages

*/
/******************************************************************************/
void Win32Window::Update(float dt)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


/******************************************************************************/
/*!
\brief
Window Manager Destructor

*/
/******************************************************************************/
Win32Window::~Win32Window()
{
	gWindow = NULL;
}


/******************************************************************************/
/*!
\brief
returns the handle to the window

*/
/******************************************************************************/

HWND Win32Window::GetHandle(void)
{

	return gWindowHandle_;
}

/******************************************************************************/
/*!
\brief
returns the instace of the app

*/
/******************************************************************************/
HINSTANCE Win32Window::GetInstance(void)
{

	return gAppInstance_;
}



/******************************************************************************/
/*!
\brief
returns the height of the window

*/
/******************************************************************************/
int Win32Window::GetHeight(void)
{
	return gWindowHeight_;
}


/******************************************************************************/
/*!
\brief
returns the width of the window

*/
/******************************************************************************/
// Returns the width of the window
int Win32Window::GetWidth(void)
{
	return gWindowWidth_;
}

/*
The message handler function
Purpose: Handles all the messages sent to the window by the OS (or other window, or self)

Parameters:

- hWnd:		Handle to the window that received the message
- msg:		Message ID
- wParam:	A Message parameter. WPARAM is defined in WinDef.h as 'typedef UINT_PTR WPARAM'
- lParam:	Another Message parameter. LPARAM is defined in WinDef.h as 'typedef LONG_PTR LPARAM'
*/
LRESULT CALLBACK Win32Window::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC dc;
	PAINTSTRUCT ps;

	//if (TwEventWin(hWnd, msg, wParam, lParam)) // send event message to AntTweakBar
	//	return 0; // event has been handled by AntTweakBar

	if (gInput->HandleInputMessage(hWnd, msg, wParam, lParam))
		return 0;



	switch (msg)
	{

	case WM_DESTROY:
		PostQuitMessage(0);
		gWindow->quit_ = true;
		break;
	case WM_CREATE:
		break;

	case WM_PAINT:
		dc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
		/* called any time the window is moved */
	case WM_MOVE:
		/* Invalidate the rect to force a redraw */
		InvalidateRect(hWnd, NULL, FALSE);
		// When we don't handle a message. We call the default window procedure. This will
		// ensure that every message is processed.
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}

	return 0;
}


void Win32Window::SetPosition(float x, float y)
{
	// Set the window position
	SetWindowPos(gWindowHandle_, NULL,
		(int)x,
		(int)y,
		(int)gWindowWidth_,
		(int)gWindowHeight_,
		SWP_SHOWWINDOW | SWP_NOSIZE);
}

void Win32Window::SetStyle(bool fullscreen)
{

	RECT winrect = { 0,0, (LONG)gWindowWidth_, (LONG)gWindowHeight_ };

	if (fullscreen)
	{
		SetWindowLong(gWindowHandle_, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_VISIBLE);
		AdjustWindowRect(&winrect, WS_POPUP | WS_SYSMENU | WS_VISIBLE, FALSE);


	}
	else
	{
		SetWindowLong(gWindowHandle_, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		AdjustWindowRect(&winrect, WS_BORDER, FALSE);
	}



	SetWindowPos(
		gWindowHandle_,		// handle to our window
		NULL,				// handle of the window to insert after - ignored
		0,					// x position (this will be ignored)
		0,					// y position (this will be ignored)
		winrect.right - winrect.left,					// new width of the window
		winrect.bottom - winrect.top,					// new height of the window
		SWP_SHOWWINDOW			// flags: in this case just ignore the position
	);

}

void Win32Window::SetSize(float w, float h)
{
	gWindowWidth_ = (int)w;
	gWindowHeight_ = (int)h;

	RECT winrect = { 0,0, (LONG)gWindowWidth_, (LONG)gWindowHeight_ };

	AdjustWindowRect(&winrect, WS_POPUP | WS_SYSMENU | WS_VISIBLE, FALSE);



	SetWindowPos(
		gWindowHandle_,		// handle to our window
		NULL,				// handle of the window to insert after - ignored
		0,					// x position (this will be ignored)
		0,					// y position (this will be ignored)
		winrect.right - winrect.left,					// new width of the window
		winrect.bottom - winrect.top,					// new height of the window
		SWP_SHOWWINDOW			// flags: in this case just ignore the position
	);
}

void Win32Window::SetText(std::string text)
{
	SetWindowText(gWindowHandle_, text.c_str());

}