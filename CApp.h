//==============================================================================
// SDL Tutorial 1
//==============================================================================
#ifndef _CAPP_H_
#define _CAPP_H_

#if _WIN32
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glew32.lib")
//# define GL_GLEXT_PROTOTYPES
#include "SDL2/SDL.h"
#include "glew-1.12.0\include\GL\glew.h"
//#include "SDL2/SDL_opengl.h"d
#include "render.h"
#else
#include "render.h"
# define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "geom.h"
enum SHIP_TYPE {
  ST_BASE,
  ST_LARGE,
  ST_FAT,
  ST_GIANT,
  ST_NUM_ST
};

#include "broadside.h"
#include "IM.h"

//==============================================================================
class CApp {
    private:
        bool            Running;

        SDL_Window * window;
        SDL_GLContext context;

		  bool m_PointerDown;
		  bool m_PointerUp;
		  IVec2 m_PointerPos;
		  IVec2 m_PointerMove;

		  UIContext ui;

    public:
        CApp();

        int OnExecute();

    public:
        bool OnInit();
        void OnEvent(SDL_Event* Event);
        void OnLoop();
        void OnRender();
        void OnCleanup();

    public:
        typedef std::vector<ButtonRenderData> ButtonVec;
        ButtonVec ButtonsToRender;
        bool IMButton( int id, const Rect &r, const TXT &text, const Style &style );
        bool IMDraggable( int id, Rect &r, const TXT &text, const Style &style );

  public:
    void Set2D();
    void DrawRect( int x, int y, int w, int h, const Vec4 &colour );

    void Set3D();
    void DrawShip( const Vec3 &pos, SHIP_TYPE type, float orientation );
};

//==============================================================================

#endif
