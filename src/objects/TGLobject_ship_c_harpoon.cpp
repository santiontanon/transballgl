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
#include "TGLobject_ship_chain.h"
#include "TGLobject_ship_grabber.h"
#include "TGLobject_ship_c_harpoon.h"
#include "TGLobject_bullet.h"
#include "TGLobject_bullet_missile.h"
#include "TGLobject_ball.h"
#include "TGLmap.h"

#include "TGLobject_FX_particle.h"
#include "TGLobject_FX_explosion2.h"

#include "debug.h"




TGLobject_ship_c_harpoon::TGLobject_ship_c_harpoon(float x,float y,int initial_fuel) : TGLobject_ship(x,y,initial_fuel)
{

	m_scale=0.45f;
	m_thrusting=false;
	m_ball=0;
	m_max_fuel=50*64;
	m_fuel=(initial_fuel/2)*64;

	m_grabber=0;
	
	m_next_join_x=x;
	m_next_join_y=y;

	m_chain_length=INITIAL_CHAIN_SIZE*(CHAIN_LINK_LENGTH*1.5f);

} /* TGLobject_ship_c_harpoon::TGLobject_ship */ 


TGLobject_ship_c_harpoon::~TGLobject_ship_c_harpoon()
{
	if (m_thrust_channel!=-1) {
		Mix_HaltChannel(m_thrust_channel);
		m_thrust_channel=-1;
	} // if 

	while(!m_chain.empty()) m_chain.pop_back();
} /* TGLobject_ship_c_harpoon::~TGLobject_ship */ 


void TGLobject_ship_c_harpoon::set_next_join(float x,float y)
{
	m_next_join_x=x;
	m_next_join_y=y;
} /* TGLobject_ship_c_harpoon::set_next_join */ 


float TGLobject_ship_c_harpoon::get_chain_length(void)
{
	return m_chain_length;
} /* TGLobject_ship_c_harpoon::get_chain_length */ 


void TGLobject_ship_c_harpoon::chain_cycle(void)
{
//	m_x=(m_x*(1-CHAIN_TENSION)+m_next_join_x*CHAIN_TENSION);
//	m_y=(m_y*(1-CHAIN_TENSION)+m_next_join_y*CHAIN_TENSION);

	if (!m_chain.empty()) {
        m_chain.front()->set_previous_join(m_x,m_y);
	} // if 

	// Execute chain cycles 10 times:
	{
		int i;
		for(i=0;i<20;i++) {
            for(TGLobject_ship_chain *link:m_chain) {
                link->chain_cycle();
            }
			if (m_ball!=0) m_grabber->chain_cycle_ball();
		} // for
	}
} /* TGLobject_ship_c_harpoon::chain_cycle */ 


