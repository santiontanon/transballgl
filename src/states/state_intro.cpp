#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/gl.h"
#include "GL/glu.h"
#endif

#include "SDL.h"
#ifdef __EMSCRIPTEN__
#include "SDL/SDL_ttf.h"
#else
#include "SDL_ttf.h"
#endif

#include "Symbol.h"
#include "BB2D.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "BB2D.h"
#include "sound.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "VirtualController.h"

#include "GLTManager.h"
#include "SFXManager.h"

#include "TGLobject.h"
#include "TGLobject_ship.h"
#include "TGLmap.h"
#include "TGL.h"
#include "TGLapp.h"
#include "TGLreplay.h"

#include "LevelPack.h"


// state cycle 0-50 : brain games appears
// state cycle 50-250 : brain games
// state cycle 250-300 : brain games disappears

// state cycle 300-350 : black


int TGLapp::intro_cycle(KEYBOARDSTATE *k)
{
	if (k->key_press(SDL_SCANCODE_ESCAPE)) return TGL_STATE_MAINMENU;
	if (k->key_press(SDL_SCANCODE_SPACE) ||
		k->key_press(SDL_SCANCODE_RETURN) ||
		!m_mouse_click_x.empty()) {
		if (!m_mouse_click_x.empty()) {
			m_mouse_click_x.pop_back();
			m_mouse_click_y.pop_back();
			m_mouse_click_button.pop_back();
		} // if 
		if (m_state_cycle<350) m_state_cycle = 350;
					      else return TGL_STATE_MAINMENU;
	} // if 

	if (m_state_cycle>1100) return TGL_STATE_MAINMENU;
	return TGL_STATE_INTRO;
} /* TGLapp::intro_cycle */ 

void TGLapp::intro_draw(void)
{
	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	if (m_state_cycle>=0 && m_state_cycle<50) {
		float f1=m_state_cycle/50.0f;
		float f2=0.75f+(m_state_cycle/250.0f)*0.25f;
		m_GLTM->get("graphics/interface/braingames.png")->draw(1,1,1,f1,SCREEN_X/2,SCREEN_Y/2,0,0,f2);
	} // if 
	if (m_state_cycle>=50 && m_state_cycle<250) {
		float f2=0.75f+(m_state_cycle/250.0f)*0.25f;
		m_GLTM->get("graphics/interface/braingames.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/2,0,0,f2);
	} // if 
	if (m_state_cycle>=250 && m_state_cycle<300) {
		float f1=(300-m_state_cycle)/50.0f;
		float f2=0.75f+(m_state_cycle/250.0f)*0.25f;
		m_GLTM->get("graphics/interface/braingames.png")->draw(1,1,1,f1,SCREEN_X/2,SCREEN_Y/2,0,0,f2);
	} // if 

	if (m_state_cycle>=350 && m_state_cycle<650) {
		float f1=(m_state_cycle-350)/300.0f;
		m_GLTM->get("graphics/interface/transball.png")->draw(1,1,1,f1*f1,SCREEN_X/2,SCREEN_Y/2*(1-f1)+(SCREEN_Y/4)*f1,0, 0.25f-f1*0.25f ,2*(1-f1)+f1*0.5f);
	} // if 

	if (m_state_cycle>=650) {		

		if (m_state_cycle>=700 && m_state_cycle<1100) {
			float f1=(m_state_cycle-700)/400.0f;
			m_GLTM->get("graphics/interface/gl.png")->draw(1,1,1,f1,SCREEN_X/2,SCREEN_Y/2,0,0,0.9f+f1*0.1f);
		} // if 
		if (m_state_cycle>=1100) {
			m_GLTM->get("graphics/interface/gl.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/2,0,0,1);
		} // if 
	
		m_GLTM->get("graphics/interface/transball.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/4,0,0,0.5f);
	} // if 
	
} /* TGLapp::intro_draw */ 

