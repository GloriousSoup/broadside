#include "render.h"
#include "util.h"
#include "geom.h"
#include "FontRender.h"

#ifndef _WIN32
#include "unistd.h"
#endif

#define CATCH_GL_ERROR( WHAT ) do { if( int error = glGetError() ) printf( "%s %i %s (%i)\n", __FILE__, __LINE__, WHAT, error ); } while(0)

#include <math.h>

void DrawMesh(float x, float y, float z, float orientation, float scale, int nSubAnim, GLMeshList* pList);
void DrawStringAtPoint(const Vec3& v, float fScale, U32 nClr, const char* sz, ...);

GLShader* GLShader::m_pCurrent = NULL;
GLShader DefaultShader;

#ifdef _GLES
# define ATTRIB_PRE "m"
#else
# define ATTRIB_PRE "m"
#endif
void BindAttributes(GLShader& ShaderProgram);
void FindUniforms(GLShader &ShaderProgram);
extern time_t get_mtime(const char *path, time_t default_value );

const char* g_pVertexShaderPrefixBaseES = "\
#version 100\n\
attribute vec4 mgl_Vertex;\n\
attribute vec3 mgl_Normal;\n\
attribute vec4 mgl_Color;\n\
attribute vec2 mgl_MultiTexCoord0;\n\
uniform mat4 mgl_ProjectionMatrix;\n\
uniform mat4 mgl_ModelViewMatrix;\n\
#define gl_Vertex mgl_Vertex\n\
#define gl_Normal mgl_Normal\n\
#define gl_Color mgl_Color\n\
#define gl_MultiTexCoord0 mgl_MultiTexCoord0\n\
#define gl_ProjectionMatrix mgl_ProjectionMatrix\n\
#define gl_ModelViewMatrix mgl_ModelViewMatrix\n";
const char* g_pFragmentShaderPrefixMedBaseES = "\
#version 100\n\
precision mediump float;\n\
precision mediump int;\n";
const char* g_pFragmentShaderPrefixBaseES = "\
#version 100\n\
precision highp float;\n\
precision mediump int;\n";

const char* g_pVertexShaderPrefixBaseDesk = "\
#version 120\n\
attribute vec4 mgl_Vertex;\n\
attribute vec3 mgl_Normal;\n\
attribute vec4 mgl_Color;\n\
attribute vec2 mgl_MultiTexCoord0;\n\
uniform mat4 mgl_ProjectionMatrix;\n\
uniform mat4 mgl_ModelViewMatrix;\n\
#define gl_Vertex mgl_Vertex\n\
#define gl_Normal mgl_Normal\n\
#define gl_Color mgl_Color\n\
#define gl_MultiTexCoord0 mgl_MultiTexCoord0\n\
#define gl_ProjectionMatrix mgl_ProjectionMatrix\n\
#define gl_ModelViewMatrix mgl_ModelViewMatrix\n";
const char* g_pFragmentShaderPrefixBaseDesk = "\
#version 120\n";

char g_pVertexShaderPrefix[1024];
char g_pFragmentShaderPrefix[1024];
void PrepareShaderPrefixes() {
#ifdef _GLES
		strcpy(g_pVertexShaderPrefix, g_pVertexShaderPrefixBaseES);
		strcpy(g_pFragmentShaderPrefix, g_pFragmentShaderPrefixBaseES);
#else
		strcpy(g_pVertexShaderPrefix, g_pVertexShaderPrefixBaseDesk);
		strcpy(g_pFragmentShaderPrefix, g_pFragmentShaderPrefixBaseDesk);
#endif
}
bool ReadAndCompileShader(GLuint &ShaderProgram, const char *filename, GLuint &shader, GLuint shaderType ) {
	static char pShaderText[8192];
	memset( pShaderText, 0, sizeof( pShaderText ) );
	const char* pPrefix = g_pVertexShaderPrefix;
	if (shaderType == GL_FRAGMENT_SHADER) pPrefix = g_pFragmentShaderPrefix;
	strcpy(pShaderText, pPrefix);
	size_t nPrefixLen = strlen(pPrefix);
	
	const char* pOldFilename = filename;
	//filename = ConvertFilename(filename);
	//printf("Load shader %s\n", filename);
	FILE *fp = fopen(filename, "r" );
	if( fp ) {
		size_t bytesRead = fread(pShaderText+nPrefixLen,1,sizeof(pShaderText)-nPrefixLen,fp);
		fclose(fp);
		if( sizeof(pShaderText) == bytesRead ) {
			printf( "failed to read all the shader text for %s\n", pOldFilename );
			return false;
		}
	} else {
		printf( "Failed to load shader %s\n", pOldFilename );
		return false;
	}
	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0]= (int)strlen(pShaderText);
	glShaderSource(shader, 1, p, Lengths);
	glCompileShader(shader);
	
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader %s, type %d: '%s'\n", pOldFilename, shaderType, InfoLog);
		return false;
	}
	glAttachShader(ShaderProgram, shader);
	return true;
}

bool LinkShaders(GLuint &ShaderProgram) {
	GLint Success;
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		return false;
	}
	return true;
}
void LoadShader(GLShader &ShaderProgram, const char *vshad, const char *fshad ) {
	ShaderProgram.fragTime = get_mtime( fshad, ShaderProgram.fragTime );
	ShaderProgram.vertTime = get_mtime( vshad, ShaderProgram.vertTime );

	ShaderProgram.m_program = glCreateProgram();
	BindAttributes(ShaderProgram);
	GLuint VertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if( !ReadAndCompileShader(ShaderProgram.m_program, vshad, VertShader, GL_VERTEX_SHADER ) )
		exit(1);
	if( !ReadAndCompileShader(ShaderProgram.m_program, fshad, FragmentShader, GL_FRAGMENT_SHADER ) )
		exit(1);
	if( !LinkShaders(ShaderProgram.m_program) )
		exit(1);
	FindUniforms(ShaderProgram);
	glValidateProgram(ShaderProgram.m_program);
	//glUseProgram(ShaderProgram.m_program);
	//printf("Attribs: v(%d) n(%d) c(%d) u(%d)\n", glGetAttribLocation(ShaderProgram.m_program, ATTRIB_PRE"gl_Vertex"),glGetAttribLocation(ShaderProgram.m_program, ATTRIB_PRE"gl_Normal"),glGetAttribLocation(ShaderProgram.m_program, ATTRIB_PRE"gl_Color"),glGetAttribLocation(ShaderProgram.m_program, ATTRIB_PRE"gl_MultiTexCoord0"));
}
void ReloadShader(GLShader &ShaderProgram, const char *vshad, const char *fshad ) {
	bool reload = false;
	if( ShaderProgram.fragTime < get_mtime( fshad, ShaderProgram.fragTime ) ) {
		reload = true;
	}
	if( ShaderProgram.vertTime < get_mtime( vshad, ShaderProgram.vertTime ) ) {
		reload = true;
	}

	if( reload ) {
		LoadShader(ShaderProgram, vshad, fshad);
	}
}

