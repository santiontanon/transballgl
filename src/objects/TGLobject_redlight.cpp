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
#include "TGLobject_redlight.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_redlight::TGLobject_redlight(float x,float y,int ao,int type) : TGLobject(x,y,ao)
{
	m_type = type;
} /* TGLobject_redlight::TGLobject_redlight */ 


TGLobject_redlight::~TGLobject_redlight()
{
} /* TGLobject_redlight::~TGLobject_redlight */ 


void TGLobject_redlight::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	int local_cycle=(m_animation_offset+m_cycle)%48;
	if (m_type==0) {
		if (local_cycle<24) m_last_tile=GLTM->get("graphics/objects/red-light1.png");
					   else m_last_tile=GLTM->get("graphics/objects/red-light2.png");
	} // if
	if (m_type==1) {
		if (local_cycle<24) m_last_tile=GLTM->get("graphics/objects/red-light1-snow.png");
					   else m_last_tile=GLTM->get("graphics/objects/red-light2-snow.png");
	} // if 
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_redlight::draw */


bool TGLobject_redlight::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_redlight")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_redlight::is_a */ 


bool TGLobject_redlight::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_redlight::is_a */ 


const char *TGLobject_redlight::get_class(void)
{
	return "TGLobject_redlight";
} /* TGLobject_redlight:get_class */ 

