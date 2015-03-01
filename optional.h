#ifndef _OPTIONAL_H_
#define _OPTIONAL_H_

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

#endif

