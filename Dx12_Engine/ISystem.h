/* Start Header -------------------------------------------------------
Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen
Institute of Technology is prohibited.
Language: C++
Project : Masters CS562 deferred shading engine
Author: Antoine.micaelian
End Header --------------------------------------------------------*/
#pragma once

#include "CoreHeader.h"

class ISystem
{
public:
	virtual ~ISystem() {};
	virtual void Initialize() = 0;
	virtual void Update(float dt = 0) = 0;
	virtual void Shutdown() = 0;
};
