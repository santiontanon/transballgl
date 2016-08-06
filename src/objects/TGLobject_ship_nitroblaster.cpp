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
#include "TGLobject_ship_nitroblaster.h"
#include "TGLobject_bullet.h"
#include "TGLobject_bullet_missile.h"
#include "TGLobject_ball.h"
#include "TGLobject_FX_particle.h"
#include "TGLobject_FX_explosion2.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_ship_nitroblaster::TGLobject_ship_nitroblaster(float x,float y,int initial_fuel) : TGLobject_ship(x,y,initial_fuel)
{
	m_scale=0.45f;
	m_thrusting=false;
	m_nitrothrusting=false;
	m_ball=0;
	m_max_fuel=50*64;
	m_fuel=(initial_fuel/2)*64;
} /* TGLobject_ship_nitroblaster::TGLobject_ship */ 


bool TGLobject_ship_nitroblaster::cycle(VirtualController *k,TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;

	// Check for collision:
	if (map->collision(this,0,0,0)) {
		if (!map->collision_with_foreground(this,0,0,0)) {
			TGLobject *obj = map->collision_with_object(this);
			if (obj->is_a("TGLobject_soft_bullet")) {
				int a=obj->get_angle()-90;
				while(a<0) a+=360;
				while(a>=360) a-=360;
				m_speed_x+=float(cos_degree(a)*64.0)/54.0f;
				m_speed_y+=float(sin_degree(a)*64.0)/54.0f;
			} else {
				Sound_play(SFXM->get(TGL_SFX_FOLDER "/explosion"),sfx_volume);
				map->add_auxiliary_front_object(new TGLobject_FX_explosion2(get_x(),get_y(),256,200));
				return false;
			}
		} else {
			Sound_play(SFXM->get(TGL_SFX_FOLDER "/explosion"),sfx_volume);
			map->add_auxiliary_front_object(new TGLobject_FX_explosion2(get_x(),get_y(),256,200));
			return false;
		}
	} // if 
			

	m_thrusting=false;
	m_nitrothrusting=false;

	if (m_fuel_recharging_timmer>0) {
		m_fuel_recharging_timmer--;
		if (m_fuel_recharging_timmer==0) {
			if (m_fuel_channel!=-1) {
				Mix_HaltChannel(m_fuel_channel);
				m_fuel_channel=-1;
			} // if 
		} // if 
	} // if 

	if (k->m_button[1] && (fabs(m_speed_x)+fabs(m_speed_y))<1) {
		TGLobject_ball *ball;

		ball=(TGLobject_ball *)map->object_exists("TGLobject_ball",get_x()-16,get_y(),get_x()+16,get_y()+64);
		if (ball!=0 && m_ball==0) {
			if (ball->attractor()) {
				if (ball->get_state()==0) map->action(0);
				ball->capture(3);
				m_ball=ball;
				Sound_play(SFXM->get(TGL_SFX_FOLDER "/takeball"),sfx_volume);
			} // if 
		} // if 

		// Attractor effect:
		{
			if ((m_cycle%6)==0) {
				map->add_auxiliary_back_object(new TGLobject_FX_particle(get_x(),get_y()+48,0,0,-1,0,false,
												0.5f,0.5f,
												0.5f,0.5f,
												1,1,
												1,0,0.75,0.5f,50,GLTM->get("graphics/objects/ripple-back.png")));
				map->add_auxiliary_front_object(new TGLobject_FX_particle(get_x(),get_y()+48,0,0,-1,0,false,
												0.5f,0.5f,
												0.5f,0.5f,
												1,1,
												1,0,0.75,0.5f,50,GLTM->get("graphics/objects/ripple-front.png")));
			} // if 
		};

	} // if 

	if (k->m_button[0] && !k->m_old_button[0] && m_fuel>40) {
		TGLobject *bullet;
		int a=m_angle-90;
		while(a<0) a+=360;
		while(a>=360) a-=360;
		bullet=new TGLobject_bullet(float(get_x()+(cos_degree(a)*12)),float(get_y()+(sin_degree(a)*12)),0,m_angle,6,1,GLTM->get("graphics/objects/bullet-purple.png"),this);
		if (m_ball!=0) bullet->exclude_for_collision(m_ball);
		map->add_object_back(bullet);
		m_fuel-=40;
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/shipshot2"),sfx_volume);
	} // if 

	if (k->m_joystick[VC_LEFT]) {
		m_angle-=4;
		if (m_angle<0) m_angle+=360;
	} // if 
	if (k->m_joystick[VC_RIGHT]) {
		m_angle+=4;
		if (m_angle>=360) m_angle-=360;
	} // if

	if (k->m_joystick[VC_UP] && !k->m_joystick[VC_DOWN] && m_fuel>0) {
		int a=m_angle-90;
		while(a<0) a+=360;
		while(a>=360) a-=360;
		m_speed_x+=float(cos_degree(a)*18.0)/256.0f;
		m_speed_y+=float(sin_degree(a)*18.0)/256.0f;
		m_thrusting=true;
		m_fuel--;
	} // if 

	if (k->m_joystick[VC_DOWN] && m_fuel>0) {
		int a=m_angle-90;
		while(a<0) a+=360;
		while(a>=360) a-=360;
		m_speed_x+=float(cos_degree(a)*54.0)/256.0f;
		m_speed_y+=float(sin_degree(a)*54.0)/256.0f;
		m_nitrothrusting=true;
		m_fuel-=2;
	} // if 


	if (m_speed_x>0) m_speed_x-=1.0f/256.0f;
	if (m_speed_x<0) m_speed_x+=1.0f/256.0f;
	m_speed_y+=2.0f/256.0f;

	if (m_speed_x>8) m_speed_x=8;
	if (m_speed_x<-8) m_speed_x=-8;
	if (m_speed_y>8) m_speed_y=8;
	if (m_speed_y<-8) m_speed_y=-8;
	m_x+=m_speed_x;
	m_y+=m_speed_y;

	if ((m_x)<0) {
		m_x=0;
		m_speed_x=0;
	} // if 
	if ((m_y)<0) {
		m_y=0;
		m_speed_y=0;
	} // if 
	if ((m_x)>(map->get_dx())) {
		m_x=float(map->get_dx());
		m_speed_x=0;
	} // if 
	if ((m_y)>(map->get_dy())) {
		m_y=float(map->get_dy());
		m_speed_y=0;
	} // if 

	// Ball attraction:
	if (m_ball!=0) {
		float dx=m_ball->get_x()-get_x();
		float dy=m_ball->get_y()-get_y();
		float d=float(sqrt(dx*dx+dy*dy));

		if (d<2) d=2;
		{
			float inc=float(28/sqrt(d));
			float inc_x=((dx/d)*inc)/256.0f;
			float inc_y=((dy/d)*inc)/256.0f;
			m_ball->set_speed_x(m_ball->get_speed_x()-inc_x);
			m_ball->set_speed_y(m_ball->get_speed_y()-inc_y);
		}

	} // if 

	if (m_thrusting) {
		if ((m_cycle%4)==0) map->add_auxiliary_back_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%60,0,0,1,false,0.125f,0,0.5f,1.5f,50,GLTM->get("graphics/objects/smoke.png")));
	} // if

	if (m_nitrothrusting) {
		if ((m_cycle%2)==0) map->add_auxiliary_back_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%60,0,0,1,false,0.2f,0,0.5f,1.5f,50,GLTM->get("graphics/objects/smoke.png")));
	} // if

	if (m_thrusting || m_nitrothrusting) {
		if (m_thrust_channel==-1) {
			m_thrust_channel=Sound_play_continuous(SFXM->get(TGL_SFX_FOLDER "/thrust"),sfx_volume);
		} // if 
	} else {
		if (m_thrust_channel!=-1) {
			Mix_HaltChannel(m_thrust_channel);
			m_thrust_channel=-1;
		} // if 
	} // if

	return true;
} /* TGLobject::cycle */ 