void GLShader::SetSources( const char *vert, const char *frag ) {
	strcpy( m_vertFilename, vert );
	strcpy( m_fragFilename, frag );
	LoadShader( *this, m_vertFilename, m_fragFilename );
}
void GLShader::ReloadIfNecessary() {
	ReloadShader( *this, m_vertFilename, m_fragFilename );
}

GLuint GetUniformLocation(GLShader& ShaderProgram, const char* pName)
{
	GLuint n = glGetUniformLocation(ShaderProgram.m_program, pName);
	//printf("GetUniLoc %d %s - %d\n", ShaderProgram.m_program, pName, n);
	return n;
}
void FindUniforms(GLShader &ShaderProgram) {
	ShaderProgram.timeLocation = GetUniformLocation(ShaderProgram, "gTime");
	ShaderProgram.worldxlocation = GetUniformLocation(ShaderProgram, "gWorldx");
	ShaderProgram.worldylocation = GetUniformLocation(ShaderProgram, "gWorldy");
	ShaderProgram.whirlsizelocation = GetUniformLocation(ShaderProgram, "gWhirlSize");
	ShaderProgram.whirlxlocation = GetUniformLocation(ShaderProgram, "gWhirlx");
	ShaderProgram.whirlylocation = GetUniformLocation(ShaderProgram, "gWhirly");
	ShaderProgram.textureLocation = GetUniformLocation(ShaderProgram, "gSampler");
	ShaderProgram.debugTexLocation = GetUniformLocation(ShaderProgram, "gDebugSampler");
	
	ShaderProgram.dotlocation = GetUniformLocation(ShaderProgram, "gDot");
	ShaderProgram.dotlocation2 = GetUniformLocation(ShaderProgram, "gDot2");
	ShaderProgram.dot1Colour = GetUniformLocation(ShaderProgram, "Dot1Colour");
	ShaderProgram.dot2Colour = GetUniformLocation(ShaderProgram, "Dot2Colour");
	
	ShaderProgram.alphaOverrideLocation = GetUniformLocation(ShaderProgram, "gAlphaOverride");
	ShaderProgram.projLocation = GetUniformLocation(ShaderProgram, ATTRIB_PRE"gl_ProjectionMatrix");
	ShaderProgram.mvLocation = GetUniformLocation(ShaderProgram, ATTRIB_PRE"gl_ModelViewMatrix");
	ShaderProgram.worldSpaceLightPos = GetUniformLocation(ShaderProgram, "worldSpaceLightPos");
	ShaderProgram.peopleDiffuse = GetUniformLocation(ShaderProgram, "Diffuse");
	ShaderProgram.objectSpaceLightDirDiffuse = GetUniformLocation(ShaderProgram, "ObjectSpaceLightDir");
	ShaderProgram.lightColour = GetUniformLocation(ShaderProgram, "LightColour");
	ShaderProgram.ambientColour = GetUniformLocation(ShaderProgram, "AmbientColour");
	ShaderProgram.gCausticsScaleLocation = GetUniformLocation(ShaderProgram, "gCausticsScale");
	ShaderProgram.vRoadColours = GetUniformLocation(ShaderProgram, "vRoadColour");
	ShaderProgram.vOutlyingColours = GetUniformLocation(ShaderProgram, "vOutlyingColour");
}
void BindAttributes(GLShader& ShaderProgram) {
#ifdef _JAMIX
#else
	glBindAttribLocation(ShaderProgram.m_program, ATTR_VERTEX, ATTRIB_PRE"gl_Vertex");
	glBindAttribLocation(ShaderProgram.m_program, ATTR_NORMAL, ATTRIB_PRE"gl_Normal");
	glBindAttribLocation(ShaderProgram.m_program, ATTR_COLOR, ATTRIB_PRE"gl_Color");
	glBindAttribLocation(ShaderProgram.m_program, ATTR_UV, ATTRIB_PRE"gl_MultiTexCoord0");
#endif
}
#include <string>
#include <map>
#include <vector>
using namespace std;

#include "assetloader.h"

extern int win_width;
extern int win_height;
extern bool gUseShaders;

GLShader DefaultShaderProgram;
void GLMakeSmallWhite();
void RenderInit() {
	PrepareShaderPrefixes();
	DefaultShader.SetSources( "data/default.vert", "data/default.frag" );
	GLMakeSmallWhite();
	FontRenderInit();
}
void RenderUpdate() {
	DefaultShader.ReloadIfNecessary();
	FontUpdate();
}

extern GLuint textureLocation;
extern float gCubeAngle;
typedef string Key;
struct TextureAsset {
	Image *im;
	GLuint glTextureID;
};
typedef map<Key, TextureAsset*> TextureDic;
//typedef map<int, TextureAsset*> MaterialToTexture;
TextureDic gTextures;
map<string,int> reverselookup;
map<string, TextureAsset> icons;

#define _M(_r,_c) (((_c)-1)*4+(_r)-1)

