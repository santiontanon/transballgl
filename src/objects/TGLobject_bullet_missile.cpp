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
#include "TGLobject_bullet.h"
#include "TGLobject_bullet_missile.h"
#include "TGLobject_enemy.h"
#include "TGLobject_FX_particle.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_bullet_missile::TGLobject_bullet_missile(float x,float y,int ao,int angle,float speed,int power,GLTile *tile,GLTile *tile2,TGLobject *ship) : 
						  TGLobject_bullet(x,y,ao,angle,speed,power,tile,ship)
{
	m_tile2=tile2;
} /* TGLobject_bullet_missile::TGLobject_bullet_missile */ 


bool TGLobject_bullet_missile::cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	if ((m_cycle%4)==0) map->add_auxiliary_back_object(new TGLobject_FX_particle(get_x(),get_y(),rand()%60,0,0,1,false,0.125f,0,0.25f,0.75f,50,GLTM->get("graphics/objects/smoke.png")));

	return TGLobject_bullet::cycle(k,map,GLTM,SFXM,sfx_volume);
} /* TGLobject_bullet_missile::cycle */ 


void TGLobject_bullet_missile::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (((m_cycle/4)%2)==0) m_last_tile=m_tile;
					   else m_last_tile=m_tile2;
    if (m_last_tile!=0) {
        glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
        if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
        if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
        m_last_tile->draw(transform2);
    }
} /* TGLobject_bullet_missile::draw */


bool TGLobject_bullet_missile::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_bullet_missile")) return true;
	return TGLobject_bullet::is_a(c);
} /* TGLobject_bullet_missile::is_a */ 


bool TGLobject_bullet_missile::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_bullet_missile::is_a */ 


const char *TGLobject_bullet_missile::get_class(void)
{
	return "TGLobject_bullet_missile";
} /* TGLobject_bullet_missile::get_class */ 

