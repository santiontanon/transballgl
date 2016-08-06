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
#include "TGLobject_ship_shield.h"
#include "TGLobject_FX_particle.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_ship_shield::TGLobject_ship_shield(float x,float y) : TGLobject(x,y,0)
{
	m_shield_status=0;
	m_shield_frame=0;
	generate_shield_textures();
} /* TGLobject_ship_shield::TGLobject_ship_shield */ 


TGLobject_ship_shield::~TGLobject_ship_shield()
{
	delete []m_shield_frames;
	
} /* TGLobject_ship_shield::~TGLobject_ship_shield */ 


void TGLobject_ship_shield::set_x(float x)
{
	m_x=x;
} /* TGLobject_ship_shield::set_x */ 


void TGLobject_ship_shield::set_y(float y)
{
	m_y=y;
} /* TGLobject_ship_shield::set_y */ 


void TGLobject_ship_shield::set_status(int status)
{
	m_shield_status=status;
} /* TGLobject_ship_shield::set_status */ 


int TGLobject_ship_shield::get_status(void)
{
	return m_shield_status;
} /* TGLobject_ship_shield::get_status */ 


bool TGLobject_ship_shield::cycle(VirtualController *k,TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;

	m_shield_frame++;
	if (m_shield_frame>=m_n_shield_frames) m_shield_frame-=m_n_shield_frames;

	if (m_shield_status<0) return false;
	return true;
} /* TGLobject_ship_shield::cycle */ 

void TGLobject_ship_shield::draw(glm::mat4 transform, class GLTManager *GLTM)
{
	if (m_shield_status>0) {
		float f=m_shield_status/25.0f;
		m_last_mask=m_last_tile=&(m_shield_frames[m_shield_frame]);

        if (m_last_tile!=0) {
            glm::mat4 transform2 = glm::translate(transform, glm::vec3(m_x, m_y, 0));
            if (m_angle!=0) transform2 = glm::rotate(transform2, float(m_angle*M_PI/180), glm::vec3(0, 0, 1));
            if (m_scale!=1) transform2 = glm::scale(transform2, glm::vec3(m_scale, m_scale, m_scale));
            m_last_tile->draw(1, 1, 1, f, transform2);
        }        
	} else {
		m_last_mask=0;
		m_last_tile=0;
	} // if 

} /* TGLobject_ship_shield::draw */ 


bool TGLobject_ship_shield::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_ship_shield")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_ship_shield::is_a */ 


bool TGLobject_ship_shield::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_ship_shield::is_a */ 


const char *TGLobject_ship_shield::get_class(void)
{
	return "TGLobject_ship_shield";
} /* TGLobject_ship_shield:get_class */ 


#define SHIELD_N_KEYFRAMES 8
#define SHIELD_N_INTERMEDIATE 16
#define SHIELD_N_NODES 12

void TGLobject_ship_shield::generate_shield_textures(void)
{
	int i;

	{
		int j,k;
		float radius[SHIELD_N_NODES]={0,0,0,0,0,0,0,0};
		float initial_radius[SHIELD_N_NODES]={0,0,0,0,0,0,0,0};
		float goal_radius[SHIELD_N_NODES]={0,0,0,0,0,0,0,0};
		float inc_radius[SHIELD_N_NODES]={0,0,0,0,0,0,0,0};

		m_n_shield_frames=SHIELD_N_KEYFRAMES*SHIELD_N_INTERMEDIATE;
		m_shield_frames=new GLTile[SHIELD_N_KEYFRAMES*SHIELD_N_INTERMEDIATE];

		for(k=0;k<SHIELD_N_NODES;k++) radius[k]=initial_radius[k]=float(16+(rand()%12));
		
		for(i=0;i<SHIELD_N_KEYFRAMES;i++) {

			for(k=0;k<SHIELD_N_NODES;k++) {
				if (i==SHIELD_N_KEYFRAMES-1) goal_radius[k]=initial_radius[k];
										else goal_radius[k]=float(16+(rand()%12));
				inc_radius[k]=(goal_radius[k]-radius[k])/SHIELD_N_INTERMEDIATE;
			} // for
			
			for(j=0;j<SHIELD_N_INTERMEDIATE;j++) {

				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE,64,64,32,RMASK,GMASK,BMASK,AMASK);
				SDL_FillRect(sfc,0,0);

				{
					int x,y;
					float angle,desired_radius,actual_radius;
					int r1,r2;
					float cr1,cr2;

					for(x=0;x<64;x++) {
						for(y=0;y<64;y++) {
							angle=float(atan2((double)(y-32),(double)(x-32)));
							if (angle<0) angle+=M_PI*2;
							angle/=(M_PI*2)/SHIELD_N_NODES;
							r1=int(angle);
							cr2=float(angle)-r1;
							cr1=1-cr2;
							if (r1>=SHIELD_N_NODES) r1-=SHIELD_N_NODES;
							r2=r1+1;
							if (r2>=SHIELD_N_NODES) r2-=SHIELD_N_NODES;
							desired_radius=cr1*radius[r1]+cr2*radius[r2];

							actual_radius=float(sqrt((double)((x-32)*(x-32)+(y-32)*(y-32))));

							// Solid shield:
//							if (actual_radius<desired_radius) putpixel(sfc,x,y,SDL_MapRGBA(sfc->format,39,198,95,255));

							// Thin line shield:
/*							
							if (actual_radius<desired_radius && actual_radius>desired_radius-4) {
								putpixel(sfc,x,y,SDL_MapRGBA(sfc->format,39,198,95,255));
							} // if 
*/
							if (actual_radius<desired_radius) {
								if (actual_radius>desired_radius-1) {
									putpixel(sfc,x,y,SDL_MapRGBA(sfc->format,39,198,95,128));
								} else {
									if (actual_radius>desired_radius-5) {
										putpixel(sfc,x,y,SDL_MapRGBA(sfc->format,39,198,95,255));
									} else {
										if (actual_radius>desired_radius-6) {
											putpixel(sfc,x,y,SDL_MapRGBA(sfc->format,39,198,95,128));
										} else {
											putpixel(sfc,x,y,SDL_MapRGBA(sfc->format,39,198,95,64));
										} // if 
									} // if 
								} // if 

							} // if 
						} // for
					} // for
				}

				for(k=0;k<SHIELD_N_NODES;k++) radius[k]+=inc_radius[k];

				m_shield_frames[i*SHIELD_N_INTERMEDIATE+j].set(sfc);
				m_shield_frames[i*SHIELD_N_INTERMEDIATE+j].set_hotspot(32,32);
			} // if 
		} // for 
	}


} /* TGLobject_ship_shield::generate_shield_textures */ 

