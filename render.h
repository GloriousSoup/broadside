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
void RenderUpdate();
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
void GLUploadPV();

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

void SetTexture(GLuint textureID, int slotID);
void SetTexture(const char* pName, int slotID);

const int c_nZoomSpeed = 5;
const int c_nZoomFwd = 2;
const int c_nNoZoom = 0x7FFFFFFF;

#endif
