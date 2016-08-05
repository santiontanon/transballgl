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
#include "TGLobject_laser_vertical.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_laser_vertical::TGLobject_laser_vertical(float x,float y,int ao,TGLobject *l1,TGLobject *l2) : TGLobject(x,y,ao)
{
	m_l1=l1;
	m_l2=l2;
} /* TGLobject_laser_vertical::TGLobject_laser_vertical */ 


TGLobject_laser_vertical::~TGLobject_laser_vertical()
{
} /* TGLobject_laser_vertical::~TGLobject_laser_vertical */ 


bool TGLobject_laser_vertical::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;

	if (!map->object_exists(m_l1) || !map->object_exists(m_l2)) return false;
	
	return true;
} /* TGLobject_laser_vertical::cycle */ 


void TGLobject_laser_vertical::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	int local_cycle=((m_animation_offset+m_cycle)/96)%2;
	if (local_cycle==1) {
		local_cycle=0;
		if ((m_cycle%32)==0) local_cycle=1;
		if ((m_cycle%32)==3) local_cycle=1;
		if ((m_cycle%32)==11) local_cycle=1;
		if ((m_cycle%32)==20) local_cycle=1;
		if (local_cycle==0) m_last_tile=GLTM->get("graphics/objects/laser-vertical1.png");
					   else m_last_tile=GLTM->get("graphics/objects/laser-vertical2.png");
        if (m_last_tile!=0) {
            glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
            if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
            if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
            m_last_tile->draw(transform2);
        }
	} else {
		m_last_tile=0;
	} // if 
} /* TGLobject_laser_vertical::draw */ 


bool TGLobject_laser_vertical::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_laser_vertical")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_laser_vertical::is_a */ 


bool TGLobject_laser_vertical::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_laser_vertical::is_a */ 


const char *TGLobject_laser_vertical::get_class(void)
{
	return "TGLobject_laser_vertical";
} /* TGLobject_laser_vertical:get_class */ 

