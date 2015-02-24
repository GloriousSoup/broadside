#include "CApp.h"

GLShader gShipShader;

float x,y;
#include "BadMesh.h" 
BadMesh * ship;

CApp::CApp() {
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

	RenderInit();

	gShipShader.SetSources( "data/ship.vert", "data/ship.frag" );
	ship = new BadMesh();
	ship->Load( "data/pirate-ship.obj" );

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

bool kUp,kDown,kLeft,kRight;

void CApp::OnEvent(SDL_Event* Event) {
	switch(Event->type) {
		case SDL_QUIT: Running = false; break;
		case SDL_KEYDOWN: {
			switch(Event->key.keysym.sym) {
				case SDLK_UP: kUp = true; break;
				case SDLK_DOWN: kDown = true; break;
				case SDLK_LEFT: kLeft = true; break;
				case SDLK_RIGHT: kRight = true; break;
				case SDLK_ESCAPE: Running = false; break;
			}
		}
		break;
		case SDL_KEYUP: {
			switch(Event->key.keysym.sym) {
				case SDLK_UP: kUp = false; break;
				case SDLK_DOWN: kDown = false; break;
				case SDLK_LEFT: kLeft = false; break;
				case SDLK_RIGHT: kRight = false; break;
			}
		}
		break;
	}
}

void CApp::OnLoop() {
	float speed = 1.0f;
	x += speed * (kRight - kLeft);
	y += speed * (kDown - kUp);
	gShipShader.ReloadIfNecessary();
}

void CApp::OnCleanup() {
	//SDL_FreeSurface(Surf_Display);
	//SDL_FreeWindow(window);
	//SDL_FreeContext(context);
	SDL_Quit();
}

void CApp::OnRender() {
	glClearColor( 0.4f,0.4f,0.5f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	DefaultShader.Use();
	glBegin(GL_QUADS);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
	glColor3f(1, 1, 0); glVertex3f(100, 0, 0);
	glColor3f(1, 0, 1); glVertex3f(100, 100, 0);
	glColor3f(1, 1, 1); glVertex3f(0, 100, 0);
	glEnd();


	gShipShader.Use();
	static int a = 0;
	a += 1;
	if( a & 1 ) {
		glBegin(GL_QUADS);
		glColor3f(1, 0, 0); glVertex3f(x+0, y+0, 0);
		glColor3f(1, 1, 0); glVertex3f(x+100, y+0, 0);
		glColor3f(1, 0, 1); glVertex3f(x+100, y+100, 0);
		glColor3f(1, 1, 1); glVertex3f(x+0, y+100, 0);
		glEnd();
	}
	
	GLLoadTransform(x,y,0.0f, 1.0f, 0.0f );
	ship->DrawTriangles();

	SDL_GL_SwapWindow(window);
}
