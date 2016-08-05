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
#include "TGLobject_bullet.h"
#include "TGLobject_enemy.h"
#include "TGLobject_directionalcannon_down.h"
#include "TGLobject_FX_particle.h"
#include "TGLobject_FX_explosion2.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_directionalcannon_down::TGLobject_directionalcannon_down(float x,float y,int ao) : TGLobject_enemy(x+15,y+12,ao)
{
	m_state=0;
	m_hitpoints=12;
	m_angle=270;
} /* TGLobject_directionalcannon_down::TGLobject_directionalcannon_down */ 


TGLobject_directionalcannon_down::~TGLobject_directionalcannon_down()
{
} /* TGLobject_directionalcannon_down::~TGLobject_directionalcannon_down */ 


bool TGLobject_directionalcannon_down::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	TGLobject *ship;
	m_cycle++;
	bool in_range=false;

	ship=map->object_exists("TGLobject_ship",get_x()-320,get_y()-240,get_x()+320,get_y()+240);
	if (ship!=0) {
		// compute desired angle:
		float dx=ship->get_x()-get_x(),dy=ship->get_y()-get_y();
		float a=float((180*atan2(float(dx),float(dy)))/M_PI);
		a=90-a;
		while (a>360) a-=360;
		while (a<0) a+=360;

		if (a>=190 && a<=350) {
			// ship in valid range:
			if (m_angle<a) m_angle++;
			if (m_angle>a) m_angle--;

			if (fabs(m_angle-a)<2) in_range=true;
		} // if 
	} // if 


	switch(m_state) {
	case 0:	if (in_range) m_state=1;
			break;
	case 1: if (in_range) {
				// fire
				TGLobject *bullet;
				int a=m_angle;
				while(a<0) a+=360;
				while(a>=360) a-=360;
				bullet=new TGLobject_bullet(float(get_x()+(cos_degree(a)*14)),float(get_y()+(sin_degree(a)*14)),0,m_angle+90,2,1,GLTM->get("graphics/objects/bullet-grey.png"),this);
				map->add_object_back(bullet);
				m_state=128;
				Sound_play(SFXM->get(TGL_SFX_FOLDER "/shot"),sfx_volume);

				// Smoke:
				{
					int i;
					for(i=0;i<4;i++) {
						map->add_auxiliary_back_object(new TGLobject_FX_particle(float(get_x()+(cos_degree(a)*14)),float(get_y()+(sin_degree(a)*14)),rand()%60,float((rand()%101)-50)/256.0f,float((rand()%101)-50)/256.0f,1,false,0.25f,0,0.25f,0.75f,50,GLTM->get("graphics/objects/smoke.png")));
					} // for
				}
			} else {
				m_state=0;
			} // if
			break;
	default:
			m_state--;
	} // switch

	if (m_hit) {
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/enemyhit"),sfx_volume);
		m_hit=false;
	} // if 

	if (m_hitpoints<=0) {
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/explosion"),sfx_volume);
		map->add_auxiliary_front_object(new TGLobject_FX_explosion2(get_x(),get_y(),128,200));
		return false;
	} // if 


	return true;
} /* TGLobject_directionalcannon_down::cycle */ 


void TGLobject_directionalcannon_down::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_state==0) m_last_tile=GLTM->get("graphics/objects/directionalcannon1.png");
	if (m_state==1) m_last_tile=GLTM->get("graphics/objects/directionalcannon1.png");
	if (m_state>124) m_last_tile=GLTM->get("graphics/objects/directionalcannon2.png");
	if (m_state>120 && m_state<=124) m_last_tile=GLTM->get("graphics/objects/directionalcannon3.png");
	if (m_state>116 && m_state<=120) m_last_tile=GLTM->get("graphics/objects/directionalcannon2.png");
	if (m_state>1 && m_state<=116) m_last_tile=GLTM->get("graphics/objects/directionalcannon1.png");
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_directionalcannon_down::draw */


bool TGLobject_directionalcannon_down::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_directionalcannon_down")) return true;
	return TGLobject_enemy::is_a(c);
} /* TGLobject_directionalcannon_down::is_a */ 


bool TGLobject_directionalcannon_down::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_directionalcannon_down::is_a */ 


const char *TGLobject_directionalcannon_down::get_class(void)
{
	return "TGLobject_directionalcannon_down";
} /* TGLobject_directionalcannon_down::get_class */ 

