#include "CApp.h"

GLShader gShipShader;
GLShader gSeaShader;

float x,y,z;
#include "BadMesh.h" 
BadMesh * ship[ST_NUM_ST];

#include "FontRender.h"
#include "gui.h"

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
	gSeaShader.SetSources( "data/sea.vert", "data/sea.frag" );
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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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
		case SDL_MOUSEBUTTONDOWN: {
			ui.m_PointerPos = IVec2( Event->motion.x, Event->motion.y );
			//printf( "MOUSEBUTTONDOWN %i,%i\n", m_PointerMove.x, m_PointerMove.y );
			ui.m_PointerDown = true;
		}
		break;
		case SDL_MOUSEBUTTONUP: {
			ui.m_PointerPos = IVec2( Event->motion.x, Event->motion.y );
			ui.m_PointerUp = true;
			//printf( "MOUSEBUTTONUP %i,%i\n", m_PointerMove.x, m_PointerMove.y );
		}
		break;
		case SDL_MOUSEMOTION: {
			ui.m_PointerPos = IVec2( Event->motion.x, Event->motion.y );
			ui.m_PointerMove += IVec2( Event->motion.xrel, Event->motion.yrel );
			//printf( "MOUSEMOTION %i,%i\n", m_PointerMove.x, m_PointerMove.y );
		}
		break;
		case SDL_MOUSEWHEEL: {
			ui.m_PointerWheel += IVec2( Event->wheel.x, Event->wheel.y );
			//printf( "MOUSEMOTION %i,%i\n", m_PointerMove.x, m_PointerMove.y );
		}
		break;
	}
}

int win_width;
int win_height;

void CApp::OnLoop() {
	RenderUpdate();

	float speed = 0.1f;
	x += speed * (kRight - kLeft);
	y += speed * (kUp - kDown);
	z += speed * (kForward - kBack);
	gShipShader.ReloadIfNecessary();
	gSeaShader.ReloadIfNecessary();
	SDL_GetWindowSize( window, &win_width, &win_height );

	{
		Rect r(200,300,30,80);
		Rect r2(310,410,30,80);
		Style s;
		s.BGColour = Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
		TXT text = "GO LEFT NOW";
		TXT text2 = "GO RIGHT SOON";
		if( IMButton( ui, 1, r, text, s ) ) {
			x -= 1.0f;
		}
		s.TextColour = Vec4( 1.0f, 0.0f, 0.0f, 1.0f );
		if( IMButton( ui, 2, r2, text2, s ) ) {
			x += 1.0f;
		}

		s.BGColour = Vec4( 0.8f, 0.8f, 0.8f, 1.0f );
		s.TextColour = Vec4( 0.0f, 0.1f, 0.4f, 1.0f );
		static Rect r3(100,200,100,200);
		IMDraggable( ui, 3, r3, "Movable R3", s );
		s.BGColour = Vec4( 0.5f, 0.5f, 0.5f, 1.0f );
		s.TextColour = Vec4( 0.0f, 0.1f, 0.9f, 1.0f );
		static Rect r4(200,300,100,200);
		IMDraggable( ui, 4, r4, "Movable R4", s );

		Rect checkR(70,100,300,320);
		s.BGColour = Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
		s.TextColour = Vec4( gOneVec3, 1.0f );
		static bool show = true;
		text = show ? "hide" : "show";
		if( IMButton( ui, 6, checkR, text, s ) ) {
			show = !show;
		}
		if( show ) {
			TXTVec tv;
			tv.push_back( "item 1" );
			tv.push_back( "item 2" );
			tv.push_back( "ninjas" );
			tv.push_back( "item 4" );
			tv.push_back( "item oops" );
			tv.push_back( "emporiatus" );
			tv.push_back( "kids" );
			tv.push_back( "zombies" );
			tv.push_back( "bunnies" );
			tv.push_back( "kittens" );
			tv.push_back( "cowboys" );
			tv.push_back( "robots" );
			tv.push_back( "pirates" );
			tv.push_back( "gold" );
			s.BGColour = Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
			s.TextColour = Vec4( 0.9f, 0.7f, 0.4f, 1.0f );
			static float scrollPos = 0.0f;
			Rect rs(100,300,300,350);
			IMScrollable( ui, 5, rs, scrollPos, tv, s );
		}
	}

}

void CApp::OnCleanup() {
	//SDL_FreeSurface(Surf_Display);
	//SDL_FreeWindow(window);
	//SDL_FreeContext(context);
	SDL_Quit();
}
void CApp::Set2D() {
	DefaultShader.Use();
	GLSetOrtho(0.0f, (float)win_width, 0.0f, (float)win_height, -1.0f, 1.0f );
	GLSetCamera( gIdentityMat );
	GLSetModel( gIdentityMat );
}
void CApp::DrawRect( int x, int y, int w, int h, const Vec4 &colour ) {
	DefaultShader.Use();
	glVertexAttribPointer(ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ATTR_VERTEX);
	glEnableVertexAttribArray(ATTR_COLOR);

	float pPos[] = {
		(float)x,(float)y,0.0f,
		(float)x+w,(float)y,0.0f,
		(float)x+w,(float)y+h,0.0f,
		(float)x,(float)y+h,0.0f
	};
	float pCol[] = {
		colour.x,colour.y,colour.z,
		colour.x,colour.y,colour.z,
		colour.x,colour.y,colour.z,
		colour.x,colour.y,colour.z,
	};
	glVertexAttribPointer(ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, pPos);
	glVertexAttribPointer(ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, pCol);

	unsigned short pInds[] = { 0,1,2, 0,2,3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, pInds);
}

void CApp::Set3D() {
	gShipShader.Use();
	GLSetPerspective( (float)M_PI / 4.0f, 1.0f, 1000.0f );
	GLSetCamera( Vec3( 0.0f, 4.0f, -6.0f ), gZeroVec3 );
}
void CApp::DrawSea() {
	gSeaShader.Use();
	GLUploadPV();
	GLSetModel( gIdentityMat );
	glVertexAttribPointer(ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ATTR_VERTEX);
	glEnableVertexAttribArray(ATTR_COLOR);

	float pPos[] = {
		-1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 1.0f
	};
	Vec3 colour( 0.05f, 0.2f, 0.3f );
	float pCol[] = {
		colour.x,colour.y,colour.z,
		colour.x,colour.y,colour.z,
		colour.x,colour.y,colour.z,
		colour.x,colour.y,colour.z,
	};
	glVertexAttribPointer(ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, pPos);
	glVertexAttribPointer(ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, pCol);

	unsigned short pInds[] = { 0,1,2, 0,2,3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, pInds);
}
void CApp::DrawShip( const Vec3 &pos, SHIP_TYPE type, float orientation ) {
	gShipShader.Use();
	GLUploadPV();
	GLSetModel( pos, orientation );
	ship[type]->DrawTriangles();
}

void CApp::OnRender() {
	glClearColor( 0.0f,0.1f,0.2f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Set2D();
	DrawGUI( ui, *this );

	Set3D();
	DrawSea();
	DrawShip( gZeroVec3, ST_BASE, z );
	DrawShip( Vec3( -x, 0.0f, y ), ST_FAT, 0.0f );
	DrawSea();

	SDL_GL_SwapWindow(window);
}

