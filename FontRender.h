#ifndef _FONT_RENDER_H_
#define _FONT_RENDER_H_

#include "optional.h"

void FontRenderInit();
void FontRenderShutdown();
void FontPrint( const Mat44 &basis, const char *string, const Optional<Vec4> &colour );
void FontUpdate();

void SetFontTexture( int character );

#endif
