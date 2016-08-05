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
#include "TGLobject_cannon_right.h"
#include "TGLobject_FX_particle.h"
#include "TGLobject_FX_explosion2.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_cannon_right::TGLobject_cannon_right(float x,float y,int ao,int type) : TGLobject_enemy(x,y,ao)
{
	m_type=type;
	m_state=0;
	m_hitpoints=4;
} /* TGLobject_cannon_right::TGLobject_cannon_right */ 


TGLobject_cannon_right::~TGLobject_cannon_right()
{
} /* TGLobject_cannon_right::~TGLobject_cannon_right */ 


bool TGLobject_cannon_right::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	TGLobject *ship;
	m_cycle++;

	switch(m_state) {
	case 0:	ship=map->object_exists("TGLobject_ship",get_x()-320,get_y()-16,get_x(),get_y()+48);
			if (ship!=0) m_state=1;
			break;
	case 1: ship=map->object_exists("TGLobject_ship",get_x()-320,get_y()-16,get_x(),get_y()+48);
			if (ship!=0) {
				// fire
				map->add_object_back(new TGLobject_bullet(get_x(),get_y()+14,0,270,2,1,GLTM->get("graphics/objects/bullet-grey.png"),this));
				m_state=128;
				Sound_play(SFXM->get(TGL_SFX_FOLDER "/shot"),sfx_volume);

				// Smoke:
				{
					int i;
					for(i=0;i<4;i++) {
						map->add_auxiliary_back_object(new TGLobject_FX_particle(get_x(),get_y()+14,rand()%60,float((rand()%101)-50)/256.0f,float((rand()%101)-50)/256.0f,1,false,0.25f,0,0.25f,0.75f,50,GLTM->get("graphics/objects/smoke.png")));
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
		map->add_auxiliary_front_object(new TGLobject_FX_explosion2(get_x()+12,get_y()+16,128,200));
		return false;
	} // if 


	return true;
} /* TGLobject_cannon_right::cycle */ 


void TGLobject_cannon_right::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	switch(m_type) {
	case 0:
		if (m_state==0) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right1.png");
		if (m_state==1) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right2.png");
		if (m_state>126) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right3.png");
		if (m_state>124 && m_state<=126) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right4.png");
		if (m_state>122 && m_state<=124) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right5.png");
		if (m_state>118 && m_state<=122) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right4.png");
		if (m_state>114 && m_state<=118) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right3.png");
		if (m_state>1 && m_state<=114) m_last_tile=GLTM->get("graphics/objects/cannon-rock-right2.png");
		break;
	case 1:
		if (m_state==0) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right1.png");
		if (m_state==1) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right2.png");
		if (m_state>126) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right3.png");
		if (m_state>124 && m_state<=126) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right4.png");
		if (m_state>122 && m_state<=124) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right5.png");
		if (m_state>118 && m_state<=122) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right4.png");
		if (m_state>114 && m_state<=118) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right3.png");
		if (m_state>1 && m_state<=114) m_last_tile=GLTM->get("graphics/objects/cannon-techno-right2.png");
		break;
	} // switch
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_cannon_right::draw */


bool TGLobject_cannon_right::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_cannon_right")) return true;
	return TGLobject_enemy::is_a(c);
} /* TGLobject_cannon_right::is_a */ 


bool TGLobject_cannon_right::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_cannon_right::is_a */ 


const char *TGLobject_cannon_right::get_class(void)
{
	return "TGLobject_cannon_right";
} /* TGLobject_cannon_right::get_class */ 

