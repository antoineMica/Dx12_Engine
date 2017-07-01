#pragma once


// include  windows
#include <Windows.h>

// include STL containers
#include <string>
#include <list>
#include <vector>
#include <map>

#include <iostream>
#include <fstream>
#include <stdio.h>  /* printf */
#include <stdlib.h> /* rand, srand */
#include <time.h>   /* time        */
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

typedef std::string String;

#define PI 3.1415926535897932384626433832795f


inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}


// CRT debug memory leak line output
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif