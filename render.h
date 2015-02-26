#ifndef __RENDER_H__
#define __RENDER_H__

#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

#if defined( __MACH__ )
#elif defined( _WIN32 )
#else
#define _LINUX
#endif

#define DANGER "\033[7;40;31m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define EMPHGREEN "\033[0;37;42m"
#define BLUE "\033[0;34m"
#define CLEAR "\033[0m"

#ifdef _GLES
# import <Foundation/Foundation.h>
# import <UIKit/UIKit.h>
#include "SDL2/SDL.h"
#else

#ifdef _WIN32
#else
# define GL_GLEXT_PROTOTYPES
#include "SDL2/SDL.h"
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#if _WIN32
//#include "glhack.h"
#endif

# include <stdio.h>
# include <stdarg.h>

#endif


#include "SimpleMesh.h"

//#include <sys/types.h>
//#include <sys/stat.h>


#define ATTR_VERTEX 0
#define ATTR_NORMAL 1
#define ATTR_UV 2
#define ATTR_COLOR 3

class GLShader
{
public:
	GLuint m_program;
	GLuint projLocation, mvLocation;
	GLuint worldSpaceLightPos;
	GLuint peopleDiffuse;
	GLuint objectSpaceLightDirDiffuse;
	GLuint lightColour;
	GLuint ambientColour;
	GLuint textureLocation;
	GLuint debugTexLocation;
	GLuint influenceTexLocation;
	GLuint alphaOverrideLocation;
	GLuint timeLocation;
    GLuint dotlocation;
    GLuint dotlocation2;
    GLuint dot1Colour;
    GLuint dot2Colour;
	GLuint worldxlocation,worldylocation;
	GLuint whirlsizelocation, whirlxlocation, whirlylocation;
	GLuint gCausticsScaleLocation;
	GLuint vRoadColours;
	GLuint vOutlyingColours;
	time_t vertTime, fragTime;
	
	static GLShader* m_pCurrent;
	void Use()
	{
		if (m_pCurrent == this) return;
		glUseProgram(m_program);
		m_pCurrent = this;
	}
	static GLShader* Current() { return m_pCurrent; }
	char m_vertFilename[128];
	char m_fragFilename[128];
	void SetSources( const char *vert, const char *frag );
	void ReloadIfNecessary();
};

# define ShaderUniformExists(_s, _n) _s._n != 0xFFFFFFFF
# define UniformExists(_n) GLShader::Current()->_n != 0xFFFFFFFF
# define SetUniform1i(_n, _v) glUniform1i(GLShader::Current()->_n, _v)
# define SetUniform1f(_n, _v) glUniform1f(GLShader::Current()->_n, _v)
# define SetUniform2f(_n, _v,_v2) glUniform2f(GLShader::Current()->_n, _v,_v2)
# define SetUniform3f(_n, _v,_v2,_v3) glUniform3f(GLShader::Current()->_n, _v,_v2,_v3)
# define SetUniform4f(_n, _v,_v2,_v3,_v4) glUniform4f(GLShader::Current()->_n, _v,_v2,_v3,_v4)
# define SetUniform2fv(_n, _c, _v) glUniform2fv(GLShader::Current()->_n, _c, _v)
# define SetUniform3fv(_n, _c, _v) glUniform3fv(GLShader::Current()->_n, _c, _v)
# define SetUniform4fv(_n, _c, _v) glUniform4fv(GLShader::Current()->_n, _c, _v)
# define SetUniformMat(_n, _v) glUniformMatrix4fv(GLShader::Current()->_n, 1, false, _v)

extern GLShader DefaultShader;

void RenderInit();
void Perspective();
void Orthographic();

void LoadShader(GLShader &ShaderProgram, const char *vshad, const char *fshad );
void ReloadShader(GLShader &ShaderProgram, const char *vshad, const char *fshad );

#include "geom.h"
void GLLoadTransform(float x, float y, float z, float sx, float sy, float sz, float orientation);
void GLLoadTransform(float x, float y, float z, float scale, float orientation);
void GLSetOrtho(float left, float right, float top, float bottom, float back, float front);
void GLSetPerspective(float fov, float near, float far );
void GLSetCamera(const Mat44 &m);
void GLSetCamera(const Vec3 &pos, const Vec3 &target);
void GLSetModel(const Mat44 &m);
void GLSetModel(const Vec3 &pos, float orientation);

#define GL_PREMULTIPLIED_ALPHA 0
#define GL_INVERT_Y 1

typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned int U32;
typedef signed int S32;
typedef float F32;

extern F32 g_fScreenW, g_fScreenH;
extern GLuint GLMemoryTexture(S32 nW, S32 nH, U32* pnImage, int bCreateMipChain, int nFormat = 0);
extern const char* ConvertFilename(const char* pName);
extern const char* ConvertWriteFilename(const char* pName);
void GLDrawSprite(F32 fX, F32 fY, F32 fW, F32 fH, F32 fZ, U32 nClr, U32 nTex, U32 nFlags, F32 fUx=0,F32 fVy=0,F32 fUw=1,F32 fVh=1);
void GLDrawSprite(F32 fX, F32 fY, F32 fW, F32 fH, F32 fZ, U32 nClr, const char* pAssetName, U32 nFlags, F32 fUx=0,F32 fVy=0,F32 fUw=1,F32 fVh=1);
void GLDrawSpriteDeferStart();
void GLDrawSpriteDeferEnd(S32 nTex, U32 nFlags);
void GLDrawSpriteDeferEnd();
void GLDrawBlobNumber(F32 fX, F32 fY, F32 fSize, S32 nValue, S32 nJustify, U32 nFg, U32 nBg);
void GLDrawMesh(GLMeshList* pMeshList, int nMeshIndex, int nOverrideMaterial = 0, float* pOverrideMaterialColour = 0);

const int c_nZoomSpeed = 5;
const int c_nZoomFwd = 2;
const int c_nNoZoom = 0x7FFFFFFF;