float g_fLastPerspectiveValues[2];
float g_fPerspectiveFrustumPlanes[6][4];
float* GLPerspective(float* pMatrix, float fFovRad, float fAspect, float fZNear, float fZFar)
{
	float fYMax = fZNear * tanf(fFovRad*0.5f);
	float fXMax = fYMax * fAspect;
	float fDiv = 1.0f / (fZFar - fZNear);
	
	memset(pMatrix, 0, sizeof(float)*16);
	pMatrix[0*4+0] = fZNear / fXMax;
	pMatrix[1*4+1] = fZNear / fYMax;
	pMatrix[2*4+2] = -(fZFar+fZNear) * fDiv;
	pMatrix[2*4+3] = -1.0f;
	pMatrix[3*4+2] = -2 * fZFar * fZNear * fDiv;
	
	g_fLastPerspectiveValues[0] = pMatrix[0*4+0];
	g_fLastPerspectiveValues[1] = pMatrix[1*4+1];
	
	g_fPerspectiveFrustumPlanes[0][0] = pMatrix[_M(4,1)] + pMatrix[_M(1,1)];
	g_fPerspectiveFrustumPlanes[0][1] = pMatrix[_M(4,2)] + pMatrix[_M(1,2)];
	g_fPerspectiveFrustumPlanes[0][2] = pMatrix[_M(4,3)] + pMatrix[_M(1,3)];
	g_fPerspectiveFrustumPlanes[0][3] = pMatrix[_M(4,4)] + pMatrix[_M(1,4)];
	
	g_fPerspectiveFrustumPlanes[1][0] = pMatrix[_M(4,1)] - pMatrix[_M(1,1)];
	g_fPerspectiveFrustumPlanes[1][1] = pMatrix[_M(4,2)] - pMatrix[_M(1,2)];
	g_fPerspectiveFrustumPlanes[1][2] = pMatrix[_M(4,3)] - pMatrix[_M(1,3)];
	g_fPerspectiveFrustumPlanes[1][3] = pMatrix[_M(4,4)] - pMatrix[_M(1,4)];

	g_fPerspectiveFrustumPlanes[2][0] = pMatrix[_M(4,1)] + pMatrix[_M(2,1)];
	g_fPerspectiveFrustumPlanes[2][1] = pMatrix[_M(4,2)] + pMatrix[_M(2,2)];
	g_fPerspectiveFrustumPlanes[2][2] = pMatrix[_M(4,3)] + pMatrix[_M(2,3)];
	g_fPerspectiveFrustumPlanes[2][3] = pMatrix[_M(4,4)] + pMatrix[_M(2,4)];
	
	g_fPerspectiveFrustumPlanes[3][0] = pMatrix[_M(4,1)] - pMatrix[_M(2,1)];
	g_fPerspectiveFrustumPlanes[3][1] = pMatrix[_M(4,2)] - pMatrix[_M(2,2)];
	g_fPerspectiveFrustumPlanes[3][2] = pMatrix[_M(4,3)] - pMatrix[_M(2,3)];
	g_fPerspectiveFrustumPlanes[3][3] = pMatrix[_M(4,4)] - pMatrix[_M(2,4)];

	g_fPerspectiveFrustumPlanes[4][0] = pMatrix[_M(4,1)] + pMatrix[_M(3,1)];
	g_fPerspectiveFrustumPlanes[4][1] = pMatrix[_M(4,2)] + pMatrix[_M(3,2)];
	g_fPerspectiveFrustumPlanes[4][2] = pMatrix[_M(4,3)] + pMatrix[_M(3,3)];
	g_fPerspectiveFrustumPlanes[4][3] = pMatrix[_M(4,4)] + pMatrix[_M(3,4)];
	
	g_fPerspectiveFrustumPlanes[5][0] = pMatrix[_M(4,1)] - pMatrix[_M(3,1)];
	g_fPerspectiveFrustumPlanes[5][1] = pMatrix[_M(4,2)] - pMatrix[_M(3,2)];
	g_fPerspectiveFrustumPlanes[5][2] = pMatrix[_M(4,3)] - pMatrix[_M(3,3)];
	g_fPerspectiveFrustumPlanes[5][3] = pMatrix[_M(4,4)] - pMatrix[_M(3,4)];
	
	return pMatrix;
}

float GLPerspectiveFactorX() { return g_fLastPerspectiveValues[0]; }
float GLPerspectiveFactorY() { return g_fLastPerspectiveValues[1]; }

float* GLLookAt(float* pMatrix, const Vec3& vPos, const Vec3& vFocus, const Vec3& vUpBase)
{
	Vec3 vFwd = (vFocus - vPos).normalized();
	Vec3 vSide = vFwd.cross(vUpBase).normalized();
	Vec3 vUp = vSide.cross(vFwd);
	memset(pMatrix, 0, sizeof(float)*16);
	pMatrix[0*4+0] = vSide.x;
	pMatrix[1*4+0] = vSide.y;
	pMatrix[2*4+0] = vSide.z;
	pMatrix[3*4+0] = -vSide.dot(vPos);
	pMatrix[0*4+1] = vUp.x;
	pMatrix[1*4+1] = vUp.y;
	pMatrix[2*4+1] = vUp.z;
	pMatrix[3*4+1] = -vUp.dot(vPos);
	pMatrix[0*4+2] = -vFwd.x;
	pMatrix[1*4+2] = -vFwd.y;
	pMatrix[2*4+2] = -vFwd.z;
	pMatrix[3*4+2] = vFwd.dot(vPos);
	pMatrix[3*4+3] = 1.0f;

	return pMatrix;
}

float* GLMultiplyMatrix(float* pD, const float* pM2, const float* pM1)
{
	float d[16];
	float* pO = pD;
	if (pM1 == pD || pM2 == pD) pO = d;
	
	pO[0*4+0] = pM1[0*4+0] * pM2[0*4+0] + pM1[0*4+1] * pM2[1*4+0] + pM1[0*4+2] * pM2[2*4+0] + pM1[0*4+3] * pM2[3*4+0];
	pO[0*4+1] = pM1[0*4+0] * pM2[0*4+1] + pM1[0*4+1] * pM2[1*4+1] + pM1[0*4+2] * pM2[2*4+1] + pM1[0*4+3] * pM2[3*4+1];
	pO[0*4+2] = pM1[0*4+0] * pM2[0*4+2] + pM1[0*4+1] * pM2[1*4+2] + pM1[0*4+2] * pM2[2*4+2] + pM1[0*4+3] * pM2[3*4+2];
	pO[0*4+3] = pM1[0*4+0] * pM2[0*4+3] + pM1[0*4+1] * pM2[1*4+3] + pM1[0*4+2] * pM2[2*4+3] + pM1[0*4+3] * pM2[3*4+3];

	pO[1*4+0] = pM1[1*4+0] * pM2[0*4+0] + pM1[1*4+1] * pM2[1*4+0] + pM1[1*4+2] * pM2[2*4+0] + pM1[1*4+3] * pM2[3*4+0];
	pO[1*4+1] = pM1[1*4+0] * pM2[0*4+1] + pM1[1*4+1] * pM2[1*4+1] + pM1[1*4+2] * pM2[2*4+1] + pM1[1*4+3] * pM2[3*4+1];
	pO[1*4+2] = pM1[1*4+0] * pM2[0*4+2] + pM1[1*4+1] * pM2[1*4+2] + pM1[1*4+2] * pM2[2*4+2] + pM1[1*4+3] * pM2[3*4+2];
	pO[1*4+3] = pM1[1*4+0] * pM2[0*4+3] + pM1[1*4+1] * pM2[1*4+3] + pM1[1*4+2] * pM2[2*4+3] + pM1[1*4+3] * pM2[3*4+3];

	pO[2*4+0] = pM1[2*4+0] * pM2[0*4+0] + pM1[2*4+1] * pM2[1*4+0] + pM1[2*4+2] * pM2[2*4+0] + pM1[2*4+3] * pM2[3*4+0];
	pO[2*4+1] = pM1[2*4+0] * pM2[0*4+1] + pM1[2*4+1] * pM2[1*4+1] + pM1[2*4+2] * pM2[2*4+1] + pM1[2*4+3] * pM2[3*4+1];
	pO[2*4+2] = pM1[2*4+0] * pM2[0*4+2] + pM1[2*4+1] * pM2[1*4+2] + pM1[2*4+2] * pM2[2*4+2] + pM1[2*4+3] * pM2[3*4+2];
	pO[2*4+3] = pM1[2*4+0] * pM2[0*4+3] + pM1[2*4+1] * pM2[1*4+3] + pM1[2*4+2] * pM2[2*4+3] + pM1[2*4+3] * pM2[3*4+3];
	
	pO[3*4+0] = pM1[3*4+0] * pM2[0*4+0] + pM1[3*4+1] * pM2[1*4+0] + pM1[3*4+2] * pM2[2*4+0] + pM1[3*4+3] * pM2[3*4+0];
	pO[3*4+1] = pM1[3*4+0] * pM2[0*4+1] + pM1[3*4+1] * pM2[1*4+1] + pM1[3*4+2] * pM2[2*4+1] + pM1[3*4+3] * pM2[3*4+1];
	pO[3*4+2] = pM1[3*4+0] * pM2[0*4+2] + pM1[3*4+1] * pM2[1*4+2] + pM1[3*4+2] * pM2[2*4+2] + pM1[3*4+3] * pM2[3*4+2];
	pO[3*4+3] = pM1[3*4+0] * pM2[0*4+3] + pM1[3*4+1] * pM2[1*4+3] + pM1[3*4+2] * pM2[2*4+3] + pM1[3*4+3] * pM2[3*4+3];

	if (pO == d) memcpy(pD, d, sizeof(d));
	return pD;
}

