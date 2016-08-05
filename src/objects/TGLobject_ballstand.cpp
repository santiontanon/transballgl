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
#include "TGLobject_ballstand.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_ballstand::TGLobject_ballstand(float x,float y,int animation_offset) : TGLobject(x,y,animation_offset)
{
} /* TGLobject_ballstand::TGLobject_ballstand */ 


TGLobject_ballstand::~TGLobject_ballstand()
{
} /* TGLobject_ballstand::~TGLobject_ballstand */ 


void TGLobject_ballstand::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	m_last_tile=GLTM->get("graphics/objects/pipe-ball-stand1.png");
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_ballstand::draw */


bool TGLobject_ballstand::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_ballstand")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_ballstand::is_a */ 


bool TGLobject_ballstand::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_ballstand::is_a */ 


const char *TGLobject_ballstand::get_class(void)
{
	return "TGLobject_ballstand";
} /* TGLobject_ballstand::get_class */ 

