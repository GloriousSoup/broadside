#include "GUI_Tools.h"


namespace Elastic
{

	float easeIn (float t) 
	{
		if (t<=0.0f) 
			return 0.0f; 
		
		if((t/=1.0f)>=1)
			return 1.0f;

		float p=.3f;
		float s=p/4;
	
		float postFix = pow(2,10*(t-=1)); // this is a fix, again, with post-increment operators
		return -(postFix * sin((t*1.0f-s)*(2*M_PI)/p ));
	}

	float easeOut(float t) 
	{
		if (t<=0.0f) 
			return 0.0f; 
		
		if((t/=1.0f)>=1)
			return 1.0f;

		float p=.3f;
		float s=p/4;

		return (pow(2,-10*t) * sin( (t*1.0f-s)*(2*M_PI)/p ) + 1.0f);
	}

	float easeInOut(float t) 
	{
		if (t<=0.0f) 
			return 0.0f; 
		
		if((t/=0.5f)>=2.0f)
			return 1.0f;
		
		float p=(.3f*1.5f);
		float s=p/4;

		if (t < 1) 
		{
			float postFix =pow(2,10*(t-=1)); // postIncrement is evil
			return -.5f*(postFix* sin( (t*1.0f-s)*(2*M_PI)/p ));
		}

		float postFix = pow(2,-10*(t-=1)); // postIncrement is evil
		return postFix * sin( (t*1.0f-s)*(2*M_PI)/p )*.5f + 1.0f;
	}
}


namespace Quart
{
	float easeIn (float t) 
	{
		return t*t*t*t;
	}

	float easeOut(float t) 
	{
		return -1 * ((t=t-1)*t*t*t - 1);
	}

	float easeInOut(float t) 
	{
		if ((t/=0.5f) < 1) 
			return 0.5f*t*t*t*t;
		else
			return -0.5f * ((t-=2)*t*t*t - 2);
	}
}

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
