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
#include "TGLobject_spike_right.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_spike_right::TGLobject_spike_right(float x,float y,int ao) : TGLobject(x,y,ao)
{
	m_start_x=m_x;
} /* TGLobject_spike_right::TGLobject_spike_right */ 


TGLobject_spike_right::~TGLobject_spike_right()
{
} /* TGLobject_spike_right::~TGLobject_spike_right */ 


bool TGLobject_spike_right::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	int tmp;

	m_cycle++;

	tmp=(m_cycle/2)%64;

	if (tmp<32) {
		m_x=m_start_x+(32-tmp);
	} else {
		m_x=m_start_x+(tmp-32);
	} // if 
		
	return true;
} /* TGLobject_spike_right::cycle */ 


bool TGLobject_spike_right::editor_cycle(TGLmap *map,GLTManager *GLTM)
{
	int tmp;

	m_cycle++;

	tmp=((m_animation_offset+m_cycle)/2)%64;

	if (tmp<32) {
		m_x=m_start_x+(32-tmp);
	} else {
		m_x=m_start_x+(tmp-32);
	} // if 
		
	return true;
} /* TGLobject_spike_right::editor_cycle */ 


void TGLobject_spike_right::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_last_tile==0) m_last_tile=GLTM->get("graphics/objects/spike-right.png");
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_spike_right::draw */


bool TGLobject_spike_right::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_spike_right")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_spike_right::is_a */ 


bool TGLobject_spike_right::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_spike_right::is_a */ 


const char *TGLobject_spike_right::get_class(void)
{
	return "TGLobject_spike_right";
} /* TGLobject_spike_right:get_class */ 


