#ifndef _COLOUR_H_
#define _COLOUR_H_

struct Colour {
	unsigned int val;
	Colour() : val(0) {}
	Colour(const Colour &c) :val(c.val) {}
	Colour( unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255 );
	Colour( float r, float g, float b, float a = 1.0f );
	Colour( unsigned int raw ) : val( raw ) {}
};

inline unsigned int colclamp( unsigned int val ) { return val > 255 ? 255 : val; }
inline unsigned int colclampf( float val ) { return (int)( val > 255 ? 255 : val ); }
inline Colour::Colour( unsigned int r, unsigned int g, unsigned int b, unsigned int a ) {
	val = colclamp(r) + (colclamp(g)<<8) + (colclamp(b)<<16) + (colclamp(a)<<24);
}
inline Colour::Colour( float r, float g, float b, float a ) {
	val = colclampf(r*255.0f) + (colclampf(g*255.0f)<<8) + (colclampf(b*255.0f)<<16) + (colclampf(a*255.0f)<<24);
}
#endif
