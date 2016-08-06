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




TGLobject_ship_chain::TGLobject_ship_chain(float x,float y) : TGLobject(x,y,0)
{
	m_scale=1.0f;

	m_next_join_x=m_x;
	m_next_join_y=m_y;
	m_previous_join_x=m_x;
	m_previous_join_y=m_y+CHAIN_LINK_LENGTH;

} /* TGLobject_ship_chain::TGLobject_ship */ 


TGLobject_ship_chain::~TGLobject_ship_chain()
{
} /* TGLobject_ship_chain::~TGLobject_ship_chain */ 


void TGLobject_ship_chain::set_previous(TGLobject *o)
{
	m_previous=o;
} /* TGLobject_ship_chain::set_previous */ 


void TGLobject_ship_chain::set_next(TGLobject *o)
{
	m_next=o;
} /* TGLobject_ship_chain::set_next */ 


void TGLobject_ship_chain::set_previous_join(float x,float y)
{
	m_previous_join_x=x;
	m_previous_join_y=y;
} /* TGLobject_ship_chain::set_previous_join */ 


void TGLobject_ship_chain::set_next_join(float x,float y)
{
	m_next_join_x=x;
	m_next_join_y=y;
} /* TGLobject_ship_chain::set_next_join */ 


void TGLobject_ship_chain::chain_cycle(void)
{
	float desired_next_join_x=m_x,desired_next_join_y=m_y+6;	

	if (m_previous_join_x==m_next_join_x &&
		m_previous_join_y==m_next_join_y) {
		m_x=m_x*(1-CHAIN_TENSION)+m_previous_join_x*CHAIN_TENSION;
		m_y=m_y*(1-CHAIN_TENSION)+m_previous_join_y*CHAIN_TENSION;
		desired_next_join_x=m_x+float((CHAIN_LINK_LENGTH/2)*cos_degree(90-m_angle));
		desired_next_join_y=m_x+float((CHAIN_LINK_LENGTH/2)*sin_degree(90-m_angle));
	} else {
		float center_x,center_y;
		int desired_angle;
		float desired_x,desired_y;

		center_x=(m_previous_join_x+m_next_join_x)/2;
		center_y=(m_previous_join_y+m_next_join_y)/2;

		desired_angle=int((180*atan2(m_next_join_y-m_previous_join_y,m_next_join_x-m_previous_join_x))/M_PI);
		while(desired_angle<0) desired_angle+=360;
		while(desired_angle>=360) desired_angle-=360;
		desired_x=center_x+float((-CHAIN_LINK_LENGTH/2)*cos_degree(desired_angle));
		desired_y=center_y+float((-CHAIN_LINK_LENGTH/2)*sin_degree(desired_angle));
		desired_next_join_x=center_x+float((CHAIN_LINK_LENGTH/2)*cos_degree(desired_angle));
		desired_next_join_y=center_y+float((CHAIN_LINK_LENGTH/2)*sin_degree(desired_angle));
//		desired_angle=90-desired_angle;
		desired_angle=desired_angle-90;

		m_x=m_x*(1-CHAIN_TENSION)+desired_x*CHAIN_TENSION;
		m_y=m_y*(1-CHAIN_TENSION)+desired_y*CHAIN_TENSION;
		m_angle=desired_angle;
	} // if 

	// update previous and next desired join posotions:
	if (m_previous->is_a("TGLobject_ship_chain")) {
		// Update
		((TGLobject_ship_chain *)m_previous)->set_next_join(m_x,m_y);
	} else {
		// The previous is the ship
		((TGLobject_ship_c_harpoon *)m_previous)->set_next_join(m_x,m_y);
	} // if 

	if (m_next->is_a("TGLobject_ship_chain")) {
		// Update
		((TGLobject_ship_chain *)m_next)->set_previous_join(desired_next_join_x,desired_next_join_y);
	} else {
		// The next is the grabber
		// Update
		((TGLobject_ship_grabber *)m_next)->set_previous_join(desired_next_join_x,desired_next_join_y);
	} // if 
} /* TGLobject_ship_chain::chain_cycle */ 


bool TGLobject_ship_chain::cycle(VirtualController *k,TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;

	chain_cycle();
	return true;
} /* TGLobject::cycle */ 


void TGLobject_ship_chain::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_last_mask==0) m_last_mask=GLTM->get("graphics/objects/ship-harpoon-4.png");
	if (m_last_tile==0) m_last_tile=GLTM->get("graphics/objects/ship-harpoon-4.png");

    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_ship_chain::draw */


bool TGLobject_ship_chain::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_ship_chain")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_ship_chain::is_a */ 


bool TGLobject_ship_chain::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_ship_chain::is_a */ 


const char *TGLobject_ship_chain::get_class(void)
{
	return "TGLobject_ship_chain";
} /* TGLobject_ship_chain:get_class */ 


