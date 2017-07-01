/* Start Header -------------------------------------------------------
Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen
Institute of Technology is prohibited.
Language: C++
Author: Antoine.micaelian
End Header --------------------------------------------------------*/
#pragma once

#include "CoreHeader.h"
#include "ISystem.h"


/*************************************************************************/
/*!
\class 	Win32Window
\brief  Creates and updates the window of the application
*/
/*************************************************************************/

class Win32Window : public ISystem
{

private:
	HWND gWindowHandle_;
	WNDCLASSEX gWindowClass_;
	HINSTANCE gAppInstance_;
	int gWindowWidth_;
	int gWindowHeight_;

public:

	Win32Window(HINSTANCE hinstance, int show, int WindowWidth, int WindowHeight, const char * WindowTitle);
	~Win32Window();

	void Initialize() {}
	void Shutdown();
	void Update(float dt);

	static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND GetHandle(void);
	int GetWidth(void);
	int GetHeight(void);
	HINSTANCE GetInstance(void);

	void SetText(std::string text);
	void SetPosition(float x, float y);
	void SetStyle(bool fullscreen);
	void SetSize(float w, float h);
	void SetCursorPosition(float x, float y);
	void ShowMouse(bool show);

	bool quit_;
};

extern Win32Window * gWindow;

