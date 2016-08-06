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
#include "Vector.h"

#include "GLTManager.h"
#include "SFXManager.h"
#include "TGLobject.h"
#include "TGLobject_bullet.h"
#include "TGLobject_bullet_hmissile.h"
#include "TGLobject_enemy.h"
#include "TGLobject_FX_particle.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_bullet_hmissile::TGLobject_bullet_hmissile(float x,float y,int ao,int angle,float speed,int power,GLTile *tile,GLTile *tile2,TGLobject *ship) : 
						  TGLobject_bullet(x,y,ao,angle,speed,power,tile,ship)
{
	m_tile2=tile2;
	m_target=0;
} /* TGLobject_bullet_hmissile::TGLobject_bullet_hmissile */ 


bool TGLobject_bullet_hmissile::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	if ((m_cycle%4)==0) map->add_auxiliary_back_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%60,0,0,1,false,0.125f,0,0.25f,0.75f,50,GLTM->get("graphics/objects/smoke.png")));

	{
		// Look for a new target:
		float d,min_d=0;
		m_target=0;
		std::list<TGLobject *> *l=map->get_objects("TGLobject_enemy");
        for(TGLobject *o:*l) {
			d=float(sqrt((o->get_x()-get_x())*(o->get_x()-get_x())+(o->get_y()-get_y())*(o->get_y()-get_y())));
			if (m_target==0 || d<min_d) {
				m_target=o;
				min_d=d;
			} // if 
		} // while 
		
		l->clear();
		delete l;
	} // if 

	if (m_target!=0) {
		int i;
		bool first=true;
		int increment=0;
		float min_distance=0,d;
		GLTile *t;

		m_dest_x=m_target->get_x();
		m_dest_y=m_target->get_y();
		t=m_target->get_last_mask();
		if (t!=0) {
			float min_x,min_y,max_x,max_y;
			
			min_x=m_dest_x-t->get_hot_x();
			min_y=m_dest_y-t->get_hot_y();
			max_x=(m_dest_x-t->get_hot_x())+t->get_dx();
			max_y=(m_dest_y-t->get_hot_y())+t->get_dy();

			m_dest_x=(min_x+max_x)/2;
			m_dest_y=(min_y+max_y)/2;
		} // if 

		for(i=-2;i<3;i++) {
			int a=(m_angle+i)-90;
			while(a<0) a+=360;
			while(a>=360) a-=360;
			float next_x=float(m_x+(cos_degree(a)*m_speed));
			float next_y=float(m_y+(sin_degree(a)*m_speed));

			d=float(sqrt((next_x-m_dest_x)*(next_x-m_dest_x)+(next_y-m_dest_y)*(next_y-m_dest_y)));

			if (first || d<min_distance) {
				increment=i;
				min_distance=d;
			} // if 
			first=false;
		} // for

		m_angle+=increment;

	} // if 


	return TGLobject_bullet::cycle(k,map,GLTM,SFXM,sfx_volume);
} /* TGLobject_bullet_hmissile::cycle */ 


void TGLobject_bullet_hmissile::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_target!=0) {
		GLTile *dot=GLTM->get("graphics/objects/dot.png");
		float x=get_x(),y=get_y();
		float dx,dy;
//		bool finished=false;
		float distance;
		do{
            glm::mat4 transform2 = glm::translate(transform, glm::vec3(x, y, 0));
            transform2 = glm::scale(transform2, glm::vec3(0.5*m_scale, 0.5*m_scale, 0.5*m_scale));
            dot->draw(transform2);
			dx=m_dest_x-x;
			dy=m_dest_y-y;
			distance=float(sqrt(dx*dx+dy*dy));
			x+=((dx*16)/distance);
			y+=((dy*16)/distance);
		}while(distance>24);
	} // if 

	if (((m_cycle/4)%2)==0) m_last_tile=m_tile;
					   else m_last_tile=m_tile2;
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }

} /* TGLobject_bullet_hmissile::draw */


bool TGLobject_bullet_hmissile::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_bullet_hmissile")) return true;
	return TGLobject_bullet::is_a(c);
} /* TGLobject_bullet_hmissile::is_a */ 


bool TGLobject_bullet_hmissile::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_bullet_hmissile::is_a */ 


const char *TGLobject_bullet_hmissile::get_class(void)
{
	return "TGLobject_bullet_hmissile";
} /* TGLobject_bullet_hmissile::get_class */ 

