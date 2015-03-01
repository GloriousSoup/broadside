#include "gui.h"
#include "CApp.h"
#include "FontRender.h"

struct ButtonRenderData {
	Rect r;
	TXT text;
	Style s;
	ButtonRenderData( const Rect &_r, const TXT &_text, const Style &_s ) : r(_r), text(_text), s(_s) {}
};

struct TextRenderData {
	IVec2 p;
	TXT text;
	Style s;
	TextRenderData( const IVec2 &_p, const TXT &_text, const Style &_s ) : p(_p), text(_text), s(_s) {}
};


typedef std::vector<ButtonRenderData> ButtonVec;
typedef std::vector<TextRenderData> TextVec;
ButtonVec ButtonsToRender;
TextVec TextToRender;

bool IMButton( UIState &ui, int id, const Rect &r, const std::string &text, const Style &style ) {
	ButtonsToRender.push_back( ButtonRenderData( r, text, style ) );
	bool overlap = r.Overlaps( ui.m_PointerPos );
	if( ui.m_PointerDown && overlap ) {
		ui.activeID = id;
	}
	if( ui.m_PointerUp ) {
		if( ui.activeID == id ) {
			ui.activeID = -1;
			return overlap;
		}
	}
	return false;
}
bool IMDraggable( UIState &ui, int id, Rect &r, const std::string &text, const Style &style ) {
	ButtonsToRender.push_back( ButtonRenderData( r, text, style ) );
	if( r.Overlaps( ui.m_PointerPos ) ) {
		if( ui.m_PointerDown ) {
			ui.activeID = id;
		}
	}
	if( ui.m_PointerUp ) {
		if( ui.activeID == id ) {
			ui.activeID = -1;
		}
	}
	if( ui.activeID == id ) {
		r.left += ui.m_PointerMove.x;
		r.right += ui.m_PointerMove.x;
		r.top += ui.m_PointerMove.y;
		r.bottom += ui.m_PointerMove.y;
	}
	return false;
}
bool IMScrollable( UIState &ui, int id, const Rect &r, float &scrollState, const TXTVec &text, const Style &style ) {
	const float lineHeight = 8.0f;
	ButtonsToRender.push_back( ButtonRenderData( r, "", style ) );
	for( size_t i = 0; i < text.size(); ++i ) {
		float topPos = - scrollState + i * lineHeight;
		float bottomPos = - scrollState + i * lineHeight + lineHeight;
		if( topPos >= 0.0f && bottomPos < r.bottom - r.top ) {
			TextToRender.push_back( TextRenderData( IVec2( r.left, r.top + topPos ), text[i], style ) );
		}
	}
	const float endScroll = lineHeight * text.size() - (r.bottom - r.top - 1);
	const float maxScroll = endScroll > 0.0f ? endScroll : 0.0f;

	if( r.Overlaps( ui.m_PointerPos ) ) {
		if( ui.m_PointerDown ) {
			ui.activeID = id;
		}
		if( ui.activeID == -1 ) {
			scrollState -= ui.m_PointerWheel.y * 4.0f;
		}
	}
	if( ui.m_PointerUp ) {
		if( ui.activeID == id ) {
			ui.activeID = -1;
		}
	}
	if( ui.activeID == id ) {
		scrollState -= ui.m_PointerMove.y;
	}
	if( scrollState < 0.0f )
		scrollState = 0.0f;
	if( scrollState > maxScroll )
		scrollState = maxScroll;
	return false;
}

void DrawGUI( UIState &ui, CApp &app ) {
	for( ButtonVec::iterator i = ButtonsToRender.begin(); i != ButtonsToRender.end(); ++i ) {
		Rect r = i->r;
		Vec4 c = Vec4(1.0f);
		if( i->s.BGColour ) c = i->s.BGColour;
		GLSetModel(gIdentityMat);
		app.DrawRect( r.left, r.top, r.right - r.left, r.bottom - r.top, c );
		Mat44 m = gIdentityMat;
		m.w.x = (float)r.left;
		m.w.y = (float)r.top;

		FontPrint( m, i->text.c_str(), i->s.TextColour );
	}
	ButtonsToRender.clear();
	for( TextVec::iterator i = TextToRender.begin(); i != TextToRender.end(); ++i ) {
		IVec2 p = i->p;
		Vec4 c = Vec4(1.0f);
		Mat44 m = gIdentityMat;
		m.w.x = (float)p.x;
		m.w.y = (float)p.y;

		FontPrint( m, i->text.c_str(), i->s.TextColour );
	}
	TextToRender.clear();

	ui.m_PointerDown = false;
	ui.m_PointerUp = false;
	ui.m_PointerMove = gZeroIVec2;
	ui.m_PointerWheel = gZeroIVec2;
}
