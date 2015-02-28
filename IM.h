#ifndef _IM_H_
#define _IM_H_

#include <string>

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

template <typename T>
struct Optional {
	T val;
	bool valid;
	operator T () const { return val; }
	operator bool () const { return valid; }
	Optional() : valid( false ) {}
	Optional( T v ) : val( v ), valid( true ) {}
	Optional( const Optional &v ) : val( v.val ), valid( v.valid ) {}
};
struct Style {
	Optional<Vec4> BGColour;
	Optional<Vec4> TextColour;
	Optional<float> TextSize;
};

struct ButtonRenderData {
	Rect r;
	std::string text;
	Style s;
	ButtonRenderData( const Rect &_r, const std::string &_text, const Style &_s ) : r(_r), text(_text), s(_s) {}
};

#endif
