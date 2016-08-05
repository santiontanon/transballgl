#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#include "SDL.h"

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
#include "TGLobject_button.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_button::TGLobject_button(float x,float y,int ao,int event,int type) : TGLobject(x,y,ao)
{
	m_event=event;
	m_type=type;
	m_state=0;
} /* TGLobject_button::TGLobject_button */ 


TGLobject_button::~TGLobject_button()
{
} /* TGLobject_button::~TGLobject_button */ 


bool TGLobject_button::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	if (m_state==1) {
		if (m_cycle==0) {
			map->action(m_event+1);
			Sound_play(SFXM->get(TGL_SFX_FOLDER "/switch"),sfx_volume);
		} // if 
		m_cycle++;
		if (m_cycle>8) {
			m_state=0;
			m_cycle=0;
		} // if 
	} // if 
	return true;
} /* TGLobject_button::cycle */ 


void TGLobject_button::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	switch(m_type) {
	case 0: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-red-left1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-red-left2.png");
			break;
	case 1: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-red-right1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-red-right2.png");
			break;
	case 2: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-red-up1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-red-up2.png");
			break;
	case 3: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-red-down1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-red-down2.png");
			break;
	case 4: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-purple-left1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-purple-left2.png");
			break;
	case 5: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-purple-right1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-purple-right2.png");
			break;
	case 6: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-purple-up1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-purple-up2.png");
			break;
	case 7: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-purple-down1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-purple-down2.png");
			break;
	case 8: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-blue-left1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-blue-left2.png");
			break;
	case 9: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-blue-right1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-blue-right2.png");
			break;
	case 10: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-blue-up1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-blue-up2.png");
			break;
	case 11: if (m_state==0) m_last_tile=GLTM->get("graphics/objects/button-blue-down1.png");
					   else m_last_tile=GLTM->get("graphics/objects/button-blue-down2.png");
			break;
	} // switch
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_button::draw */


bool TGLobject_button::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_button")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_button::is_a */ 


bool TGLobject_button::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_button::is_a */ 


void TGLobject_button::ball_hit(void)
{
	m_state=1;
	m_cycle=0;
} /* TGLobject_button::ball_hit */ 


const char *TGLobject_button::get_class(void)
{
	return "TGLobject_button";
} /* TGLobject_button::get_class */ 

