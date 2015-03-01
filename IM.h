#ifndef _IM_H_
#define _IM_H_

#include "broadside.h"
#include "optional.h"

enum UI_STATE {
	UI_NOHIT,
	UI_HOVER,
	UI_START,
	UI_END,

	UI_UNKNOWN
};

struct UIContext {
	int activeID;
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
};

struct Style {
	Optional<Vec4> BGColour;
	Optional<Vec4> TextColour;
	Optional<float> TextSize;
};

struct ButtonRenderData {
	Rect r;
	TXT text;
	Style s;
	ButtonRenderData( const Rect &_r, const TXT &_text, const Style &_s ) : r(_r), text(_text), s(_s) {}
};

#endif
