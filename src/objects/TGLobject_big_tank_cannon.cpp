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
#include "TGLobject_ship.h"
#include "TGLobject_bullet.h"
#include "TGLobject_enemy.h"
#include "TGLobject_big_tank_cannon.h"
#include "TGLobject_FX_particle.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_big_tank_cannon::TGLobject_big_tank_cannon(float x,float y,int ao,TGLobject_enemy *tank) : TGLobject_enemy(x+15,y+12,ao)
{
	m_state=0;
	m_hitpoints=12;
	m_angle=270;
	m_tank=tank;
} /* TGLobject_big_tank_cannon::TGLobject_big_tank_cannon */ 


TGLobject_big_tank_cannon::~TGLobject_big_tank_cannon()
{
} /* TGLobject_big_tank_cannon::~TGLobject_big_tank_cannon */ 


bool TGLobject_big_tank_cannon::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	TGLobject_ship *ship;
	m_cycle++;
	bool in_range=false;

	if (m_hit) {
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/enemyhit"),sfx_volume);
		m_hit=false;
	} // if 

	if (m_tank->get_hitpoints()<m_hitpoints) m_hitpoints=m_tank->get_hitpoints();
	if (m_tank->get_hitpoints()>m_hitpoints) m_tank->set_hitpoints(m_hitpoints);

	{
		int a=-m_tank->get_angle();
		float ox=0,oy=-8;
		float offs_x;
		float offs_y;

		while(a<0) a+=360;
		while(a>=360) a-=360;
		offs_x=float(cos_degree(a)*ox+sin_degree(a)*oy);
		offs_y=float(cos_degree(a)*oy-sin_degree(a)*ox);

		m_x=(m_tank->get_x()+offs_x);
		m_y=(m_tank->get_y()+offs_y);
	}


	ship=(TGLobject_ship *)map->object_exists("TGLobject_ship",get_x()-320,get_y()-240,get_x()+320,get_y()+240);
	if (ship!=0) {
		// compute desired angle:
		float dx=ship->get_x()-get_x(),dy=ship->get_y()-get_y();
		float angle=float((180*atan2(dx,dy))/M_PI);
		angle=90-angle;
		while (angle>360) angle-=360;
		while (angle<0) angle+=360;

		{
			int a;
			float best_a=angle;
			float s_sx=ship->get_speedx();
			float s_sy=ship->get_speedy();
			float b_sx,b_sy;
			float d;
			float ls,lb;
			float error,min_error=10000;

			int min=190,max=350;
			/* Copute the error given an angle "alpha": */ 
			for(a=min;a<max;a+=1) {
				b_sx=float(2*cos_degree(a));
				b_sy=float(2*sin_degree(a));

				d=s_sy*b_sx-s_sx*b_sy;
				if (d!=0) {
					ls=(dx*b_sy-dy*b_sx)/d;
					lb=(s_sy*dx-s_sx*dy)/d;

					if (lb>0) {
						error=float(fabs(ls-lb));
					} else {
						error=10000;
					} // if 
				} else {
					error=10000;
				} // if 

				if (error<min_error) {
					best_a=float(a);
					min_error=error;
				} // if 
			} // for 

			while (best_a>360) best_a-=360;
			while (best_a<0) best_a+=360;

			if (float(fabs(angle-best_a))>60 &&
				float(fabs(angle-best_a))<300) {
				float d=angle-best_a;

				if (d>0 && d<180) best_a=angle-60;
						     else best_a=angle+60;
			} // if 

			angle=best_a;
		}	
		
		if (angle>=190 && angle<=350) {
			// ship in valid range:
			if (m_angle<angle) m_angle++;
			if (m_angle>angle) m_angle--;
			if (m_angle<angle) m_angle++;
			if (m_angle>angle) m_angle--;
			if (fabs(m_angle-int(angle))<2) in_range=true;
		} // if 
	} // if 


	switch(m_state) {
	case 0:	if (in_range) m_state=1;
			break;
	case 1: if (in_range) {
				// fire
				TGLobject *bullet;
				int a=m_angle;
				while(a<0) a+=360;
				while(a>=360) a-=360;
				bullet=new TGLobject_bullet(float(get_x()+(cos_degree(a)*14)),float(get_y()+(sin_degree(a)*14)),0,m_angle+90,2,1,GLTM->get("graphics/objects/bullet-grey.png"),this);
				bullet->exclude_for_collision(m_tank);
				map->add_object_back(bullet);
				m_state=128;
				Sound_play(SFXM->get(TGL_SFX_FOLDER "/shot"),sfx_volume);

				// Smoke:
				{
					int i;
					for(i=0;i<4;i++) {
						map->add_auxiliary_back_object(new TGLobject_FX_particle(float(get_x()+(cos_degree(a)*14)),float(get_y()+(sin_degree(a)*14)),rand()%60,float((rand()%101)-50)/256.0f,float((rand()%101)-50)/256.0f,1,false,0.25f,0,0.25f,0.75f,50,GLTM->get("graphics/objects/smoke.png")));
					} // for
				}
			} else {
				m_state=0;
			} // if
			break;
	default:
			m_state--;
	} // switch

	if (m_hit) {
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/enemyhit"),sfx_volume);
		m_hit=false;
	} // if 

	if (m_hitpoints<=0) return false;

	return true;
} /* TGLobject_big_tank_cannon::cycle */ 


void TGLobject_big_tank_cannon::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_last_tile==0) m_last_tile=GLTM->get("graphics/objects/grey-tank-cannon.png");
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_big_tank_cannon::draw */


bool TGLobject_big_tank_cannon::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_big_tank_cannon")) return true;
	return TGLobject_enemy::is_a(c);
} /* TGLobject_big_tank_cannon::is_a */ 


bool TGLobject_big_tank_cannon::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_big_tank_cannon::is_a */ 


const char *TGLobject_big_tank_cannon::get_class(void)
{
	return "TGLobject_big_tank_cannon";
} /* TGLobject_big_tank_cannon::get_class */ 

