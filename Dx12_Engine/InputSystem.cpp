/* Start Header -------------------------------------------------------
Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen
Institute of Technology is prohibited.
Language: C++
Author: Antoine.micaelian
End Header --------------------------------------------------------*/
#include "InputSystem.h"
#include "Win32Window.h"

InputSystem * gInput = NULL;

/*
Function: Constructor
Purpose: Initialize the input manager

Sets all the keys(basically all the values are false). to zero using memset.
*/
InputSystem::InputSystem()
{
	gInput = this;
}

InputSystem::~InputSystem()
{
	gInput = NULL;
}

void InputSystem::Initialize()
{
	// Initialize all the values of the key arrays to 0
	memset(CurrentKey, 0, 256);
	memset(PreviousKey, 0, 256);

	// Initialize all the values of the mouse arrays to 0
	memset(CurrentMouseButton, 0, 2);
	memset(PreviousMouseButton, 0, 2);

}

/*

Function: Handle InputMessages
Purpose: handles the window messages that relate to input:

WM_KEYDOWN
WM_MOUSEDOWN
...

it returns true if it handled the message, false otherwise
*/
bool InputSystem::HandleInputMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Select the message type
	switch (msg)
	{
		// Key pressed
	case WM_KEYDOWN:
		CurrentKey[wParam] = 1; // Set the corresponding key to pressed
		break;
		// Key released
	case WM_KEYUP:
		CurrentKey[wParam] = 0; // Set the corresponding key to released
		break;
		// Left mouse button pressed
	case WM_LBUTTONDOWN:
		CurrentMouseButton[MOUSE_LEFT] = 1; // Set the left mouse button to pressed
		break;
		// Left mouse button released
	case WM_LBUTTONUP:
		CurrentMouseButton[MOUSE_LEFT] = 0; // Set the left mouse button to released
		break;
		// Right mouse button pressed
	case WM_RBUTTONDOWN:
		CurrentMouseButton[MOUSE_RIGHT] = 1; // Set the right mouse button to pressed
		break;
		// Right mouse button released
	case WM_RBUTTONUP:
		CurrentMouseButton[MOUSE_RIGHT] = 0; // Set the right mouse button to released
		break;
	default:
		return false; // Return that the message was not handled
		break;
	}

	return true; // Return that the message was handled

}

/*
computes whether each key is pressed or not and
determine if it is triggered or not.

Also compute the mouse position.
*/
void InputSystem::Update(float dt)
{
	POINT pos; // Windows point
	GetCursorPos(&pos); // Get the cursor position in the screen
	//ScreenToClient(gWindow->GetHandle(), &pos); // Convert the position into window position

																							// Convert the window position into window centered
	//mousePos_.x = ((float)(pos.x - gWindow->GetWidth() / 2));
	//mousePos_.y = ((float)(-pos.y + gWindow->GetHeight() / 2));

	// Check if the position on the screen is outside the window
	if (pos.x >= gWindow->GetWidth() || pos.x <= 0
		|| pos.y >= gWindow->GetHeight() || pos.y <= 0)
	{
		gMouseInsideWindow = false; // Set inside window to false
	}
	else
	{
		gMouseInsideWindow = true; // Set inside window to true
	}

	// Go through all the keys
	for (int i = 0; i<256; i++)
	{
		// If the corresponding key is pressed or triggered
		if (CurrentKey[i]/* == 1 || CurrentKey[i] == 2*/)
		{
			// Check if it was not pressed
			if (PreviousKey[i] == 0)
			{
				CurrentKey[i] = 2; // Set key to triggered
			}
			else
			{
				CurrentKey[i] = 1; // Set key to pressed
			}
		}
		//else
		//{
		//	CurrentKey[i] = 0; // Set key to not pressed
		//}

		PreviousKey[i] = CurrentKey[i]; // Set the current value in the previous
	}

	// Go through all the mouse buttons
	for (int i = 0; i<2; i++)
	{
		// If the corresponding button is pressed or triggered
		if (CurrentMouseButton[i]/* == 1 || CurrentMouseButton[i] == 2*/)
		{
			// Check if it was not pressed
			if (PreviousMouseButton[i] == 0)
			{
				CurrentMouseButton[i] = 2; // Set button to triggered
			}
			else
			{
				CurrentMouseButton[i] = 1; // Set button to pressed
			}
		}
		//else
		//{
		//	CurrentMouseButton[i] = 0; // Set button to not pressed
		//}

		PreviousMouseButton[i] = CurrentMouseButton[i]; // Set the current value in the previous
	}
}

/*
Check if the key past as a parameter is pressed and return true if so
*/
bool InputSystem::KeyIsPressed(unsigned char key)
{
	return (CurrentKey[key] >= 1); // Return if the key is pressed
}

/*
Check if the key past as a parameter is triggered and return true if so
*/
bool InputSystem::KeyIsTriggered(unsigned  char key)
{
	return (CurrentKey[key] == 2); // Return if the key is triggered
}

/*
Check if the key past as a parameter was pressed and return true if so
*/
bool InputSystem::KeyPrevIsPressed(unsigned  char key)
{
	return (PreviousKey[key] >= 1); // Return if the key was pressed
}

/*
Check if the key past as a parameter was triggered and return true if so
*/
bool InputSystem::KeyPrevIsTriggered(unsigned  char key)
{
	return (PreviousKey[key] == 2); // Return if the key was triggered
}

/*
Check if the button past as a parameter is pressed and return true if so
*/
bool InputSystem::MouseIsPressed(unsigned  int button)
{
	return (CurrentMouseButton[button] >= 1); // Return if the button is pressed
}

/*
Check if the button past as a parameter is triggered and return true if so
*/
bool InputSystem::MouseIsTriggered(unsigned int button)
{
	return (CurrentMouseButton[button] == 2); // Return if the button is triggered
}

/*
Check if the button past as a parameter was pressed and return true if so
*/
bool InputSystem::MousePrevIsPressed(unsigned int button)
{
	return (PreviousMouseButton[button] >= 1); // Return if the button was pressed
}

/*
Check if the button past as a parameter was triggered and return true if so
*/
bool InputSystem::MousePrevIsTriggered(unsigned int button)
{
	return (PreviousMouseButton[button] == 2); // Return if the button was triggered
}
