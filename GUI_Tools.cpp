#include "GUI_Tools.h"
#include "GUI_EasingCurves.h"







void Fade( float pcnt, Style &style )
{
	style.BGColour.val.w = style.BGColour.val.w * Quart::easeIn( pcnt );	
}

void Shift( float pcnt, Rect &rect, float adjustment )
{
	float newPcnt = Elastic::easeInOut( pcnt );
	rect.left += (int)(newPcnt * adjustment);
	rect.right += (int)(newPcnt * adjustment);
}

void Interpolator::Clear()
{
	interp_coords = coords;
	interp_style = style;
}

void Interpolator::Ease( E_Style eStyle, float pcnt, float arg1, float arg2 )
{
	switch( eStyle )
	{
	default:	break;
	case en_style_fade:		Fade( pcnt, interp_style );	break;
	case en_style_slideOn:	Shift( pcnt, interp_coords, arg1);	break;
	}
}