void TGLobject_ship_nitroblaster::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_last_mask==0) m_last_mask=GLTM->get("graphics/objects/ship-nblaster-1.png");

	if (!m_thrusting && !m_nitrothrusting) {
		m_last_tile=GLTM->get("graphics/objects/ship-nblaster-1.png");
	} // if 
	if (m_thrusting && !m_nitrothrusting) {
		if (((m_cycle/4)%2)==0) m_last_tile=GLTM->get("graphics/objects/ship-nblaster-2.png");
						   else m_last_tile=GLTM->get("graphics/objects/ship-nblaster-3.png");
	} // if 
	if (!m_thrusting && m_nitrothrusting) {
		if (((m_cycle/4)%2)==0) m_last_tile=GLTM->get("graphics/objects/ship-nblaster-4.png");
						   else m_last_tile=GLTM->get("graphics/objects/ship-nblaster-5.png");
	} // if 

    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_ship_nitroblaster::draw */


bool TGLobject_ship_nitroblaster::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_ship_nitroblaster")) return true;
	return TGLobject_ship::is_a(c);
} /* TGLobject_ship_nitroblaster::is_a */ 


bool TGLobject_ship_nitroblaster::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_ship_nitroblaster::is_a */ 


const char *TGLobject_ship_nitroblaster::get_class(void)
{
	return "TGLobject_ship_nitroblaster";
} /* TGLobject_ship_nitroblaster:get_class */ 


