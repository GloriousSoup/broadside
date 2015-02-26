#include "CApp.h"

GLShader gShipShader;

float x,y,z;
#include "BadMesh.h" 
BadMesh * ship[ST_NUM_ST];

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

	int S = 800/16;
	int ww = 16*S;
	int hh = 9*S;

	window = SDL_CreateWindow("thing", 10, 10, ww, hh, SDL_WINDOW_OPENGL );

	if(window == NULL) {
		return false;
	}
	context = SDL_GL_CreateContext(window);
	if( context == NULL ) {
		return false;
	}

#ifdef _WIN32
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		printf("Glew can't run\n");
		exit(1);
	}
	//if (!glewIsSupported("glCreateProgram")) {
		//printf("Aaargh\n");
		//exit(1);
	//}
#endif

	RenderInit();

	gShipShader.SetSources( "data/ship.vert", "data/ship.frag" );
	const char *meshnames[] = {
		"data/pirate-ship.obj",
		"data/pirate-ship-large.obj",
		"data/pirate-ship-fat.obj",
		"data/pirate-ship-giant.obj"
	};
	for( int i = 0; i < ST_NUM_ST; ++i ) {
		ship[i] = new BadMesh();
		ship[i]->Load( meshnames[i] );
	}

	glClearColor(0, 0, 0, 0);

	glViewport(0, 0, ww, hh);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, ww, hh, 0, 1, -1);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);

	glLoadIdentity();

	return true;
}

bool kUp,kDown,kLeft,kRight,kForward,kBack;

void CApp::OnEvent(SDL_Event* Event) {
	switch(Event->type) {
		case SDL_QUIT: Running = false; break;
		case SDL_KEYDOWN: {
			switch(Event->key.keysym.sym) {
				case SDLK_UP: kUp = true; break;
				case SDLK_DOWN: kDown = true; break;
				case SDLK_LEFT: kLeft = true; break;
				case SDLK_RIGHT: kRight = true; break;
				case SDLK_HOME: kForward = true; break;
				case SDLK_END: kBack = true; break;
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
				case SDLK_HOME: kForward = false; break;
				case SDLK_END: kBack = false; break;
			}
		}
		break;
	}
}

int win_width;
int win_height;

void CApp::OnLoop() {
	float speed = 0.1f;
	x += speed * (kRight - kLeft);
	y += speed * (kUp - kDown);
	z += speed * (kForward - kBack);
	gShipShader.ReloadIfNecessary();
	SDL_GetWindowSize( window, &win_width, &win_height );
}

void CApp::OnCleanup() {
	//SDL_FreeSurface(Surf_Display);
	//SDL_FreeWindow(window);
	//SDL_FreeContext(context);
	SDL_Quit();
}
void CApp::Set2D() {
	DefaultShader.Use();
	GLSetOrtho(0.0f, win_width, 0.0f, win_height, -1.0f, 1.0f );
	GLSetCamera( gIdentityMat );
	GLSetModel( gIdentityMat );
}
void CApp::DrawRect( int x, int y, int w, int h, const Vec4 &colour ) {
	glBegin(GL_QUADS);
	glColor3f(colour.x, colour.y, colour.z); glVertex3f(x, y, 0);
	glColor3f(colour.x, colour.y, colour.z); glVertex3f(x+w, y, 0);
	glColor3f(colour.x, colour.y, colour.z); glVertex3f(x+w, y+h, 0);
	glColor3f(colour.x, colour.y, colour.z); glVertex3f(x, y+h, 0);
	glEnd();
}

void CApp::Set3D() {
	gShipShader.Use();
	GLSetPerspective( M_PI / 4.0f, 1.0f, 1000.0f );
	GLSetCamera( Vec3( 0.0f, 4.0f, -6.0f ), gZeroVec3 );
}
void CApp::DrawShip( const Vec3 &pos, SHIP_TYPE type, float orientation ) {
	GLSetModel( pos, orientation );
	ship[type]->DrawTriangles();
}

void CApp::OnRender() {
#if 1
	glClearColor( 0.1f,0.2f,0.3f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Set2D();
	DrawRect( x + 40.0f, y + 40.f, 100, 100, Vec4( 0.5f,0.5f,0.5f,1.0f) );
	DrawRect( 240.0f, 240.f, x+100, y+100, Vec4( 1.0f,1.0f,0.1f,1.0f) );

	Set3D();
	DrawShip( gZeroVec3, ST_BASE, z );

	DrawShip( Vec3( -x, 0.0f, y ), ST_FAT, 0.0f );

#else
	glClearColor( 0.4f,0.4f,0.5f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	Perspective();

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
	GLSetOrtho(0.0f, win_width, 0.0f, win_height, -1.0f, 1.0f );
	GLSetCamera( gIdentityMat );
	GLSetModel( gIdentityMat );
	if( a & 1 ) {
		glBegin(GL_QUADS);
		glColor3f(1, 0, 0); glVertex3f(x+0, y+0, 0);
		glColor3f(1, 1, 0); glVertex3f(x+100, y+0, 0);
		glColor3f(1, 0, 1); glVertex3f(x+100, y+100, 0);
		glColor3f(1, 1, 1); glVertex3f(x+0, y+100, 0);
		glEnd();
	}

	
	GLSetPerspective( M_PI / 4.0f, 1.0f, 1000.0f );
	GLSetCamera( Vec3( 0.0f, 4.0f, -6.0f ), gZeroVec3 );
	GLSetModel( Vec3( x, y, z ), 0.0f );
	ship->DrawTriangles();
#endif

	SDL_GL_SwapWindow(window);
}