bool TGLobject_ship_c_harpoon::cycle(VirtualController *k,TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;

	if (m_chain.empty()) {
		// Initially, create a chain of length 8:
		int i;
		TGLobject_ship_chain *chain,*previous_chain;
		TGLobject_ship_grabber *grabber;

		previous_chain=0;
		for(i=0;i<INITIAL_CHAIN_SIZE;i++) {
			chain=new TGLobject_ship_chain(m_x,m_y+i*6);
			chain->set_previous_join(m_x,m_y+i*CHAIN_LINK_LENGTH);
			chain->set_next_join(m_x,m_y+(i+1)*CHAIN_LINK_LENGTH);
			m_chain.push_back(chain);

			if (previous_chain==0) {
				chain->set_previous(this);
			} else {
				chain->set_previous(previous_chain);
				previous_chain->set_next(chain);
			} // if 
		
			map->add_auxiliary_back_object(chain);

			previous_chain=chain;
		} // for 

		grabber=new TGLobject_ship_grabber(m_x,m_y+i*CHAIN_LINK_LENGTH);
		grabber->set_previous_join(m_x,m_y+i*CHAIN_LINK_LENGTH);
		previous_chain->set_next(grabber);
		grabber->set_chain(previous_chain);
		grabber->set_ship(this);
		m_grabber=grabber;
		
		map->add_auxiliary_back_object(grabber);
	} // if 

	// Check for collision:
	if (map->collision(this,0,0,0)) {
		if (!map->collision_with_foreground(this,0,0,0)) {
			TGLobject *obj = map->collision_with_object(this);
			if (obj->is_a("TGLobject_soft_bullet")) {
				int a=obj->get_angle()-90;
				while(a<0) a+=360;
				while(a>=360) a-=360;
				m_speed_x+=float(cos_degree(a)*54.0)/256.0f;
				m_speed_y+=float(sin_degree(a)*54.0)/256.0f;
			} else {
				Sound_play(SFXM->get(TGL_SFX_FOLDER "/explosion"),sfx_volume);
				map->add_auxiliary_front_object(new TGLobject_FX_explosion2(get_x(),get_y(),256,200));
				
                for(TGLobject_ship_chain *link:m_chain) {
                    map->remove_auxiliary_back_object(link);
                    delete link;
                }
				m_chain.clear();
				map->remove_auxiliary_back_object(m_grabber);
				map->remove_auxiliary_front_object(m_grabber);
				delete m_grabber;
				m_grabber=0;		
				return false;
			}
		} else {
			Sound_play(SFXM->get(TGL_SFX_FOLDER "/explosion"),sfx_volume);
			map->add_auxiliary_front_object(new TGLobject_FX_explosion2(get_x(),get_y(),256,200));
			
            for(TGLobject_ship_chain *link:m_chain) {
                map->remove_auxiliary_back_object(link);
                delete link;
            }
            m_chain.clear();
  			map->remove_auxiliary_back_object(m_grabber);
			map->remove_auxiliary_front_object(m_grabber);
			delete m_grabber;
			m_grabber=0;		
			return false;
		}
	} // if 

	m_thrusting=false;

	if (m_fuel_recharging_timmer>0) {
		m_fuel_recharging_timmer--;
		if (m_fuel_recharging_timmer==0) {
			if (m_fuel_channel!=-1) {
				Mix_HaltChannel(m_fuel_channel);
				m_fuel_channel=-1;
			} // if 
		} // if 
	} // if 

	if (k->m_button[0] && !k->m_old_button[0] && m_fuel>32) {
		TGLobject *bullet;
		int a=m_angle-90;
		while(a<0) a+=360;
		while(a>=360) a-=360;
		bullet=new TGLobject_bullet(float(get_x()+(cos_degree(a)*12)),float(get_y()+(sin_degree(a)*12)),0,m_angle,5,1,GLTM->get("graphics/objects/bullet-red.png"),this);
		if (m_ball!=0) bullet->exclude_for_collision(m_ball);
		map->add_object_back(bullet);
		m_fuel-=32;
//		Sound_play(SFXM->get(TGL_SFX_FOLDER "/shipshot"),sfx_volume);
		Sound_play(SFXM->get(TGL_SFX_FOLDER "/shipshot2"),sfx_volume);
	} // if 

	if (k->m_joystick[VC_LEFT] && !k->m_joystick[VC_DOWN]) {
		m_angle-=4;
		if (m_angle<0) m_angle+=360;
	} // if 
	if (k->m_joystick[VC_RIGHT] && !k->m_joystick[VC_DOWN]) {
		m_angle+=4;
		if (m_angle>=360) m_angle-=360;
	} // if

	if (k->m_joystick[VC_UP] && m_fuel>0) {
		int a=m_angle-90;
		while(a<0) a+=360;
		while(a>=360) a-=360;
		m_speed_x+=float(cos_degree(a)*18.0)/256.0f;
		m_speed_y+=float(sin_degree(a)*18.0)/256.0f;
		m_thrusting=true;
		m_fuel--;
	} // if 

	if (k->m_joystick[VC_DOWN] && k->m_joystick[VC_LEFT] && get_chain_length()>MIN_CHAIN_SIZE*CHAIN_LINK_LENGTH && m_fuel>0) {
		m_chain_length--;
		if (m_chain_length<m_chain.size()*(CHAIN_LINK_LENGTH*1.5f)) {
			// Remove the first link:
			TGLobject *o=m_chain.front();
            m_chain.pop_front();
			map->remove_auxiliary_back_object(o);
			delete o;
			m_chain.front()->set_previous(this);
		} // if 
	} // if 

	if (k->m_joystick[VC_DOWN] && k->m_joystick[VC_RIGHT] && get_chain_length()<MAX_CHAIN_SIZE*CHAIN_LINK_LENGTH && m_fuel>0) {
		m_chain_length++;
		if (m_chain_length>m_chain.size()*(CHAIN_LINK_LENGTH*1.5f)) {
			// Add another link:
			TGLobject_ship_chain *link;


			link=new TGLobject_ship_chain(m_x,m_y);
			link->set_previous_join(m_x,m_y);
			link->set_next_join(m_x,m_y);

			link->set_previous(this);
			link->set_next(m_chain.front());
			m_chain.front()->set_previous(link);
			m_chain.push_front(link);
	
			map->add_auxiliary_back_object(link);

		} // if 
	} // if 

	if (k->m_button[1] && !k->m_old_button[1]) {
		if (m_ball==0) {
			m_ball=m_grabber->grab_ball(map);

			if (m_ball!=0) {
				m_ball->capture(2);
				Sound_play(SFXM->get(TGL_SFX_FOLDER "/takeball"),sfx_volume);
			} // if 

		} else {
			m_grabber->release_ball();
			m_ball=0;
		} // if 
	} // if 


	if (m_speed_x>0) {
		if (m_speed_x>1.0f/256.0f) m_speed_x-=1.0f/256.0f;
							  else m_speed_x=0;
	} // if 
	if (m_speed_x<0) {
		if (m_speed_x<-1.0f/256.0f) m_speed_x+=1.0f/256.0f;
							   else m_speed_x=0;
	} // if 
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

	chain_cycle();

	if (m_thrusting) {
		int a=m_angle;
		while(a<0) a+=360;
		while(a>=360) a-=360;

		if ((m_cycle%4)==0) map->add_auxiliary_back_object(new TGLobject_FX_particle(float(get_x()+(cos_degree(a)*11)),float(get_y()+(sin_degree(a)*11)),rand()%60,0,0,1,false,0.125f,0,0.375f,1.0f,50,GLTM->get("graphics/objects/smoke.png")));
		if ((m_cycle%4)==0) map->add_auxiliary_back_object(new TGLobject_FX_particle(float(get_x()-(cos_degree(a)*11)),float(get_y()-(sin_degree(a)*11)),rand()%60,0,0,1,false,0.125f,0,0.375f,1.0f,50,GLTM->get("graphics/objects/smoke.png")));

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


void TGLobject_ship_c_harpoon::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_last_mask==0) m_last_mask=GLTM->get("graphics/objects/ship-harpoon-1.png");

	if (m_thrusting) {
		if (((m_cycle/4)%2)==0) m_last_tile=GLTM->get("graphics/objects/ship-harpoon-2.png");
						   else m_last_tile=GLTM->get("graphics/objects/ship-harpoon-3.png");
	} else {
		m_last_tile=GLTM->get("graphics/objects/ship-harpoon-1.png");
	} // if 

    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_ship_c_harpoon::draw */


bool TGLobject_ship_c_harpoon::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_ship_c_harpoon")) return true;
	return TGLobject_ship::is_a(c);
} /* TGLobject_ship_c_harpoon::is_a */ 


bool TGLobject_ship_c_harpoon::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_ship_c_harpoon::is_a */ 


const char *TGLobject_ship_c_harpoon::get_class(void)
{
	return "TGLobject_ship_c_harpoon";
} /* TGLobject_ship_c_harpoon:get_class */ 


