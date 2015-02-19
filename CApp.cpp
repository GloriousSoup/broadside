#include "CApp.h"

CApp::CApp() {
    //Surf_Display = NULL;
   window = 0;
       context = 0;

    Running = true;
}

int CApp::OnExecute() {
    if(OnInit() == false) {
        return -1;
    }

    SDL_Event Event;

    while(Running) {
        while(SDL_PollEvent(&Event)) {
            OnEvent(&Event);
        }

        OnLoop();
        OnRender();
    }

    OnCleanup();

    return 0;
}

bool CApp::OnInit() {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,    	    8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,  	    8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,   	    8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,  	    8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  	    16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,		    32);

    //SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,	    8);
    //SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,	8);
    //SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,	    8);
    //SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,	8);

    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);

    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window = SDL_CreateWindow("thing", 10, 10, 640, 480, SDL_WINDOW_OPENGL );

    if(window == NULL) {
        return false;
    }
    context = SDL_GL_CreateContext(window);
    if( context == NULL ) {
        return false;
    }


    glClearColor(0, 0, 0, 0);

    glViewport(0, 0, 640, 480);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 640, 480, 0, 1, -1);

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);

    glLoadIdentity();

    return true;
}

void CApp::OnEvent(SDL_Event* Event) {
    if(Event->type == SDL_QUIT) {
        Running = false;
    }
}

void CApp::OnLoop() {
}

void CApp::OnCleanup() {
    //SDL_FreeSurface(Surf_Display);
    //SDL_FreeWindow(window);
    //SDL_FreeContext(context);
    SDL_Quit();
}

void CApp::OnRender() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_QUADS);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
        glColor3f(1, 1, 0); glVertex3f(100, 0, 0);
        glColor3f(1, 0, 1); glVertex3f(100, 100, 0);
        glColor3f(1, 1, 1); glVertex3f(0, 100, 0);
    glEnd();

    SDL_GL_SwapWindow(window);
    //SDL_GL_SwapBuffers();
}