void AddAsset( const string &name, Image *source ) {
	TextureAsset &asset = icons[ name ];
	asset.im = source;
	asset.glTextureID = -1;
	gTextures.insert( TextureDic::value_type(name, &asset ) );
}

void MakeGLTexture( TextureAsset *a ) {
	if( a && a->im ) {
		if( int error = glGetError() ) {
			printf( "before MakeGLTexture (%i,%x)\n", error, error );
		}
		//printf( "MakeGLTexture:\ntextureID = %i => ", a->glTextureID );
		glGenTextures( 1, &a->glTextureID );
		if( int error = glGetError() ) {
			printf( "glGenTextures (%i,%x)\n", error, error );
			printf( "glGenTextures : (%i)\n", a->glTextureID );
		}
		//printf( "%i (%i)\n", a->glTextureID, glGetError() );
		glBindTexture( GL_TEXTURE_2D, a->glTextureID );
		//glUniform1i(textureLocation, a->glTextureID);
		if( int error = glGetError() ) {
			printf( "glBindTexture (%i,%x)\n", error, error );
		}
		int nGLFmt = GL_RGBA;
#ifdef _GLES
#else
		glShadeModel( GL_SMOOTH );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		nGLFmt = 4;
#endif
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//NEAREST);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexImage2D( GL_TEXTURE_2D, 0, nGLFmt,
					 a->im->w, a->im->h,
					 0, GL_RGBA,
					 GL_UNSIGNED_BYTE, a->im->p);
		if( int error = glGetError() ) {
			printf( "glTexImage2D (%i,%x)\n", error, error );
			printf( "glTexImage2D : (%i) (%i,%i : %x)\n",
				   a->glTextureID,
				   a->im->w, a->im->h,
				   (unsigned int)((char*)a->im->p-(char*)0) );
		}
	} else {
		printf( "cannot make much with a null\n" );
	}
}
bool TextureExists(const char* name) {
	return gTextures.count(name) > 0;
}
GLuint GLReadTexture(const char* pFilename)
{
	if (pFilename[0] == 0) return 0xFFFFFFFF;
	TextureAsset* pAss;
	if (TextureExists(pFilename))
		pAss = gTextures[pFilename];
	else
	{
		char pPath[1024];
		sprintf(pPath, "meshes/textures/%s", pFilename);
		AddAsset(pFilename, LoadImageG(pPath));
		pAss = gTextures[pFilename];
		if (pAss->im != NULL)
			MakeGLTexture(pAss);
		else
			printf("**** Tex %s read failed\n", pFilename);
	}
	return pAss->glTextureID;
}

