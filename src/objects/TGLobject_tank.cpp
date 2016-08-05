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
#include "TGLobject_tank.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_tank::TGLobject_tank(float x,float y,int ao,int type) : TGLobject_enemy(x,y,ao)
{
	m_state=0;
	m_hitpoints=10;
	m_type=type;
} /* TGLobject_tank::TGLobject_tank */ 


TGLobject_tank::~TGLobject_tank()
{
} /* TGLobject_tank::~TGLobject_tank */ 


bool TGLobject_tank::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	// The bottom part of the tanks never disappears
	if (m_hitpoints<=0) return true;

	// Test if the tank has to go down:
	if (!map->collision_with_foreground(this,0,1,0)) {
		m_y+=1;
	} else {
/*
		{
			float vx=0,vy=0;
			if (map->collision_vector(this,&vx,&vy)) {
				if (vy>0) {
					if (vx<0 && !map->collision_with_foreground(this,0,-1,1)) m_angle++;
					if (vx>0 && !map->collision_with_foreground(this,0,-1,-1)) m_angle--;
				} // if 
			} else {
				m_y+=PRECISION_FACTOR;
				if (map->collision_vector(this,&vx,&vy)) {
					m_y-=PRECISION_FACTOR;
					if (vy>0) {
						if (vx<0 && !map->collision_with_foreground(this,0,-1,1)) m_angle++;
						if (vx>0 && !map->collision_with_foreground(this,0,-1,-1)) m_angle--;
					} // if 
				} else {
					m_y-=PRECISION_FACTOR;
				} // if
			} // if 
		}
*/		
		if (map->collision_with_foreground(this,0,0,0) &&
			!map->collision_with_foreground(this,0,-4,0)) m_y-=1;

		{
			int i;
			int m=7;
			int max_cc=0;
			int max_acc=0;

			for(i=1;i<m;i+=2) if (!map->collision_with_foreground(this,0,0,i)) max_cc=i;
			for(i=1;i<m;i+=2) if (!map->collision_with_foreground(this,0,0,-i)) max_acc=i;

			if (max_cc>max_acc) {
				if (m_angle<35) {
					m_angle++;
				} else {
					if (m_state==1) m_state=0;
				} // if 
			} // if
			if (max_acc>max_cc && m_angle>-35) {
				if (m_angle>-35) {
					m_angle--;
				} else {
					if (m_state==0) m_state=1;
				} // if 
			} // if
		}

		switch(m_state) {
		case 0: if (map->collision_with_foreground(this,1,-6,0)) {
					m_state=1;
				} else {
					m_x+=0.5f;
					m_cycle++;
				} // if 
				break;
		case 1: if (map->collision_with_foreground(this,-1,-6,0)) {
					m_state=0;
				} else {
					m_x-=0.5f;
					m_cycle--;
					if (m_cycle<0) m_cycle+=5*100;
				} // if 
				break;
		} // if 

	} // if 

	return true;
} /* TGLobject_tank::cycle */ 


void TGLobject_tank::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	int tmp=(m_cycle/2)%5;

	if (m_type==0) {
		if (tmp==0) m_last_tile=GLTM->get("graphics/objects/grey-tank-body1.png");
		if (tmp==1) m_last_tile=GLTM->get("graphics/objects/grey-tank-body2.png");
		if (tmp==2) m_last_tile=GLTM->get("graphics/objects/grey-tank-body3.png");
		if (tmp==3) m_last_tile=GLTM->get("graphics/objects/grey-tank-body4.png");
		if (tmp==4) m_last_tile=GLTM->get("graphics/objects/grey-tank-body5.png");
	} // if 
	if (m_type==1) {
		if (tmp==0) m_last_tile=GLTM->get("graphics/objects/red-tank-body1.png");
		if (tmp==1) m_last_tile=GLTM->get("graphics/objects/red-tank-body2.png");
		if (tmp==2) m_last_tile=GLTM->get("graphics/objects/red-tank-body3.png");
		if (tmp==3) m_last_tile=GLTM->get("graphics/objects/red-tank-body4.png");
		if (tmp==4) m_last_tile=GLTM->get("graphics/objects/red-tank-body5.png");
	} // if 
	if (m_type==2) {
		if (tmp==0) m_last_tile=GLTM->get("graphics/objects/green-tank-body1.png");
		if (tmp==1) m_last_tile=GLTM->get("graphics/objects/green-tank-body2.png");
		if (tmp==2) m_last_tile=GLTM->get("graphics/objects/green-tank-body3.png");
		if (tmp==3) m_last_tile=GLTM->get("graphics/objects/green-tank-body4.png");
		if (tmp==4) m_last_tile=GLTM->get("graphics/objects/green-tank-body5.png");
	} // if 
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_tank::draw */


bool TGLobject_tank::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_tank")) return true;
	return TGLobject_enemy::is_a(c);
} /* TGLobject_tank::is_a */ 


bool TGLobject_tank::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_tank::is_a */ 


const char *TGLobject_tank::get_class(void)
{
	return "TGLobject_tank";
} /* TGLobject_tank:get_class */ 



void TGLobject_tank::hit(int power)
{
} /* TGLobject_tank::is_a */ 

