//==============================================================================
// SDL Tutorial 1
//==============================================================================
#ifndef _CAPP_H_
#define _CAPP_H_

#if _WIN32
#include <SDL.h>
#include <SDL_opengl.h>
#else
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