void SetTexture(GLuint textureID, int slotID) {
	if( UniformExists(textureLocation) ) {
		if (textureID == 0xFFFFFFFF ) {
			extern GLuint g_nSmallWhiteTexture;
			textureID = g_nSmallWhiteTexture;
		}
		glActiveTexture(GL_TEXTURE0 + slotID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		SetUniform1i(textureLocation, slotID);
	}
}

void SetTexture(const char* pName, int slotID) {
	if( gTextures.count( pName ) ) {
		GLuint tid = gTextures[pName]->glTextureID;
		SetTexture(tid, slotID);
	} else {
		extern GLuint g_nSmallWhiteTexture;
		SetTexture(g_nSmallWhiteTexture, slotID);
	}
}
void GLActivateTexture(GLuint textureID, int nId);

void GLActivateTextureForLocation(GLuint textureID, int nSlot, int nLocation)
{
	extern GLuint g_nSmallWhiteTexture;
	if (textureID == 0xFFFFFFFF && (unsigned int)nLocation != 0xFFFFFFFF)
		textureID = g_nSmallWhiteTexture;
	if (textureID != 0xFFFFFFFF && (unsigned int)nLocation != 0xFFFFFFFF)
	{
		glActiveTexture(GL_TEXTURE0 + nSlot);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(nLocation, nSlot);
	}
}
void GLActivateTexture(GLuint textureID, int nId)
{
	extern GLuint g_nSmallWhiteTexture;
	if (textureID == 0xFFFFFFFF && UniformExists(textureLocation))
		textureID = g_nSmallWhiteTexture;
	if (textureID != 0xFFFFFFFF && UniformExists(textureLocation))
	{
		glActiveTexture(GL_TEXTURE0 + nId);
		glBindTexture(GL_TEXTURE_2D, textureID);
		SetUniform1i(textureLocation, nId);
	}
}

//void GLActivateTexture(const char* pName, int nId)
//{
	//GLActivateTexture(gTextures[pName]->glTextureID, nId);
//}

static char s_szTemp[1024];
char* Mtx2Str(float* p)
{
	sprintf(s_szTemp, "%f,%f,%f,%f : %f,%f,%f,%f : %f,%f,%f,%f : %f,%f,%f,%f\n",
		p[0],p[1],p[2],p[3], p[4],p[5],p[6],p[7], p[8],p[9],p[10],p[11], p[12],p[13],p[14],p[15]);
	return s_szTemp;
}

GLfloat m_ProjectionMatrix[16];
Mat44 m_ProjMatrix;
Mat44 m_CameraMatrix;
Mat44 m_ModelMatrix;

void Perspective() {
	GLPerspective(m_ProjectionMatrix, 2.0f, (float)win_width / (float)win_height, 0.1f, 100.0f);
}
void Orthographic() {
	memcpy( m_ProjectionMatrix, &gIdentityMat.x.x, sizeof( float ) * 16 );
}


void GLMultiplyVectorMatrix(Vec3 &output, const float* pM2, const Vec3 vec)
{
	GLfloat mvvec[4];
	
	mvvec[0*4+0] = vec.x * pM2[0*4+0] + vec.y * pM2[1*4+0] + vec.z * pM2[2*4+0] +  pM2[3*4+0];
	mvvec[0*4+1] = vec.x * pM2[0*4+1] + vec.y * pM2[1*4+1] + vec.z * pM2[2*4+1] +  pM2[3*4+1];
	mvvec[0*4+2] = vec.x * pM2[0*4+2] + vec.y * pM2[1*4+2] + vec.z * pM2[2*4+2] +  pM2[3*4+2];
	mvvec[0*4+3] = vec.x * pM2[0*4+3] + vec.y * pM2[1*4+3] + vec.z * pM2[2*4+3] +  pM2[3*4+3];
    
    //Minus the y, because its the only way to have player come out correctly
    if (mvvec[3] != 0)
        output = Vec3((mvvec[0]/mvvec[3]),-mvvec[1]/mvvec[3],mvvec[2]/mvvec[3]);
    
	return;
}

void GLWorldToScreenPoint(Vec3 &output, Vec3 input)
{
    GLMultiplyVectorMatrix(output, m_ProjectionMatrix, input);
}

//void GLLoadTransformWithAxisRotation(float x, float y, float z, float sx, float sy, float sz, float orientation, Vec2 axis, float ammount) {
//}

void GLLoadTransform(float x, float y, float z, float sx, float sy, float sz, float orientation )
{
	GLfloat mvmat[16];
	float fSin = sinf(orientation), fCos = cosf(orientation);
	memset(mvmat, 0, sizeof(mvmat));
	mvmat[0*4+0] = fCos*sx;
	mvmat[0*4+2] = -fSin*sx;

	mvmat[1*4+1] = sy;

	mvmat[2*4+0] = fSin*sz;
	mvmat[2*4+2] = fCos*sz;
	mvmat[3*4+0] = x;
	mvmat[3*4+1] = y;
	mvmat[3*4+2] = z;
	mvmat[3*4+3] = 1;

	SetUniformMat(mvLocation, mvmat);
	SetUniformMat(projLocation, m_ProjectionMatrix);
}
void GLLoadTransform(float x, float y, float z, float scale, float orientation )
{
	GLLoadTransform(x,y,z, scale,scale,scale, orientation );
}
void GLSetOrtho(float left, float right, float top, float bottom, float back, float front) {
	m_ProjMatrix = Mat44Orthographic( left, right, bottom, top, front, back );
	SetUniformMat(projLocation, m_ProjMatrix);
}
void GLSetPerspective(float fov, float near, float far ) {
	float ratio = (float)win_width / (float)win_height;
	m_ProjMatrix = Mat44Perspective( fov, ratio, near, far );
	SetUniformMat(projLocation, m_ProjMatrix);
}
void GLSetCamera(const Mat44 &m) {
	m_CameraMatrix = m;
}
void GLSetCamera(const Vec3 &pos, const Vec3 &target) {
	GLSetCamera( Mat44LookAt( pos, target, gYVec3 ) );
}
void GLUploadPV() {
	SetUniformMat(projLocation, m_ProjMatrix);
}
void GLSetModel(const Mat44 &m) {
	m_ModelMatrix = m;
	Mat44 mv = m_ModelMatrix * m_CameraMatrix;
	SetUniformMat(mvLocation, mv );
}
void GLSetModel(const Vec3 &pos, float orientation) {
	Mat44 m;
	m.RotY( orientation );
	m.w = pos;
	GLSetModel( m );
}

Vec3 wsLightDir(0.5f, 1.0f, -0.5f);
Vec3 g_lightColour(1,1,1);
Vec3 g_ambientColour(0,0,0);

void GLSetObjectSpaceLightDir(float orientation)
{
	// rotate light back into object space by rotating by -orientation
	float fSin = sinf(orientation), fCos = cosf(orientation);
	Vec3 osLightDir;
	osLightDir.x = wsLightDir.x * fCos + wsLightDir.z * -fSin;
	osLightDir.y = wsLightDir.y;
	osLightDir.z = wsLightDir.x * fSin + wsLightDir.z * fCos;
	SetUniform3fv(objectSpaceLightDirDiffuse, 1, &osLightDir.x);
	SetUniform3fv(lightColour, 1, &g_lightColour.x);
	SetUniform3fv(ambientColour, 1, &g_ambientColour.x);
}
void GLSetWorldSpaceLightDir()
{
	SetUniform3fv(objectSpaceLightDirDiffuse, 1, &wsLightDir.x);
	SetUniform3fv(lightColour, 1, &g_lightColour.x);
	SetUniform3fv(ambientColour, 1, &g_ambientColour.x);
}
Vec3 g_vToDLights[] = { Vec3(1,0.6f,0.5f), Vec3(1,0.9f,0.7f), Vec3(1,1,0.8f), Vec3(1,0.9f,0.7f), Vec3(1,0.6f,0.5f), Vec3(0.3f,0.3f,0.7f), Vec3(0.2f,0.2f,1.0f), Vec3(0.3f,0.3f,0.7f) };
void GLUpdateLight(float )
{
	float fStage = 0.5f;
	// fTime 0 means mid-day
	float fTheta = (float)(fStage * 2 * M_PI);
	float fS = sinf(fTheta), fC = cosf(fTheta);
	wsLightDir.x = fC; wsLightDir.y = fS + 0.3f;
	fStage *= 8;
	int nStage = (int)fStage;
	float fSub = fStage - (float)nStage;
	g_lightColour = g_vToDLights[nStage] + (g_vToDLights[(nStage+1)&7] - g_vToDLights[nStage]) * fSub;
}

void DrawTree(float x, float y, float z, float orientation, float scale, int treeType, int nTreeState, Vec2 skew, GLMeshList *pList) {
	F32 fSub = (F32)(nTreeState & 15) * (1.0f/16.0f);
	// 3,2,2.5; 2.5,1.5,2; 2,1,1.5, 1.5,0
	F32 fBounce = (fSub - 0.5f) * 2; if (fBounce < 0) fBounce = -fBounce*0.5f;
	fBounce = (3.0f - fBounce - fBounce) * (fBounce * fBounce);
	fBounce = 0.5f + fBounce * 0.5f;
	S32 n = nTreeState / 16;
	fBounce = (fBounce + n * 0.5f) * (1.0f/3.0f);
	//printf("Tree %d %d - %f\n", n, nTreeState, fBounce);
	S32 nLow = nTreeState; if (nLow > 16) nLow = 16;
	F32 f = (F32)nLow * (1.0f/16.0f);
	S32 nFrame = 0;
	float scaleY = scale;
	switch (treeType)
    {
        case 0: orientation += (float)M_PI*2*f; scaleY *= fBounce * f; scale *= f; break;
        case 1: orientation += (float)M_PI*2*f; scaleY *= fBounce * f; scale *= f; break;
    }
	if (pList == NULL) return;
	if (nFrame < 0 || nFrame >= pList->m_nMeshCount) return;
	
	DefaultShaderProgram.Use();

	if (treeType < 4)
        GLLoadTransform(x,y,z, scale,scaleY,scale, orientation);
    else
        GLLoadTransform(x+skew.x/5,y,z+skew.y/5, scale,scaleY,scale, orientation);
	GLSetObjectSpaceLightDir(orientation);

	GLDrawMesh(pList, nFrame);
}


void GLDrawVertStrip(F32* pPoints, int nNumPoints, U32 nClr, U32 nTex)
{
	float fClr[4] = { ((nClr>>0)&0xFF) * (1.0f/255.0f), ((nClr>>8)&0xFF) * (1.0f/255.0f), ((nClr>>16)&0xFF) * (1.0f/255.0f), ((nClr>>24)&0xFF) * (1.0f/255.0f) };
	SetUniform4fv(peopleDiffuse, 1, fClr);
	GLActivateTexture(nTex, 0);
	GLLoadTransform(0,0,0, 1, 0);
	glEnableVertexAttribArray(ATTR_VERTEX);
	glVertexAttribPointer(ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(F32)*5, &pPoints[0]);
	glDisableVertexAttribArray(ATTR_NORMAL);
	glEnableVertexAttribArray(ATTR_UV);
	glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, GL_FALSE, sizeof(F32)*5, &pPoints[3]);
	glDisableVertexAttribArray(ATTR_COLOR);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, nNumPoints);
}

