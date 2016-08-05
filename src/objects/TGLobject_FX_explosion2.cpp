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
#include "TGLobject_FX_explosion2.h"
#include "TGLobject_FX_particle.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_FX_explosion2::TGLobject_FX_explosion2(float x,float y,float maxsize,int duration) : TGLobject(x,y,0)
{
	m_maxsize=maxsize;
	m_duration=duration;
	m_state=0;
	m_cycle=0;
} /* TGLobject_FX_explosion2::TGLobject_FX_explosion2 */ 


TGLobject_FX_explosion2::~TGLobject_FX_explosion2()
{
} /* TGLobject_FX_explosion2::~TGLobject_FX_explosion2 */ 


bool TGLobject_FX_explosion2::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	// Wave
	if (m_cycle==0) {
		map->insert_auxiliary_front_object(new TGLobject_FX_particle(get_x(),get_y(),0,0,0,0,false,
										0.5f,0.5f,
										0.5f,0.5f,
										1,1,
										1,0,m_maxsize/512.0f,m_maxsize/32.0f,m_duration,GLTM->get("graphics/objects/ripple-back.png")));
		map->add_auxiliary_front_object(new TGLobject_FX_particle(get_x(),get_y(),0,0,0,0,false,
										0.5f,0.5f,
										0.5f,0.5f,
										1,1,
										1,0,m_maxsize/512.0f,m_maxsize/32.0f,m_duration,GLTM->get("graphics/objects/ripple-front.png")));
	} // if 

	// fire:
	{
		int i;
		int range=100;
		range=int(m_maxsize/16);
		if (range<1) range=1;
		for(i=0;i<(m_duration-m_cycle*4);i+=32) {
			int r=rand()%range;
			int a=rand()%360;
			if ((rand()%128)==0) {
				map->insert_auxiliary_front_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%360,float(r*cos_degree(a))/16,float(r*sin_degree(a))/16,4,true,
																		 0.5f,0.5f,
																		 0.5f,0.5f,
																		 0.5f,0.5f,
																		 1,0,0.25f,0.35f,m_duration/2,GLTM->get("graphics/objects/explosion.png")));
			} else {
				if (r<range/2) {
					map->add_auxiliary_front_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%360,float(r*cos_degree(a))/32,float(r*sin_degree(a))/32,4,false,
																			 1,1,
																			 1,1,
																			 1,1,
																			 1,0,0.25f,0.35f,m_duration/4,GLTM->get("graphics/objects/explosion.png")));
				} else {
					map->insert_auxiliary_front_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%360,float(r*cos_degree(a))/32,float(r*sin_degree(a))/32,4,false,
																			 0.5f,0.5f,
																			 0.5f,0.5f,
																			 1,1,
																			 1,0,0.25f,0.35f,m_duration/4,GLTM->get("graphics/objects/explosion.png")));
				} // if 
			} // if 
		} // for
	}

	// smoke:
	// ...

	// particles:
	if (m_cycle==0) {
		int i;
		int range=100;
		range=int(m_maxsize);
		if (range<1) range=1;
		for(i=0;i<m_duration/2;i++) map->add_auxiliary_back_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%60,float((rand()%(range*2+1))-range),float((rand()%(range*2+1))-range),4,true,0.5f,0.5f,0.5f,0.5f,1,1,1.0f,0,0.25f,0.25f,m_duration,GLTM->get("graphics/objects/particle1.png")));
	} // if

	m_cycle++;
	if (m_cycle>m_duration) return false;
	return true;
} /* TGLobject_FX_explosion2::cycle */ 


void TGLobject_FX_explosion2::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	m_last_tile=0;
} /* TGLobject_FX_explosion2::draw */ 


bool TGLobject_FX_explosion2::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_FX_explosion2")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_FX_explosion2::is_a */ 


bool TGLobject_FX_explosion2::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_FX_explosion2::is_a */ 


const char *TGLobject_FX_explosion2::get_class(void)
{
	return "TGLobject_FX_explosion2";
} /* TGLobject_FX_explosion2::get_class */ 
