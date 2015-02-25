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

//==============================================================================
class CApp {
    private:
        bool            Running;

        SDL_Window * window;
        SDL_GLContext context;

    public:
        CApp();

        int OnExecute();

    public:
        bool OnInit();
        void OnEvent(SDL_Event* Event);
        void OnLoop();
        void OnRender();
        void OnCleanup();
};

//==============================================================================

#endif