void GLDrawMeshWithAlpha(GLMeshList* pMeshList, int nMeshIndex, float alpha, int nOverrideMaterial, float* pOverrideMaterialColour)
{
	GLMeshContainer* pMesh = pMeshList->Mesh(nMeshIndex);
	
	pMesh->BeginDraw();
	const int nSubMeshCount = pMesh->PartCount();
    for (int n = 0; n < nSubMeshCount; ++n)
    {
		if (n == nOverrideMaterial)
			SetUniform4fv(peopleDiffuse, 1, pOverrideMaterialColour);
		else
			pMesh->SetDiffuseToUniform(n, GLShader::Current()->peopleDiffuse, alpha);
		pMesh->Draw(n);
	}
	pMesh->EndDraw();
}

void GLDrawMeshWithoutTexture(GLMeshList* pMeshList, int nMeshIndex)
{
	GLMeshContainer* pMesh = pMeshList->Mesh(nMeshIndex);
	
	pMesh->BeginDraw();
	const int nSubMeshCount = pMesh->PartCount();
    for (int n = 0; n < nSubMeshCount; ++n)
    {
		pMesh->Draw(n);
	}
	pMesh->EndDraw();
}

void GLDrawMeshWithMeshTextureAndParameterAlpha(GLMeshList* pMeshList, int nMeshIndex, float alpha)
{
	if (nMeshIndex < 0 || nMeshIndex >= pMeshList->m_nMeshCount) return;
	GLMeshContainer* pMesh = pMeshList->Mesh(nMeshIndex);
	
	float pWhite[] = { 1,1,1, alpha };
	SetUniform4fv(peopleDiffuse, 1, pWhite);
	
	pMesh->BeginDraw();
	const int nSubMeshCount = pMesh->PartCount();
    for (int n = 0; n < nSubMeshCount; ++n)
    {
		GLuint nTex = pMesh->PartDiffuseTexture(n);
		GLActivateTexture(nTex, 0);
		pMesh->Draw(n);
	}
	pMesh->EndDraw();
}

void GLDrawMesh(GLMeshList* pMeshList, int nMeshIndex, int nOverrideMaterial, float* pOverrideMaterialColour)
{
	if (nMeshIndex < 0 || nMeshIndex >= pMeshList->m_nMeshCount) return;
	GLMeshContainer* pMesh = pMeshList->Mesh(nMeshIndex);
	
	pMesh->BeginDraw();
	const int nSubMeshCount = pMesh->PartCount();
    for (int n = 0; n < nSubMeshCount; ++n)
    {
		GLuint nTex = pMesh->PartDiffuseTexture(n);
		GLActivateTexture(nTex, 0);
		if (nTex != 0xFFFFFFFFU)
		{
			float pWhite[] = { 1,1,1,1 };
			SetUniform4fv(peopleDiffuse, 1, pWhite);
		}
		else if (nOverrideMaterial & (1<<n))
			SetUniform4fv(peopleDiffuse, 1, pOverrideMaterialColour);
		else
			pMesh->SetDiffuseToUniform(n, GLShader::Current()->peopleDiffuse);
		pMesh->Draw(n);
	}
	pMesh->EndDraw();
}

void GLDrawMeshWithoutColour(GLMeshList* pMeshList, int nMeshIndex)
{
	GLMeshContainer* pMesh = pMeshList->Mesh(nMeshIndex);
	
	pMesh->BeginDraw();
	const int nSubMeshCount = pMesh->PartCount();
    for (int n = 0; n < nSubMeshCount; ++n)
    {
		pMesh->Draw(n);
	}
	pMesh->EndDraw();
}

//========================================================

U32 DrawSpriteColourConv(U32 nClr)
{
#if GL_PREMULTIPLIED_ALPHA
	U32 nA = nClr >> 24;
	U32 nR = (nClr >> 16) & 0xFF;
	U32 nG = (nClr >> 8) & 0xFF;
	U32 nB = (nClr >> 0) & 0xFF;
	nR = nR * nA / 255;
	nG = nG * nA / 255;
	nB = nB * nA / 255;
	return (nA<<24) | (nR << 16) | (nG << 8) | (nB << 0);
#else
	return nClr;
#endif
}
U32 DrawSpriteColourConvAI(U32 nClr)
{
#if GL_PREMULTIPLIED_ALPHA
	U32 nA = nClr >> 8;
	U32 nI = (nClr >> 0) & 0xFF;
	nI = nI * nA / 255;
	return (nA<<8) | (nI << 0);
#else
	return nClr;
#endif
}

