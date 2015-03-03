#ifndef __GUI_navigator_h__
#define __GUI_navigator_h__

#include "CApp.h"
#include "GUI_Tools.h"

//	Getting started. This will turn into a singleton probably. 
struct NavigationPanel
{
//	AREP::TODO::Move out of GUI into a model structure. 
	float targetSpeed, targetTurn;
	float currentSpeed, currentTurn;

	void AdjustHeading( float speedAdjust, float turnAdjust );
	
	void Update( float fDelta );
	void Render( UIState &ui, CApp &app );

	void NavigationPanel::RenderNavCross( UIState &ui, CApp &app, int xOffset, int yOffset );
};

extern NavigationPanel g_navigation;

#endif	//	__GUI_navigator_h__