#include "GUI_Navigator.h"


NavigationPanel g_navigation;


//	AREP::TODO::Config these.
float c_navigation_accelModifier = 30.0f;
float c_navigation_brakeModifier = 30.0f;
float c_navigation_turnModifier = 30.0f;

float c_navigation_maxSpeed = 10.0f;
float c_navigation_maxBrake = -5.0f;
float c_navigation_maxTurn = 7.0f;


void NavigationPanel::Update( float fDelta )
{
	if( currentSpeed != targetSpeed ) 
	{
		if( currentSpeed < targetSpeed )
			currentSpeed = std::min( targetSpeed, currentSpeed + (fDelta * c_navigation_accelModifier) );
		else
			currentSpeed = std::max( targetSpeed, currentSpeed - (fDelta * c_navigation_brakeModifier) );
	}

	if( currentTurn != targetTurn )
	{
		if( currentTurn < targetTurn )
			currentTurn = std::min( targetTurn, currentTurn + (fDelta * c_navigation_turnModifier) );
		else
			currentTurn = std::max( targetTurn, currentTurn - (fDelta * c_navigation_turnModifier) );
	}
}

void NavigationPanel::AdjustHeading( float speedAdjust, float turnAdjust )
{
	targetSpeed = std::min( c_navigation_maxSpeed, std::max( c_navigation_maxBrake, targetSpeed + speedAdjust ) );
	targetTurn = std::min( c_navigation_maxTurn, std::max( -c_navigation_maxTurn, targetTurn + turnAdjust ) );
}


int c_nav_crossL = 100;
int c_nav_crossW = 24;
int c_nav_pipW = 20;

int c_nav_buttonW = 16;
int c_nav_buttonOff = 20;

void NavigationPanel::RenderNavCross( UIState &ui, CApp &app, int xOffset, int yOffset )
{
	Style bgStyle;
	bgStyle.BGColour = Vec4( 0.6f, 0.6f, 0.6f, 1.0f );

	SaneRect crosshairWidth( xOffset - (c_nav_crossL/2), yOffset - (c_nav_crossW/2), c_nav_crossL, c_nav_crossW );
	IMTexture( crosshairWidth, bgStyle );

	SaneRect crosshairHeight( xOffset - (c_nav_crossW/2), yOffset - (c_nav_crossL/2), c_nav_crossW, c_nav_crossL );
	IMTexture( crosshairHeight, bgStyle );

	Style pipStyle;
	pipStyle.BGColour = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	
	

	float turnPcnt = currentTurn / c_navigation_maxTurn;
	float speedPcnt = currentSpeed > 0.0f ? currentSpeed / c_navigation_maxSpeed : currentSpeed / -c_navigation_maxBrake ;

	SaneRect turnPip( xOffset + ( turnPcnt * ((c_nav_crossL-c_nav_pipW)/2) ) - (c_nav_pipW/2), yOffset - (c_nav_pipW/2), c_nav_pipW, c_nav_pipW );
	SaneRect speedPip( xOffset - (c_nav_pipW/2), yOffset + ( -speedPcnt * ((c_nav_crossL-c_nav_pipW)/2) ) - (c_nav_pipW/2), c_nav_pipW, c_nav_pipW );

	IMTexture( turnPip, pipStyle );
	IMTexture( speedPip, pipStyle );


	
	Rect buttonRect;
	Style buttonStyle;
	buttonStyle.BGColour = Vec4( 0.6f, 1.0f, 0.6f, 1.0f );

	buttonRect.left = xOffset - (c_nav_crossL/2) - c_nav_buttonOff;
	buttonRect.top = yOffset - (c_nav_buttonW/2);
	buttonRect.right = buttonRect.left + c_nav_buttonW;
	buttonRect.bottom = buttonRect.top + c_nav_buttonW;		
	if( IMButton( ui, en_butID_navLeft, buttonRect, "", buttonStyle ) ) 
		AdjustHeading( 0.0f, -c_navigation_maxTurn/3 );
	

	buttonRect.left = xOffset + (c_nav_crossL/2) + c_nav_buttonOff - c_nav_buttonW;
	buttonRect.top = yOffset - (c_nav_buttonW/2);
	buttonRect.right = buttonRect.left + c_nav_buttonW;
	buttonRect.bottom = buttonRect.top + c_nav_buttonW;
	if( IMButton( ui, en_butID_navRight, buttonRect, "", buttonStyle ) ) 
		AdjustHeading( 0.0f, +c_navigation_maxTurn/3 );
	

	buttonRect.left = xOffset - (c_nav_buttonW/2);
	buttonRect.top = yOffset - (c_nav_crossL/2) - c_nav_buttonOff;
	buttonRect.right = buttonRect.left + c_nav_buttonW;
	buttonRect.bottom = buttonRect.top + c_nav_buttonW;
	if( IMButton( ui, en_butID_navFaster, buttonRect, "", buttonStyle ) ) 
		AdjustHeading( c_navigation_maxSpeed/3, 0.0f );
	
	
	buttonRect.left = xOffset - (c_nav_buttonW/2);
	buttonRect.top = yOffset + (c_nav_crossL/2) + c_nav_buttonOff - c_nav_buttonW;
	buttonRect.right = buttonRect.left + c_nav_buttonW;
	buttonRect.bottom = buttonRect.top + c_nav_buttonW;
	if( IMButton( ui, en_butID_navSlower, buttonRect, "", buttonStyle ) ) 
		AdjustHeading( c_navigation_maxSpeed/3, 0.0f );
}

void NavigationPanel::Render( UIState &ui, CApp &app )
{		
	//	AREP::TODO::Figure out anchoring
	RenderNavCross( ui, app, 700, 360 );
}

