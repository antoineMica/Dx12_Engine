/* Start Header -------------------------------------------------------
Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen
Institute of Technology is prohibited.
Language: C++
Author: Antoine.micaelian
End Header --------------------------------------------------------*/
#pragma once

#include "ISystem.h"
#include "CoreHeader.h" 

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1


class InputSystem : public ISystem
{
private:
	char	CurrentKey[256];
	char	PreviousKey[256];

	char	CurrentMouseButton[2];
	char	PreviousMouseButton[2];

	bool	gMouseInsideWindow;


public:
	/*
	Function: Constructor
	Purpose: Initialize the input manager

	Sets all the keys(basically all the values are false). to zero using memset.
	*/
	InputSystem();

	~InputSystem();

	void Initialize();
	void Shutdown() {}
	bool HandleInputMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Update(float dt);

	/*  Keyboard input */
	bool KeyIsPressed(unsigned  char key);
	bool KeyIsTriggered(unsigned  char key);
	bool KeyPrevIsPressed(unsigned  char key);
	bool KeyPrevIsTriggered(unsigned  char key);

	/*  mouse input */
	bool MouseIsPressed(unsigned  int button);
	bool MouseIsTriggered(unsigned  int button);
	bool MousePrevIsPressed(unsigned  int button);
	bool MousePrevIsTriggered(unsigned  int button);

	float GetMousePositionX();
	float GetMousePositionY();
};

extern InputSystem * gInput;