GLuint GLMemoryTexture(S32 nW, S32 nH, U32* pnImage, S32 bCreateMipChain, S32 nFormat)
{
	GLuint nTex;
	S32 bPalettised = (nW >> 16) & 1;
	(void)bPalettised;
	nW &= 0xFFFF; nH &= 0xFFFF;
	
	if (nFormat == -1) // 32bit input with no checks - really want 32bit!
	{
		nFormat = 0;
	}
	else if (nFormat == 0) // 32bit input
	{
		// check whether we have alpha or grey
		S32 i;
		S32 bNoAlpha = 1, bGrey = 1;
#if !GL_PREMULTIPLIED_ALPHA
		S32 bWhite = 1;
#endif
		U8* pbImage = (U8*)pnImage;
		for (i = 0; i < nW*nH; i ++)
		{
			if (pbImage[i*4+3] != 255)
				bNoAlpha = 0;
			if (pbImage[i*4+0] != pbImage[i*4+1] || pbImage[i*4+0] != pbImage[i*4+2] || pbImage[i*4+1] != pbImage[i*4+2])
				bGrey = 0;
#if !GL_PREMULTIPLIED_ALPHA
			if (pbImage[i*4+0] != 255)
				bWhite = 0;
#endif
			if (!bNoAlpha && !bGrey) break;
		}
		if (bNoAlpha && bGrey)
		{
			// convert to GL_LUMINANCE
			nFormat = 4;
			for (i = 0; i < nW*nH; i ++)
				pbImage[i] = pbImage[i*4+0];
		}
#if !GL_PREMULTIPLIED_ALPHA
		else if (!bNoAlpha && bGrey && bWhite)
		{
			// convert to GL_ALPHA
			nFormat = 2;
			for (i = 0; i < nW*nH; i ++)
				pbImage[i] = pbImage[i*4+3];
		}
#endif
		else if (!bNoAlpha && bGrey)
		{
			// convert to GL_LUMINANCE_ALPHA
			nFormat = 1;
			for (i = 0; i < nW*nH; i ++)
				*(U16*)&pbImage[i*2] = DrawSpriteColourConvAI((pbImage[i*4+3]<<8) | pbImage[i*4+0]);
		}
		else if (bNoAlpha)
		{
			// convert to GL_RGB
			for (i = 0; i < nW*nH; i ++)
			{
				nFormat = 3;
				pbImage[i*3+0] = pbImage[i*4+0];
				pbImage[i*3+1] = pbImage[i*4+1];
				pbImage[i*3+2] = pbImage[i*4+2];
			}
		}
	}
	else if (nFormat == 1) // LumAlpha input
	{
		S32 i;
		S32 bNoAlpha = 1;
#if !GL_PREMULTIPLIED_ALPHA
		S32 bWhite = 1;
#endif
		U8* pbImage = (U8*)pnImage;
		for (i = 0; i < nW*nH; i ++)
		{
			if (pbImage[i*2+1] != 255) bNoAlpha = 0;
#if !GL_PREMULTIPLIED_ALPHA
			if (pbImage[i*2+0] != 255) bWhite = 0;
			if (!bNoAlpha && !bWhite) break;
#endif
		}
		if (bNoAlpha)
		{
			// convert to GL_LUMINANCE
			nFormat = 4;
			for (i = 0; i < nW*nH; i ++)
				pbImage[i] = pbImage[i*2+0];
		}
#if !GL_PREMULTIPLIED_ALPHA
		else if (bWhite)
		{
			// convert to GL_ALPHA
			nFormat = 2;
			for (i = 0; i < nW*nH; i ++)
				pbImage[i] = pbImage[i*2+1];
		}
#endif
	}
	
	GLint nGLFmt;
	switch (nFormat)
	{
		case 0: nGLFmt = GL_RGBA; break;
		case 1: nGLFmt = GL_LUMINANCE_ALPHA; break;
		case 2: nGLFmt = GL_ALPHA; break;
		case 3: nGLFmt = GL_RGB; break;
		case 4: nGLFmt = GL_LUMINANCE; break;
	}
	glGenTextures(1, &nTex);
	
	// GL - 070113 - GL3.1+ and GLES2+ depricate GL_GENERATE_MIPMAP but rather than make the
	// assumption I'm going to detect an error setting it and use the newer glGenerateMipmap function
	// King of Hacks crown firmly retained for the new year!
#ifdef _GLES
	S32 bUseGenerateMipmap = 0;
#endif
	
	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, bCreateMipChain ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, bCreateMipChain ? GL_TRUE : GL_FALSE);
#ifdef _GLES
	if (glGetError()) bUseGenerateMipmap = 1;
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, nGLFmt, nW, nH, 0, nGLFmt, GL_UNSIGNED_BYTE, pnImage);

#ifdef _GLES
	if (bUseGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
#endif
	return nTex;
}
void GLMemoryTextureReload(GLuint handle, S32 nW, S32 nH, U32* pnImage)
{
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nW, nH, GL_RGBA, GL_UNSIGNED_BYTE, pnImage);
}

GLuint g_nSmallWhiteTexture;
void GLMakeSmallWhite()
{
	U32 nWhite[8*8];
	memset(nWhite, 0xFF, sizeof(nWhite));
	g_nSmallWhiteTexture = GLMemoryTexture(8, 8, nWhite, 0, 0);
}

