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
#include "TGLobject_radar.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_radar::TGLobject_radar(float x,float y,int ao) : TGLobject(x,y,ao)
{
} /* TGLobject_radar::TGLobject_radar */ 


TGLobject_radar::~TGLobject_radar()
{
} /* TGLobject_radar::~TGLobject_radar */ 


void TGLobject_radar::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	int local_cycle=((m_animation_offset+m_cycle)%64)/8;
	if (local_cycle==0) m_last_tile=GLTM->get("graphics/objects/radar1.png");
	if (local_cycle==1) m_last_tile=GLTM->get("graphics/objects/radar2.png");
	if (local_cycle==2) m_last_tile=GLTM->get("graphics/objects/radar3.png");
	if (local_cycle==3) m_last_tile=GLTM->get("graphics/objects/radar4.png");
	if (local_cycle==4) m_last_tile=GLTM->get("graphics/objects/radar5.png");
	if (local_cycle==5) m_last_tile=GLTM->get("graphics/objects/radar4.png");
	if (local_cycle==6) m_last_tile=GLTM->get("graphics/objects/radar3.png");
	if (local_cycle==7) m_last_tile=GLTM->get("graphics/objects/radar2.png");
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_radar::draw */


bool TGLobject_radar::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_radar")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_radar::is_a */ 


bool TGLobject_radar::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_radar::is_a */ 


const char *TGLobject_radar::get_class(void)
{
	return "TGLobject_radar";
} /* TGLobject_radar:get_class */ 

