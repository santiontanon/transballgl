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
#include "GL/glew.h"
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
#include "TGLobject_enemy.h"
#include "TGLobject_laser_down.h"
#include "TGLobject_FX_explosion2.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_laser_down::TGLobject_laser_down(float x,float y,int ao) : TGLobject_enemy(x,y,ao)
{
	m_hitpoints=16;
} /* TGLobject_laser_down::TGLobject_laser_down */ 


TGLobject_laser_down::~TGLobject_laser_down()
{
} /* TGLobject_laser_down::~TGLobject_laser_down */ 


bool TGLobject_laser_down::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;

	if (m_hit) {
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/enemyhit"),sfx_volume);
		m_hit=false;
	} // if 

	if (m_hitpoints<=0) {
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/explosion"),sfx_volume);
		map->add_auxiliary_front_object(new TGLobject_FX_explosion2(get_x()+20,get_y()+16,128,200));
		return false;
	} // if 

	return true;
} /* TGLobject_laser_down::cycle */ 


void TGLobject_laser_down::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	int local_cycle=((m_animation_offset+m_cycle)/96)%2;
	if (local_cycle==0) m_last_tile=GLTM->get("graphics/objects/laser-down1.png");
				   else m_last_tile=GLTM->get("graphics/objects/laser-down2.png");
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_laser_down::draw */


bool TGLobject_laser_down::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_laser_down")) return true;
	return TGLobject_enemy::is_a(c);
} /* TGLobject_laser_down::is_a */ 


bool TGLobject_laser_down::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_laser_down::is_a */ 


const char *TGLobject_laser_down::get_class(void)
{
	return "TGLobject_laser_down";
} /* TGLobject_laser_down::get_class */ 

