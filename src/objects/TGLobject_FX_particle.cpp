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
#include "TGLobject_FX_particle.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_FX_particle::TGLobject_FX_particle(float x,float y,int a,float speedx,float speedy,int speeda,bool gravity,float alpha1,float alpha2,float scale1,float scale2,int duration,GLTile *t) : TGLobject(x,y,0)
{
	m_angle=a;
	m_affected_by_gavity=gravity;
	m_speedx=speedx;
	m_speedy=speedy;
	m_speeda=speeda;
	m_alpha1=alpha1;
	m_alpha2=alpha2;
	m_scale=m_scale1=scale1;
	m_scale2=scale2;
	m_duration=duration;
	m_last_tile=t;
	m_r1=m_r2=1;
	m_g1=m_g2=1;
	m_b1=m_b2=1;
} /* TGLobject_FX_particle::TGLobject_FX_particle */ 


TGLobject_FX_particle::TGLobject_FX_particle(float x,float y,int a,float speedx,float speedy,int speeda,bool gravity,
											 float r1,float r2,
											 float g1,float g2,
											 float b1,float b2,
											 float alpha1,float alpha2,float scale1,float scale2,int duration,GLTile *t) : TGLobject(x,y,0)
{
	m_angle=a;
	m_affected_by_gavity=gravity;
	m_speedx=speedx;
	m_speedy=speedy;
	m_speeda=speeda;
	m_alpha1=alpha1;
	m_alpha2=alpha2;
	m_scale=m_scale1=scale1;
	m_scale2=scale2;
	m_duration=duration;
	m_last_tile=t;
	m_r1=r1;
	m_r2=r2;
	m_g1=g1;
	m_g2=g2;
	m_b1=b1;
	m_b2=b2;
} /* TGLobject_FX_particle::TGLobject_FX_particle */ 


TGLobject_FX_particle::~TGLobject_FX_particle()
{
} /* TGLobject_FX_particle::~TGLobject_FX_particle */ 


bool TGLobject_FX_particle::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;
	if (m_cycle>m_duration) return false;

	m_x+=m_speedx;
	m_y+=m_speedy;

	{
		float f=float(m_cycle)/m_duration;
		m_scale=m_scale1*(1-f)+m_scale2*f;
	}

	if (m_affected_by_gavity) m_speedy+=2.0f/256.0f;

	m_angle+=m_speeda;
	return true;
} /* TGLobject_FX_particle::cycle */ 


void TGLobject_FX_particle::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	float f=float(m_cycle)/m_duration;

    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(m_r1*(1-f)+m_r2*f,
                          m_g1*(1-f)+m_g2*f,
                          m_b1*(1-f)+m_b2*f,
                          m_alpha1*(1-f)+m_alpha2*f,
                          transform2);
    }
} /* TGLobject_FX_particle::draw */ 


bool TGLobject_FX_particle::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_FX_particle")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_FX_particle::is_a */ 


bool TGLobject_FX_particle::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_FX_particle::is_a */ 

const char *TGLobject_FX_particle::get_class(void)
{
	return "TGLobject_FX_particle";
} /* TGLobject_FX_particle::get_class */ 
