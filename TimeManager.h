#ifndef __GUI_timemanager_h__
#define __GUI_timemanager_h__

#include <algorithm>

#include "SDL2/SDL.h"

struct TimeManager
{	
	float currentDelta;
	Uint32 currentTime;

	void Init()
	{	
		SDL_Init( SDL_INIT_TIMER );
		currentDelta = 0.0f;
		currentTime = SDL_GetTicks();
	}

	void Update()
	{
		SDL_Delay(1);
		const Uint32 newTime = SDL_GetTicks();
		currentDelta = (float)(newTime - currentTime) / 1000.0f;
		currentTime = newTime;
	}
};

#endif	//	__GUI_timemanager_h__