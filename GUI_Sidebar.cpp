#include "GUI_Sidebar.h"


SidePanel testSidePanel;


//	AREP::TODO::Config these.
int c_sidepanel_w = 200;
int c_sidepanel_button_y = 80;
int c_sidepanel_button_w = 60;
int c_sidepanel_button_h = 120;


void SidePanel::Update( float fDelta )
{
	coreVis.Update( isOpen, fDelta );
}

void SidePanel::Render( UIState &ui, CApp &app )
{	
	Interpolator bgInterpolator;
		
	bgInterpolator.coords.left = -c_sidepanel_w;
	bgInterpolator.coords.right = bgInterpolator.coords.left + c_sidepanel_w;
	bgInterpolator.coords.top = 0;
	bgInterpolator.coords.bottom = app.m_screenH;
	
	bgInterpolator.style.BGColour = Vec4( 0.2f, 0.2f, 0.2f, 0.8f );

	bgInterpolator.Clear();
	bgInterpolator.Ease( Interpolator::en_style_slideOn, coreVis.GetPcnt(), c_sidepanel_w );
	bgInterpolator.Ease( Interpolator::en_style_fade, coreVis.GetPcnt() );

	app.DrawRect( bgInterpolator.interp_coords, bgInterpolator.interp_style.BGColour );

	Rect showButton;
	showButton.left = bgInterpolator.interp_coords.right;
	showButton.right = showButton.left + c_sidepanel_button_w;
	showButton.top = c_sidepanel_button_y;
	showButton.bottom = showButton.top + c_sidepanel_button_h;
	
	Style s;
	s.BGColour = Vec4( 0.6f, 0.6f, 0.6f, 1.0f );

	if( IMButton( ui, 69, showButton, "", s ) ) 
	{
		isOpen= !isOpen;
	}
}



/*


struct SaneRect
{
	int x, y;
	int w, h;

	Rect( int _x, int _y, int _w, int _h ) : x(_x), y(_y), w(_w), h(_h) {}
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

SaneRect SaneRect_From_Rect( Rect rect )
{
	return SaneRect( rect.left, rect.top, 
}

Rect Rect_From_SaneRect( SaneRect rect )
{

}
*/