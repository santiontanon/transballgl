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




TGLobject_ship_grabber::TGLobject_ship_grabber(float x,float y) : TGLobject(x,y,0)
{
	m_scale=1.0f;

	m_chain=0;
	m_ball=0;
	m_ship=0;

	m_speed_x=0;
	m_speed_y=0;
	m_speed_a=0;
	m_precission_angle=float(m_angle);

	m_previous_join_x=x;
	m_previous_join_y=y;

} /* TGLobject_ship_grabber::TGLobject_ship */ 


TGLobject_ship_grabber::~TGLobject_ship_grabber()
{
} /* TGLobject_ship_grabber::~TGLobject_ship_grabber */ 


void TGLobject_ship_grabber::set_chain(TGLobject_ship_chain *chain)
{
	m_chain=chain;
} /* TGLobject_ship_grabber::set_chain */ 


void TGLobject_ship_grabber::set_ball(TGLobject_ball *ball)
{
	m_ball=ball;
} /* TGLobject_ship_grabber::set_ball */ 


void TGLobject_ship_grabber::set_ship(TGLobject_ship_c_harpoon *ship)
{
	m_ship=ship;
} /* TGLobject_ship_grabber::set_ball */ 


void TGLobject_ship_grabber::set_previous_join(float x,float y)
{
	m_previous_join_x=x;
	m_previous_join_y=y;
} /* TGLobject_ship_grabber::set_previous_join */ 


TGLobject_ball *TGLobject_ship_grabber::grab_ball(TGLmap *map)
{
	if (m_ball==0) {
		TGLobject_ball *ball=(TGLobject_ball *)map->object_exists("TGLobject_ball",get_x()-16,get_y(),get_x()+16,get_y()+64);

		if (ball!=0 && collision(ball) && 
			(fabs(m_speed_x)+fabs(m_speed_y))<1 &&
			fabs(m_speed_a)<1) {
			if (ball->get_state()==0) map->action(0);
			m_ball=ball;
			map->remove_auxiliary_back_object(this);
			map->add_auxiliary_front_object(this);			
		} // if 
	} // if

	return m_ball;
} /* TGLobject_ship_grabber::grab_ball */ 


bool TGLobject_ship_grabber::release_ball(void)
{
	if (m_ball!=0) {
		m_ball->capture(-1);
		m_ball=0;
		return true;
	} // if 

	return false;
} /* TGLobject_ship_grabber::release_ball */ 


void TGLobject_ship_grabber::chain_cycle(void)
{
//	m_x=(m_x*(1-CHAIN_TENSION)+m_previous_join_x*CHAIN_TENSION);
//	m_y=(m_y*(1-CHAIN_TENSION)+m_previous_join_y*CHAIN_TENSION);

	if (m_chain!=0) {
		m_chain->set_next_join(m_x,m_y);
	} // if 

} /* TGLobject_ship_grabber::chain_cycle */ 


void TGLobject_ship_grabber::chain_cycle_ball(void)
{
	float desired_next_join_x=m_x,desired_next_join_y=m_y+10;
	float next_join_x=m_ball->get_x();
	float next_join_y=m_ball->get_y();

	if (m_previous_join_x==next_join_x &&
		m_previous_join_y==next_join_y) {
		m_x=m_x*(1-CHAIN_TENSION)+m_previous_join_x*CHAIN_TENSION;
		m_y=m_y*(1-CHAIN_TENSION)+m_previous_join_y*CHAIN_TENSION;
		desired_next_join_x=m_x+float((7)*cos_degree(90-m_angle));
		desired_next_join_y=m_x+float((7)*sin_degree(90-m_angle));
	} else {
		float center_x,center_y;
		int desired_angle;
		float desired_x,desired_y;

		center_x=(m_previous_join_x+next_join_x)/2;
		center_y=(m_previous_join_y+next_join_y)/2;

		desired_angle=int((180*atan2(next_join_y-m_previous_join_y,next_join_x-m_previous_join_x))/M_PI);
		while(desired_angle<0) desired_angle+=360;
		while(desired_angle>=360) desired_angle-=360;
		desired_x=center_x+float((-5)*cos_degree(desired_angle));
		desired_y=center_y+float((-5)*sin_degree(desired_angle));
		desired_next_join_x=center_x+float((5)*cos_degree(desired_angle));
		desired_next_join_y=center_y+float((5)*sin_degree(desired_angle));
//		desired_angle=90-desired_angle;
		desired_angle=desired_angle-90;

		m_x=m_x*(1-CHAIN_TENSION)+desired_x*CHAIN_TENSION;
		m_y=m_y*(1-CHAIN_TENSION)+desired_y*CHAIN_TENSION;
		m_angle=desired_angle;
		m_precission_angle=float(m_angle);
	} // if 

	m_chain->set_next_join(m_x,m_y);
} /* TGLobject_ship_grabber::chain_cycle_ball */ 



