#ifndef _GUI_H_
#define _GUI_H_

#include "broadside.h"
#include "optional.h"
#include "geom.h"

enum UI_STATE {
	UI_NOHIT,
	UI_HOVER,
	UI_START,
	UI_END,

	UI_UNKNOWN
};

struct UIState {
	bool m_PointerDown;
	bool m_PointerUp;
	IVec2 m_PointerPos;
	IVec2 m_PointerMove;
	IVec2 m_PointerWheel;

	E_ButtonIDs activeID;

	UIState() :
		m_PointerDown(false),
		m_PointerUp(false),
		m_PointerPos(gZeroIVec2),
		m_PointerMove(gZeroIVec2),
		m_PointerWheel(gZeroIVec2),
		activeID(en_butID_void)
	{}
};


struct SaneRect	//	AREP::TODO::Give this a less sarky name. 
{
	int x, y;
	int w, h;

	SaneRect( int _x, int _y, int _w, int _h ) : x(_x), y(_y), w(_w), h(_h) {}
};

struct Rect { // for pixel space positioning
	int left, right; // 0 is left
	int top, bottom; // 0 is top
	bool Overlaps( IVec2 p ) const {
		if( p.x < left || p.x >= right )
			return false;
		if( p.y < top || p.y >= bottom )
			return false;
		return true;
	}
	Rect( int l, int r, int t, int b ) : left(l), right(r), top(t), bottom(b) {}
	Rect() : left(0), right(0), top(0), bottom(0) {}
};

Rect Rect_From_SaneRect( SaneRect _rect );

struct Style {
	Optional<Vec4> BGColour;
	Optional<Vec4> TextColour;
	Optional<float> TextSize;
};


bool IMButton( UIState &ui, E_ButtonIDs id, const Rect &r, const TXT &text, const Style &style );

bool IMTexture( const SaneRect &r, const Style &style );
bool IMDraggable( UIState &ui, E_ButtonIDs id, Rect &r, const TXT &text, const Style &style );
bool IMScrollable( UIState &ui, E_ButtonIDs id, const Rect &r, float &scrollState, const TXTVec &text, const Style &style );

class CApp;
void UpdateGUI( float fDelta );
void DrawGUI( UIState &ui, CApp &app );


#endif
