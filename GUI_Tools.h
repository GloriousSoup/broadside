#ifndef __GUI_tools_h__
#define __GUI_tools_h__

#include <algorithm>

#include "SDL2/SDL.h"
#include "gui.h"

struct InterpBool
{	
	void Set( bool _isTrue )
	{
		isTrue = _isTrue;
		truePcnt = isTrue ? 1.0f : 0.0f;
	}

	void Update( bool _isTrue, float fDelta, float speed = 1.0f )
	{
		isTrue = _isTrue;
		if( isTrue )
			truePcnt = std::min( 1.0f, truePcnt + fDelta*speed );
		else
			truePcnt = std::max( 0.0f, truePcnt - fDelta*speed );
	}

	bool IsFull() { return truePcnt >= 1.0f; }
	bool IsAny() { return truePcnt > 0.0f; };
	bool IsEmpty() { return truePcnt <= 0.0f; }

	bool GetBool() { return isTrue; }
	float GetPcnt() { return truePcnt; }
	
private:
	bool isTrue;
	float truePcnt;
};


struct Interpolator
{
	enum E_Style
	{
		en_style_void,
		en_style_fade,
		en_style_slideOn,
	};

	Rect coords, interp_coords;
	Style style, interp_style;

	void Clear();
	void Ease( E_Style eStyle, float pcnt, float arg1 = 0.0f, float arg2 = 0.0f);
};


#endif	//	__GUI_tools_h__