class GLFont
{
public:
	enum
	{
		en_Flags_Shadow = 0x00000001,
		en_Flags_Relief = 0x00000002,
		en_Flags_ReliefDiag = 0x00000004,
		en_Flags_Caps = 0x00000010,
		en_Flags_SmallCaps = 0x00000020,
		en_Flags_NoneProportionalNumbers = 0x00000040,
		//
		en_Justify_Left = 0,
		en_Justify_Centre = 1,
		en_Justify_Right = 2,
	};
	static const U32 c_nSpriteFlags = 64; // we're using GL_ALPHA textures so can't do premultiplied alpha for sprites
	GLuint m_nTexture;
	int m_nTexW, m_nTexH;
	int m_nWAdj;
	short m_nData[(126-'!')*5];
	static char m_szOut[512];
	int m_bDefer;
	float m_fGlobalScale;
public:
	GLFont() { m_nTexture = 0; }
	void Init(const char* szName, F32 fScale = 1.0f) // .font file
	{
		//m_nTexture = GLLoadImageTexture(GLResourceName(szName, @"png"), &m_nTexW, &m_nTexH);
		m_fGlobalScale = fScale;
		//printf("Font <%s> - <%s>\n", GLResourceName(szName, @"font"), [szName UTF8String]);
		FILE* f = fopen(ConvertFilename(szName), "rb");
		if (f == NULL) return;
		fread(m_nData, 1, (126-'!')*5*2, f);
		fread(&m_nTexW, 1, 4, f);
		fread(&m_nTexH, 1, 4, f);
		S32 bOutlineFonts = 1;
		U32* pnData;
		m_nWAdj = 0;
		int i;
		if (bOutlineFonts)
		{
			m_nWAdj = -2*2;
			pnData = new U32[m_nTexW*m_nTexH/2];
			for (i = 0; i < m_nTexW*m_nTexH; i ++)
			{
				U8 nAlpha = fgetc(f);
				((U8*)pnData)[i*2+0] = nAlpha;
				((U8*)pnData)[i*2+1] = nAlpha;
			}
			S32 x, y;
			S32 nDir[8*2] = { -1,-1, 0,-1, 1,-1, -1,0, 1,0, -1,1, 0,1, 1,1 };
			S32 nB = 3;
			for (y = 0; y < m_nTexH; y ++)
				for (x = 0; x < m_nTexW; x ++)
				{
					S32 nFrom = 0, nTo = 1-nFrom;
					U8 nAlpha = ((U8*)pnData)[(y*m_nTexW+x)*2+nFrom];
					U8 n;
					S32 nD, nR;
					for (nR = 0; nR < nB; nR ++)
						for (nD = 0; nD < 8; nD ++)
						{
							S32 nDX = x + nDir[nD*2+0] * nR;
							S32 nDY = y + nDir[nD*2+1] * nR;
							if (nDX >= 0 && nDX < m_nTexW && nDY >= 0 && nDY < m_nTexH)
							{
								n = ((U8*)pnData)[(nDY*m_nTexW+nDX)*2+nFrom];
								if (n > nAlpha) nAlpha = n;
							}
						}
					((U8*)pnData)[(y*m_nTexW+x)*2+nTo] = nAlpha;
				}
		}
		else
		{
			pnData = new U32[m_nTexW*m_nTexH/4];
			for (i = 0; i < m_nTexW*m_nTexH; i ++)
				((U8*)pnData)[i] = fgetc(f);
		}
#if !GL_INVERT_Y
		void GLImageFlipY(S32& nW, S32& nH, U32* pnImage);
		GLImageFlipY(m_nTexW/4, m_nTexH, pnData);
#endif
		m_nTexture = GLMemoryTexture(m_nTexW, m_nTexH, pnData, 0, 2-bOutlineFonts);
		fclose(f);
		delete[] pnData;
		m_bDefer = 0;
	}
	float DrawChar(int nChar, float fX, float fY, unsigned int nClr, unsigned int nFlags, int nZoomOn, float fScale)
	{
		fScale *= m_fGlobalScale;
		if (nChar == '`') return 0;
		if (nChar == ' ') return m_nData[0*5+1] * 4 / 2 * fScale;
		
		int nCharForSize = nChar;
		float fFullScale = fScale;
		if ((nFlags & en_Flags_Caps) && nChar >= 'a' && nChar <= 'z') { nChar += 'A'-'a'; }
		if ((nFlags & en_Flags_SmallCaps) && nChar >= 'a' && nChar <= 'z') { nChar += 'A'-'a'; fScale *= 0.9f; }
		if ((nFlags & en_Flags_NoneProportionalNumbers) && nChar >= '0' && nChar <= '9') nCharForSize = '0';
		
		nChar -= '!';
		nCharForSize -= '!';
		int nX = m_nData[nChar*5+0];
		int nW = m_nData[nChar*5+1];
		int nY = m_nData[nChar*5+2];
		int nH = m_nData[nChar*5+3];
		int nDY = m_nData[nChar*5+4];
		if (nW < 0) return m_nData[0*5+1] * 4 / 2 * fScale;
		int nTW = m_nTexW;
		int nTH = m_nTexH;
		float fITW = 1.0f / (float)nTW, fITH = 1.0f / (float)nTH;
		float fU = (float)nX * fITW, fV = (float)nY * fITH;
		float fDU = ((float)nW+0.5f) * fITW, fDV = ((float)nH + 0.5f) * fITH;
		fV = 1.0f - fV; fDV = -fDV;
		fY += (float)nDY * fScale;
		if (fScale != fFullScale) { float fTotalY = (float)(nDY+nH); fY += fTotalY * (1.0f - fScale/fFullScale); }
		float fW = ((float)nW + 0.5f) * fScale, fH = ((float)nH + 0.5f) * fScale;
		float fMargin = 0.0f;
		if (nChar != nCharForSize)
		{
			F32 fWMargin = ((float)m_nData[nCharForSize*5+1] + 0.5f) * fScale;
			fMargin = (fWMargin - fW - m_nWAdj*fScale) * 0.5f;
			fX += fMargin;
		}
		if (nZoomOn != 256)
		{
			float fZoom = (float)nZoomOn * (1.0f/256.0f);
			fX += fW * (1-fZoom) * 0.5f;
			fY += fH * (1-fZoom) * 0.5f;
			fW *= fZoom; fH *= fZoom;
		}
		float fD = 1.1f * 320.0f / g_fScreenW;//1.2f;//1.4f * fScale;//g_fScreenW * (1.0f/320.0f);
		if (nFlags & en_Flags_Shadow)
			GLDrawSprite(fX+fD, fY+fD, fW, fH, 0.0f, nClr&0xFF000000, m_nTexture, c_nSpriteFlags, fU,fV,fDU,fDV);
		else if (nFlags & en_Flags_Relief)
		{
			GLDrawSprite(fX, fY-fD, fW, fH, 0.0f, nClr&0xFF000000, m_nTexture, c_nSpriteFlags, fU,fV,fDU,fDV);
			GLDrawSprite(fX, fY+fD, fW, fH, 0.0f, nClr|0xFFFFFF, m_nTexture, c_nSpriteFlags, fU,fV,fDU,fDV);
		}
		else if (nFlags & en_Flags_ReliefDiag)
		{
			GLDrawSprite(fX-fD, fY-fD, fW, fH, 0.0f, nClr&0xFF000000, m_nTexture, c_nSpriteFlags, fU,fV,fDU,fDV);
			GLDrawSprite(fX+fD, fY+fD, fW, fH, 0.0f, nClr|0xFFFFFF, m_nTexture, c_nSpriteFlags, fU,fV,fDU,fDV);
		}
		GLDrawSprite(fX, fY, fW, fH, 0.0f, nClr, m_nTexture, c_nSpriteFlags, fU,fV,fDU,fDV);
		return fW+m_nWAdj*fScale+1.0f+fMargin*2;
	}
	float SizeChar(int nChar, int nFlags, float fScale)
	{
		fScale *= m_fGlobalScale;
		if (nChar == ' ') return m_nData[0*5+1] * 4 / 2 * fScale;
		if ((nFlags & en_Flags_Caps) && nChar >= 'a' && nChar <= 'z') { nChar += 'A'-'a'; }
		if ((nFlags & en_Flags_SmallCaps) && nChar >= 'a' && nChar <= 'z') { nChar += 'A'-'a'; fScale *= 0.9f; }
		if ((nFlags & en_Flags_NoneProportionalNumbers) && nChar >= '0' && nChar <= '9') nChar = '0';
		nChar -= '!';
		int nW = m_nData[nChar*5+1] + m_nWAdj;
		if (nW < 0) return m_nData[0*5+1] * 4 / 2 * fScale;
		return nW * fScale + 1;
	}
	float SizeString(float fScale, int nFlags, int nMaxChar, const char* szText)
	{
		int i;
		float fSize = 0;
		if (nMaxChar == -1) nMaxChar = 0x7FFFFFFF;
		for (i = 0; i < nMaxChar && szText[i]; i ++)
			fSize += SizeChar(szText[i], nFlags, fScale);
		return fSize;
	}
	void BeginDefer()
	{
		m_bDefer = 1;
		GLDrawSpriteDeferStart();
	}
	void EndDefer()
	{
		m_bDefer = 0;
		GLDrawSpriteDeferEnd(m_nTexture, 0);
	}
	float DrawString(float fX, float fY, float fScale, unsigned int nClr, unsigned int nFlags, int nCounter, int nMaxChar, const char* szText)
	{
		int i;
		if (nMaxChar == -1) nMaxChar = 0x7FFFFFFF;
		if (nCounter == -1) nCounter = 0x7FFFFFFF;
		if (!m_bDefer)
			GLDrawSpriteDeferStart();
		for (i = 0; i < nMaxChar && szText[i]; i ++)
		{
			int nZoomOn;
			if (nCounter > i*c_nZoomFwd) nZoomOn = 256;
			else if (nCounter < i*c_nZoomFwd-c_nZoomSpeed) nZoomOn = 0;
			else nZoomOn = (nCounter - (i*c_nZoomFwd-c_nZoomSpeed)) * 256 / c_nZoomSpeed;
			fX += DrawChar(szText[i], fX,fY, nClr, nFlags, nZoomOn, fScale);
		}
		if (!m_bDefer)
			GLDrawSpriteDeferEnd(m_nTexture, c_nSpriteFlags);
		return fX;
	}
	float DrawString(float fX, float fY, float fW, float fScale, int nJustify, unsigned int nClr, unsigned int nFlags, int nCounter, int nMaxChar, const char* szText)
	{
		float fLen = SizeString(fScale, nFlags, nMaxChar, szText);
		fX += (fW - fLen) * (float)nJustify * 0.5f;
		return DrawString(fX, fY, fScale, nClr, nFlags, nCounter, nMaxChar, szText);
	}
	float DrawStringf(float fX, float fY, float fScale, unsigned int nClr, unsigned int nFlags, int nCounter, const char* szText, ...)
	{
		va_list arglist;
		va_start(arglist, szText);
		vsprintf(m_szOut, szText, arglist);
		va_end(arglist);
		return DrawString(fX, fY, fScale, nClr, nFlags, nCounter, -1, m_szOut);
	}
	float DrawStringf(float fX, float fY, float fW, float fScale, int nJustify, unsigned int nClr, unsigned int nFlags, int nCounter, int nMaxChar, const char* szText, ...)
	{
		va_list arglist;
		va_start(arglist, szText);
		vsprintf(m_szOut, szText, arglist);
		va_end(arglist);
		return DrawString(fX, fY, fW, fScale, nJustify, nClr, nFlags, nCounter, nMaxChar, m_szOut);
	}
};

#endif
