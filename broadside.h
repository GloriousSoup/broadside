#ifndef _BROADSIDE_H_
#define _BROADSIDE_H_

#include <string>
#include <map>
#include <vector>

typedef std::string TXT;
typedef std::vector<TXT> TXTVec;

enum E_ButtonIDs
{
	en_butID_void,
	en_butID_navLeft,
	en_butID_navRight,
	en_butID_navFaster,
	en_butID_navSlower,

	
	en_butID_testScrollbar,
	en_butID_testScrollbarShow,
	en_butID_testLeft, 
	en_butID_testRight, 


	en_butID_testDragA, 
	en_butID_testDragB, 
	en_butID_testSidebar,
};

#endif

