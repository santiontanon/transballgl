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
#ifdef __EMSCRIPTEN__
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif


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
#include "TGLmap.h"
#include "collision.h"

#include "debug.h"


TGLobject::TGLobject(GLTile *t,float x,float y,int animation_offset)
{
	m_x=x;
	m_y=y;
	m_angle=0;
	m_scale=1;
	m_last_tile=t;
	m_last_mask=t;
	m_state=0;
	m_cycle=0;
	m_animation_offset=animation_offset;
	
	m_controller=0;
	
	m_collision_cache = 0;
	
} /* TGLobject::TGLobject */ 



TGLobject::TGLobject(float x,float y,int animation_offset)
{
	m_x=x;
	m_y=y;
	m_angle=0;
	m_scale=1;
	m_last_tile=0;
	m_last_mask=0;
	m_state=0;
	m_cycle=0;
	m_animation_offset=animation_offset;

	m_controller=0;
	
	m_collision_cache = 0;
	
} /* TGLobject::TGLobject */ 


TGLobject::~TGLobject()
{
	m_excluded_from_collision.clear();
	if (m_collision_cache!=0) {
		SDL_FreeSurface(m_collision_cache);
		m_collision_cache = 0;
	}
} /* TGLobject::~TGLobject */ 


void TGLobject::draw(glm::mat4 transform, class GLTManager *GLTM)
{
} // TGLobject::draw



bool TGLobject::cycle(VirtualController *k,TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;
	return true;
} /* TGLobject::cycle */ 


bool TGLobject::editor_cycle(TGLmap *map,GLTManager *GLTM)
{
	m_cycle++;
	return true;
} /* TGLobject::editor_cycle */ 


GLTile *TGLobject::get_last_tile(void)
{
	return m_last_tile;
} /* TGLobject::get_last_tile */ 


GLTile *TGLobject::get_last_mask(void)
{
	if (m_last_mask!=0) return m_last_mask;
	return m_last_tile;
} /* TGLobject::get_last_mask */ 


float TGLobject::get_x(void)
{
	return m_x;
} /* TGLobject::get_x */ 


float TGLobject::get_y(void)
{
	return m_y;
} /* TGLobject::get_y */ 


void TGLobject::set_x(float x)
{
	m_x=x;
} /* TGLobject::set_x */ 


void TGLobject::set_y(float y)
{
	m_y=y;
} /* TGLobject::set_y */ 


int TGLobject::get_angle(void)
{
	return m_angle;
} /* TGLobject::get_angle */ 


float TGLobject::get_scale(void)
{
	return m_scale;
} /* TGLobject::get_scale */ 


int TGLobject::get_state(void)
{
	return m_state;
} /* TGLobject::get_state */ 


void TGLobject::set_state(int s)
{
	m_state=s;
} /* TGLobject::set_state */ 


int TGLobject::get_cycle(void)
{
	return m_cycle;
} /* TGLobject::get_cycle */ 


void TGLobject::set_cycle(int c)
{
	m_cycle = c;
} /* TGLobject::set_cycle */ 


int TGLobject::get_animation_offset(void)
{
	return m_animation_offset;
} /* TGLobject::get_animation_offset */ 


void TGLobject::set_animation_offset(int s)
{
	m_animation_offset=s;
} /* TGLobject::set_animation_offset */ 


bool TGLobject::is_a(Symbol *c)
{
	return c->cmp("TGLobject");
} /* TGLobject::is_a */ 


bool TGLobject::is_a(const char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject::is_a */ 


const char *TGLobject::get_class(void)
{
	return "TGLobject";
} /* TGLobject::get_class */ 


void TGLobject::exclude_for_collision(TGLobject *o)
{
	m_excluded_from_collision.push_back(o);
} /* TGLobject::exclude_for_collision */ 


void TGLobject::reconsider_for_collision(TGLobject *o)
{
	m_excluded_from_collision.remove(o);
} /* TGLobject::reconsider_for_collision */ 


bool TGLobject::check_collision(TGLobject *o)
{
    for(TGLobject *o2:m_excluded_from_collision)
        if (o==o2) return false;
    return true;
//	return !(m_excluded_from_collision.MemberRefP(o));
} /* TGLobject::check_collision */ 


void TGLobject::set_controller(int c)
{
	m_controller=c;
} /* TGLobject::set_controller */ 


int TGLobject::get_controller(void)
{
	return m_controller;
} /* TGLobject::get_controller */ 



bool TGLobject::collision(TGLobject *o)
{
	GLTile *mask;
	SDL_Surface *sfc=0;
	float x,y;
	int hx,hy,angle;
	float s1;

	mask=get_last_mask();
	if (mask==0) return false;
	sfc=mask->getSurface();
	x=get_x();
	y=get_y();
	hx=mask->get_hot_x();
	hy=mask->get_hot_y();
	angle=get_angle();
	s1=get_scale();

	if (o!=this && o->check_collision(this) && this->check_collision(o)) {
		GLTile *mask2;
		SDL_Surface *sfc2=0;
		float x2,y2;
		int hx2,hy2,angle2;
		float s2;

		mask2=o->get_last_mask();
		if (mask2!=0) {
			sfc2=mask2->getSurface();
			x2=o->get_x();
			y2=o->get_y();
			hx2=mask2->get_hot_x();
			hy2=mask2->get_hot_y();
			angle2=o->get_angle();
			s2=o->get_scale();

			if (::collision(this,sfc,x,y,hx,hy,angle,s1,o,sfc2,x2,y2,hx2,hy2,angle2,s2)) return true;
		} // if 
	} // if 

	return false;
} /* TGLobject::collision */ 


SDL_Surface *TGLobject::get_collision_cache(float angle,float scale,SDL_Surface *source) {
	if (angle == m_collision_cache_angle && 
		scale == m_collision_cache_scale &&
		source == m_collision_cache_source)
		return m_collision_cache;
	return 0;
}

void TGLobject::set_collision_cache(SDL_Surface *s, float angle, float scale, SDL_Surface *source) {
	if (m_collision_cache!=0) SDL_FreeSurface(m_collision_cache);
	m_collision_cache = s;
	m_collision_cache_angle = angle;
	m_collision_cache_scale = scale;
	m_collision_cache_source = source;
}