class GLVertexSprite
{
public:
	F32 m_fX, m_fY, m_fZ;
	F32 m_fU, m_fV;
	U32 m_nClr;
	void Set(F32 fX, F32 fY, F32 fZ, F32 fU, F32 fV, U32 nClr)
	{
		m_fX = fX; m_fY = fY; m_fZ = fZ;
		m_fU = fU; m_fV = fV; m_nClr = nClr;
	}
	void Set()
	{
		glEnableVertexAttribArray(ATTR_VERTEX);
		glDisableVertexAttribArray(ATTR_NORMAL);
		glEnableVertexAttribArray(ATTR_UV);
		glEnableVertexAttribArray(ATTR_COLOR);
		glVertexAttribPointer(ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertexSprite), &m_fX);
		glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertexSprite), &m_fU);
		glVertexAttribPointer(ATTR_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLVertexSprite), &m_nClr);
	}
};
void GLDrawSprite(F32 fX, F32 fY, F32 fW, F32 fH, F32 fZ, U32 nClr, U32 nTex, U32 , F32 fUx,F32 fVy,F32 fUw,F32 fVh)
{
	DefaultShaderProgram.Use();//SpriteShaderProgram.Use();
	nClr = DrawSpriteColourConv(nClr);
	
	//fX = fX / g_fScreenW;
	//fY = fY / g_fScreenH;
	//fW = fW / g_fScreenW;
	//fH = fH / g_fScreenH;
	
	fY = 1 - fY - fH;
	fVy = 1 - fVy - fVh;
		
	glBindTexture(GL_TEXTURE_2D, nTex);
	
	GLVertexSprite pVerts[4];
	pVerts[0].Set(fX*2-1,fY*2-1,fZ, fUx,fVy, nClr);
	pVerts[1].Set((fX+fW)*2-1,fY*2-1,fZ, fUx+fUw,fVy, nClr);
	pVerts[2].Set(fX*2-1,(fY+fH)*2-1,fZ, fUx,fVy+fVh, nClr);
	pVerts[3].Set((fX+fW)*2-1,(fY+fH)*2-1,fZ, fUx+fUw,fVy+fVh, nClr);
	pVerts[0].Set();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void GLDrawSprite(F32 fX, F32 fY, F32 fW, F32 fH, F32 fZ, U32 nClr, const char* pAssetName, U32 nFlags, F32 fUx,F32 fVy,F32 fUw,F32 fVh)
{
	GLDrawSprite(fX,fY,fW,fH, fZ, nClr, gTextures[pAssetName]->glTextureID, nFlags, fUx,fVy,fUw,fVh);
}
void GLDrawSpriteDeferStart() {}
void GLDrawSpriteDeferEnd(S32 , U32 ) {}
void GLDrawSpriteDeferEnd() {}


typedef struct
{
	float m_fU, m_fV, m_fDU, m_fDV;
	float m_fW, m_fLPad;
} DIGIT_INFO;
DIGIT_INFO m_cBlobDigitInfo[] = {
	{ 0/8.0f, 0/8.0f, 1/8.0f, 1/8.0f, 18.0f*2*0+17*2, 0 },
	{ 1/8.0f, 0/8.0f, 1/8.0f, 1/8.0f,  8.0f*2*0+17*2, 0 },
	{ 2/8.0f, 0/8.0f, 1/8.0f, 1/8.0f, 14.0f*2*0+17*2, 0 },
	{ 3/8.0f, 0/8.0f, 1/8.0f, 1/8.0f, 16.0f*2*0+17*2, 0 },
	{ 4/8.0f, 0/8.0f, 1/8.0f, 1/8.0f, 18.0f*2*0+17*2, 0 },
	{ 5/8.0f, 0/8.0f, 1/8.0f, 1/8.0f, 16.0f*2*0+17*2, 0 },
	{ 6/8.0f, 0/8.0f, 1/8.0f, 1/8.0f, 17.0f*2*0+17*2, 0 },
	{ 7/8.0f, 0/8.0f, 1/8.0f, 1/8.0f, 17.0f*2*0+17*2, 0 },
	{ 0/8.0f, 1/8.0f, 1/8.0f, 1/8.0f, 17.0f*2*0+17*2, 0 },
	{ 1/8.0f, 1/8.0f, 1/8.0f, 1/8.0f, 17.0f*2*0+17*2, 0 },
	//
	{ 2/8.0f, 1/8.0f, 1/8.0f, 1/8.0f, 17.0f*2*0+17*2, 0 },
	{ 3/8.0f, 1/8.0f, 1/8.0f, 1/8.0f, 17.0f*2*0+17*2, 8 },
	{ 4/8.0f, 1/8.0f, 1/8.0f, 1/8.0f, 17.0f*2*0+17*2, 0 },
};
const F32 c_fStandardBlobW = 64.0f;
const F32 c_fStandardBlobH = 60.0f;
const F32 c_fDUPerPass = 0.0f;
const F32 c_fDVPerPass = 0.25f;
const F32 c_fOverlapFactor = 0.65f;
void GLDrawBlobNumber(F32 fX, F32 fY, F32 fSize, S32 nValue, S32 nJustify, U32 nFg, U32 nBg)
{
	U32 nClrs[] = { 0x00FFFFFF + (nBg & 0xFF000000), nBg, nFg };
	S32 nMag = 0;
	if (nValue >= 10000000-1000) // GL - 310112 - -1000 to avoid mismatch between last in k and first in m
	{
		nMag = 2;
		nValue = (nValue + 500000) / 1000000;
	}
	else if (nValue > 10000)
	{
		nMag = 1;
		nValue = (nValue + 500) / 1000;
	}
	int nNumDigits = 0, nDigits[20];
	if (nMag) nDigits[nNumDigits++] = 10 + nMag - 1;	
	while (nValue)
	{
		int nNext = nValue / 10;
		nDigits[nNumDigits++] = nValue - nNext * 10;
		nValue = nNext;
	}
	if (nNumDigits == 0) nDigits[nNumDigits++] = 0;
	
	int nPass, nDigit;
	F32 fDX[] = { 4*0, 0, 0 }, fDY[] = { 4*0, 0, 0 };
	
	// calc total width
	F32 fTW = 0;
	for (nDigit = 0; nDigit < nNumDigits; nDigit ++)
		fTW += m_cBlobDigitInfo[nDigits[nDigit]].m_fW*fSize*c_fOverlapFactor;
		
	// now add on for anchor
	fX -= fTW * nJustify * 0.5f;
	
	//fX -= m_cBlobDigitInfo[nDigits[0]].m_fW*fSize*0.5f;
	//fX -= (1 - m_cBlobDigitInfo[nDigits[0]].m_fW * (1.0f/c_fStandardBlobW)) * m_cBlobDigitInfo[nDigits[0]].m_fDU * 0.5f;
	fX -= c_fStandardBlobW * 0.25f * fSize;
	
	F32 fIX = fX;
	for (nPass = 0; nPass < 3; nPass ++)
	{
		fX = fIX;
		for (nDigit = 0; nDigit < nNumDigits; nDigit ++)
		{
			S32 n = nDigits[nNumDigits-1-nDigit];
			DIGIT_INFO* pI = &m_cBlobDigitInfo[n];
			
			F32 fW = pI->m_fW;
			F32 fU = pI->m_fU;
			F32 fV = pI->m_fV;
			F32 fDU = pI->m_fDU;
			F32 fDV = pI->m_fDV;
			F32 fP = 0;//(1 - fW * (1.0f/c_fStandardBlobW)) * fDU;
			fU += fP * 0.5f; fDU -= fP;
			
			fU += c_fDUPerPass*nPass;
			fV += c_fDVPerPass*nPass;
			
			fX += pI->m_fLPad;
			
			GLDrawSprite(fX+fDX[nPass]*fSize, fY+fDY[nPass]*fSize, c_fStandardBlobW*fSize+0*fW*fSize, c_fStandardBlobH*fSize, 0.0f, nClrs[nPass], "Blobby", 0, fU,fV,fDU,fDV);
			fX += fW*fSize*c_fOverlapFactor;
		}
		/*if (nMag && nPass == 2)
		{
			const char* c_szMags[] = { "k", "m" };
			extern GLFont* g_pMainFont;
			g_pMainFont->DrawString(fX+22*fSize, fY+20*fSize, 0.5f*fSize, nClrs[nPass], 0, 0x7FFFFFFF, -1, c_szMags[nMag-1]);
		}*/
	}
}

