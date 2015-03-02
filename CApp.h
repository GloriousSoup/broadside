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
#include "TimeManager.h"
#include "gui.h"



//==============================================================================
class CApp {
    private:
        bool            Running;

        SDL_Window * window;
        SDL_GLContext context;

		  UIState ui;

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
	  int m_screenW, m_screenH;
	  TimeManager timeManager;

	  void Set2D();
    void DrawRect( int x, int y, int w, int h, const Vec4 &colour );
    void DrawRect( Rect &rect, const Vec4 &colour );

    void Set3D();
    void DrawSea();
    void DrawShip( const Vec3 &pos, SHIP_TYPE type, float orientation );
};

//==============================================================================

#endif