bool TGLobject_ship_grabber::cycle(VirtualController *k,TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;

	// Interaction with the ship and ball:
	if (m_ship!=0) {
		if (m_ball==0) {
			float dx,dy,d;
			dx=m_x-m_ship->get_x();
			dy=m_y-m_ship->get_y();
			d=float(sqrt(dx*dx+dy*dy));
			float v1=dx/d,v2=dy/d;

			if (d>m_ship->get_chain_length()) {
				float f1,f2;

				f1=1/(1+GRABBER_WEIGHT_WO_BALL);
				f2=GRABBER_WEIGHT_WO_BALL/(1+GRABBER_WEIGHT_WO_BALL);
				
				// Interaction of strengths:
				{
					float ship_f1;
					float ship_fx1,ship_fy1,ship_fx2,ship_fy2;
					float grabber_f1;
					float grabber_fx1,grabber_fy1,grabber_fx2,grabber_fy2;

					float tension=0;

					ship_f1=m_ship->get_speedx()*v1+m_ship->get_speedy()*v2;
					ship_fx1=v1*ship_f1;
					ship_fy1=v2*ship_f1;
					ship_fx2=m_ship->get_speedx()-ship_fx1;
					ship_fy2=m_ship->get_speedy()-ship_fy1;
				
					grabber_f1=m_speed_x*v1+m_speed_y*v2;
					grabber_fx1=v1*grabber_f1;
					grabber_fy1=v2*grabber_f1;
					grabber_fx2=m_speed_x-grabber_fx1;
					grabber_fy2=m_speed_y-grabber_fy1;

					if (grabber_f1>ship_f1) {
						m_ship->set_speedx(f1*ship_fx1+f2*grabber_fx1 + ship_fx2);
						m_ship->set_speedy(f1*ship_fy1+f2*grabber_fy1 + ship_fy2);
					} else {
						// nothing to do
					} // if 

					if (ship_f1<grabber_f1) {
						m_speed_x=f1*ship_fx1+f2*grabber_fx1 + grabber_fx2;
						m_speed_y=f1*ship_fy1+f2*grabber_fy1 + grabber_fy2;
					} else {
						// nothing to do
					} // if 

					tension=grabber_f1-ship_f1;
					if (tension<0) tension=0;

					if (tension>0) {
						float angle_difference;
						float desired_angle=float((180*atan2(v2,v1))/M_PI);
						while(desired_angle<0) desired_angle+=360;
						while(desired_angle>=360) desired_angle-=360;
						desired_angle=desired_angle-90;

						while(m_precission_angle<0) m_precission_angle+=360;
						while(m_precission_angle>=360) m_precission_angle-=360;

						angle_difference=desired_angle-m_precission_angle;
						while(angle_difference<-180) angle_difference+=360;
						while(angle_difference>=180) angle_difference-=360;

						m_speed_a+=angle_difference*tension*0.05f;
					}
				}

				// Fix the coordinates: (never modify the ship coordinates, to ease control)
				{
					m_x=m_ship->get_x()+(dx*m_ship->get_chain_length())/d;
					m_y=m_ship->get_y()+(dy*m_ship->get_chain_length())/d;
				}
			} // if 
		} else {
			float dx,dy,d;
			dx=m_ball->get_x()-m_ship->get_x();
			dy=m_ball->get_y()-m_ship->get_y();
			d=float(sqrt(dx*dx+dy*dy));
			float v1=dx/d,v2=dy/d;

			if (d>m_ship->get_chain_length()+14) {
				float f1,f2;

				f1=1/(1+GRABBER_WEIGHT_W_BALL);
				f2=GRABBER_WEIGHT_W_BALL/(1+GRABBER_WEIGHT_W_BALL);

				// Interaction of strengths:
				{
					float ship_f1;
					float ship_fx1,ship_fy1,ship_fx2,ship_fy2;
					float ball_f1;
					float ball_fx1,ball_fy1,ball_fx2,ball_fy2;

					ship_f1=m_ship->get_speedx()*v1+m_ship->get_speedy()*v2;
					ship_fx1=v1*ship_f1;
					ship_fy1=v2*ship_f1;
					ship_fx2=m_ship->get_speedx()-ship_fx1;
					ship_fy2=m_ship->get_speedy()-ship_fy1;
				
					ball_f1=m_ball->get_speed_x()*v1+m_ball->get_speed_y()*v2;
					ball_fx1=v1*ball_f1;
					ball_fy1=v2*ball_f1;
					ball_fx2=m_ball->get_speed_x()-ball_fx1;
					ball_fy2=m_ball->get_speed_y()-ball_fy1;

					if (ball_f1>ship_f1) {
						m_ship->set_speedx(f1*ship_fx1+f2*ball_fx1 + ship_fx2);
						m_ship->set_speedy(f1*ship_fy1+f2*ball_fy1 + ship_fy2);
					} else {
						// nothing to do
					} // if 

					if (ship_f1<ball_f1) {
						m_ball->set_speed_x(f1*ship_fx1+f2*ball_fx1 + ball_fx2);
						m_ball->set_speed_y(f1*ship_fy1+f2*ball_fy1 + ball_fy2);
					} else {
						// nothing to do
					} // if 

				}

				// Fix the coordinates: (never modify the ship coordinates, to ease control)
				{
					m_ball->set_x(m_ship->get_x()+(dx*(m_ship->get_chain_length()+14))/d);
					m_ball->set_y(m_ship->get_y()+(dy*(m_ship->get_chain_length()+14))/d);
				}

			} // if 

			// Compute position of grabber:
			chain_cycle_ball();
		} // if 
	}


	if (m_ball==0) {
		// When the ball is not taken, compute grabber's own phisics:

		// Gravity:
		if (m_speed_x>0) {
			if (m_speed_x>1.0f/256.0f) m_speed_x-=1.0f/256.0f;
								  else m_speed_x=0;
		} // if 
		if (m_speed_x<0) {
			if (m_speed_x<-1.0f/256.0f) m_speed_x+=1.0f/256.0f;
								   else m_speed_x=0;
		} // if 
		if (m_speed_a>0) {
			if (m_speed_a>16.0f/256.0f) m_speed_a-=16.0f/256.0f;
								  else m_speed_a=0;
		} // if 
		if (m_speed_a<0) {
			if (m_speed_a<-16.0f/256.0f) m_speed_a+=16.0f/256.0f;
								   else m_speed_a=0;
		} // if 
		m_speed_y+=4.0f/256.0f;

		if (m_speed_x>8) m_speed_x=8;
		if (m_speed_x<-8) m_speed_x=-8;
		if (m_speed_y>8) m_speed_y=8;
		if (m_speed_y<-8) m_speed_y=-8;
		if (m_speed_a<-8) m_speed_a=-8;
		if (m_speed_a>8) m_speed_a=8;
		m_x+=m_speed_x;
		m_y+=m_speed_y;
		m_precission_angle+=m_speed_a;
		m_angle=int(m_precission_angle);

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

	} // if 

	chain_cycle();

	return true;
} /* TGLobject::cycle */ 


void TGLobject_ship_grabber::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_last_mask==0) m_last_mask=GLTM->get("graphics/objects/ship-harpoon-5.png");
	if (m_last_tile==0) m_last_tile=GLTM->get("graphics/objects/ship-harpoon-5.png");

    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_ship_grabber::draw */


bool TGLobject_ship_grabber::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_ship_grabber")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_ship_grabber::is_a */ 


bool TGLobject_ship_grabber::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_ship_grabber::is_a */ 


const char *TGLobject_ship_grabber::get_class(void)
{
	return "TGLobject_ship_grabber";
} /* TGLobject_ship_grabber:get_class */ 


