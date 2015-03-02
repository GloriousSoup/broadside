#ifndef __GUI_sidebar_h__
#define __GUI_sidebar_h__

#include "CApp.h"
#include "GUI_Tools.h"

//	Getting started. This will turn into a singleton probably. 
struct SidePanel
{
	bool isOpen;
	InterpBool coreVis;

	void Update( float fDelta );
	void Render( UIState &ui, CApp &app );
};

extern SidePanel testSidePanel;

#endif	//	__GUI_sidebar_h__