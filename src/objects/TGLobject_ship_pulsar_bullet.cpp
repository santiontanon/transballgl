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
#include "SDL_glutaux.h"
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
#include "TGLobject_ship_pulsar_bullet.h"
#include "TGLobject_FX_particle.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_ship_pulsar_bullet::TGLobject_ship_pulsar_bullet(float x,float y, float angle, TGLobject *ship) : TGLobject(x,y,0)
{
	m_angle=angle;
	m_radius=8;
	m_speed = 1.5;
	timmer = 250;
	exclude_for_collision(ship);
	
	n_points = 128;
	m_point_x = new double[n_points];
	m_point_y = new double[n_points];
	m_point_alpha = new double[n_points];
	m_point_width = new double[n_points];
	for(int i = 0;i<n_points;i++) m_point_alpha[i] = 1.0;
	
	SDL_Surface *pixel_sfc = SDL_CreateRGBSurface(SDL_SWSURFACE,1,1,32,RMASK,GMASK,BMASK,AMASK);
	putpixel(pixel_sfc,0,0,SDL_MapRGBA(pixel_sfc->format, 255,255,255,255));
	pixel_tile = new GLTile(pixel_sfc);
	pixel_object = new TGLobject(pixel_tile,0,0,0);
	pixel_object->exclude_for_collision(ship);

} /* TGLobject_ship_shield::TGLobject_ship_shield */ 


TGLobject_ship_pulsar_bullet::~TGLobject_ship_pulsar_bullet()
{
	delete pixel_tile;
	delete pixel_object;
	delete []m_point_x;
	delete []m_point_y;
	delete []m_point_alpha;
	delete []m_point_width;
} /* TGLobject_ship_pulsar_bullet::~TGLobject_ship_pulsar_bullet */


bool TGLobject_ship_pulsar_bullet::cycle(VirtualController *k,TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	m_cycle++;
	timmer--;
	m_radius+=0.3;
	
	// Move:
	if (m_state==0) {
		int a=m_angle-90;
		while(a<0) a+=360;
		while(a>=360) a-=360;
		m_x+=float((cos_degree(a)*m_speed));
		m_y+=float((sin_degree(a)*m_speed));
	} // if	
	
	// after a while, the ripple desintegrates:
	if (timmer==0) {
		// one point at random (use the "x" position as the random number generator to avoid calls to "rand()"):
		m_point_alpha[int(m_x)%n_points] *= 0.9;
	}
	
	// propagate disintegration:
	double max_alpha = 0;
	for(int i = 0;i<2;i++) {
		double previous_alpha = m_point_alpha[n_points-1], tmp;
		for(int i = 0;i<n_points;i++) {
			tmp = m_point_alpha[i];
			if (m_point_alpha[i]>max_alpha) max_alpha = m_point_alpha[i];
			if (m_point_alpha[i]<0.99 ||
				m_point_alpha[(i+1)%n_points]<0.99 ||
				previous_alpha<0.99) {
				m_point_alpha[i]*=0.9;
			}
			previous_alpha = tmp;
		}
	}
		
	// collisions for the parts of the wave still alive:
	for(int i = 0;i<n_points;i++) {
		if (m_point_alpha[i]>0.99) {
			if (map->collision(pixel_object,m_x+m_point_x[i],m_y+m_point_y[i],0)) {				
				TGLobject *o=map->collision_with_object(m_x+m_point_x[i],m_y+m_point_y[i]);
				if (o!=0) {
					if (this->check_collision(o)) {
						m_point_alpha[i]*=0.9;
						if (o->is_a("TGLobject_enemy")) {
							((TGLobject_enemy *)o)->hit(1);
							exclude_for_collision(o);	// only hit each enemy once per wave
						} else if (o->is_a("TGLobject_bullet") && this->check_collision(o)) {
							((TGLobject_bullet *)o)->hit();
						}
					}
				} else {
					m_point_alpha[i]*=0.9;
				} // if 	
			} // if 
		}
	}
	
	if (max_alpha<0.05) return false;
	
//	if (get_x()<0 || get_y()<0 || get_x()>map->get_dx() || get_y()>map->get_dy()) return false;
	
	return true;
} /* TGLobject_ship_pulsar_bullet::cycle */ 



GLuint pulsar_bullet_VertexArrayID = 0;
GLuint pulsar_bullet_Vertexbuffer = 0;
GLfloat *pulsar_bullet_buffer_data = 0;


void TGLobject_ship_pulsar_bullet::draw(glm::mat4 transform, class GLTManager *GLTM)
{
    glm::mat4 tmp = glm::translate(transform, glm::vec3(m_x,m_y,0));

	int angle=m_angle;
	while(angle<0) angle+=360;
	while(angle>=360) angle-=360;
	
	double d = 0, r = 0, perturb = 0;
	double a = 0;
	double x,y;
	for(int i = 0;i<n_points;i++,a+=(M_PI*2/n_points)) {
		d = a * m_radius;
		perturb = m_radius*0.1;
		if (perturb>8) perturb = 10;
		r = m_radius + sin(d*0.075)*perturb;
		x = r*cos(a);
		y = r*sin(a)*0.25;
		m_point_width[i] = 8;
		
		m_point_x[i] = cos_degree(angle)*x - sin_degree(angle)*y;
		m_point_y[i] = sin_degree(angle)*x + cos_degree(angle)*y;
	}
    
    if (pulsar_bullet_buffer_data==0) {
        pulsar_bullet_buffer_data = new GLfloat[n_points*2*5];
        glGenVertexArrays(1, &pulsar_bullet_VertexArrayID);
        glBindVertexArray(pulsar_bullet_VertexArrayID);
        glGenBuffers(1, &pulsar_bullet_Vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, pulsar_bullet_Vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*2*n_points, pulsar_bullet_buffer_data, GL_DYNAMIC_DRAW);
    }
    int loc1 = glGetAttribLocation(programID, "vPosition");
    int loc2 = glGetAttribLocation(programID, "UV");
    
    float wx,wy,vx,vy,vn;
    for(int i = 0;i<n_points;i++) {
        wx = m_point_x[(i+1)%n_points] - m_point_x[i];
        wy = m_point_y[(i+1)%n_points] - m_point_y[i];
        vx = wy;
        vy = -wx;
        vn = sqrt(vx*vx + vy*vy);
        vx = vx/vn;
        vy = vy/vn;
        pulsar_bullet_buffer_data[i*2*5] = m_point_x[i];
        pulsar_bullet_buffer_data[i*2*5+1] = m_point_y[i];
        pulsar_bullet_buffer_data[i*2*5+2] = 0;
        pulsar_bullet_buffer_data[i*2*5+3] = 1;
        pulsar_bullet_buffer_data[i*2*5+4] = 0.5*m_point_alpha[i];
        
        pulsar_bullet_buffer_data[i*2*5+5] = m_point_x[i]+vx*m_point_width[i];
        pulsar_bullet_buffer_data[i*2*5+6] = m_point_y[i]+vy*m_point_width[i];
        pulsar_bullet_buffer_data[i*2*5+7] = 0;
        pulsar_bullet_buffer_data[i*2*5+8] = 1;
        pulsar_bullet_buffer_data[i*2*5+9] = 0;
    }
    glBindVertexArray(pulsar_bullet_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, pulsar_bullet_Vertexbuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5*2*n_points, pulsar_bullet_buffer_data);
    
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    glUniform4f(inColorID, 0, 0, 1, 1);
    glUniform1i(useTextureID, 2);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, n_points*2);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    for(int i = 0;i<n_points;i++) {
        wx = m_point_x[(i+1)%n_points] - m_point_x[i];
        wy = m_point_y[(i+1)%n_points] - m_point_y[i];
        vx = wy;
        vy = -wx;
        vn = sqrt(vx*vx + vy*vy);
        vx = vx/vn;
        vy = vy/vn;
        pulsar_bullet_buffer_data[i*2*5] = m_point_x[i];
        pulsar_bullet_buffer_data[i*2*5+1] = m_point_y[i];
        pulsar_bullet_buffer_data[i*2*5+2] = 0;
        pulsar_bullet_buffer_data[i*2*5+3] = 1;
        pulsar_bullet_buffer_data[i*2*5+4] = 0.5*m_point_alpha[i];
        
        pulsar_bullet_buffer_data[i*2*5+5] = m_point_x[i]-vx*m_point_width[i];
        pulsar_bullet_buffer_data[i*2*5+6] = m_point_y[i]-vy*m_point_width[i];
        pulsar_bullet_buffer_data[i*2*5+7] = 0;
        pulsar_bullet_buffer_data[i*2*5+8] = 1;
        pulsar_bullet_buffer_data[i*2*5+9] = 0;
    }
    glBindVertexArray(pulsar_bullet_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, pulsar_bullet_Vertexbuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5*2*n_points, pulsar_bullet_buffer_data);
    
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    glUniform4f(inColorID, 0, 0, 1, 1);
    glUniform1i(useTextureID, 2);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, n_points*2);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    for(int i = 0;i<n_points;i++) {
        pulsar_bullet_buffer_data[i*2*5] = m_point_x[i];
        pulsar_bullet_buffer_data[i*2*5+1] = m_point_y[i];
        pulsar_bullet_buffer_data[i*2*5+2] = 0;
        pulsar_bullet_buffer_data[i*2*5+3] = 1;
        pulsar_bullet_buffer_data[i*2*5+4] = m_point_alpha[i];
        pulsar_bullet_buffer_data[i*2*5+5] = m_point_x[(i+1)%n_points];
        pulsar_bullet_buffer_data[i*2*5+6] = m_point_y[(i+1)%n_points];
        pulsar_bullet_buffer_data[i*2*5+7] = 0;
        pulsar_bullet_buffer_data[i*2*5+8] = 1;
        pulsar_bullet_buffer_data[i*2*5+9] = m_point_alpha[(i+1)%n_points];
    }
    glBindVertexArray(pulsar_bullet_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, pulsar_bullet_Vertexbuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5*2*n_points, pulsar_bullet_buffer_data);
    
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    glUniform4f(inColorID, 1, 1, 1, 1);
    glUniform1i(useTextureID, 2);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_LINES, 0, n_points*2);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glUniform4f(inColorID, 1, 1, 1, 1);
    
} /* TGLobject_ship_pulsar_bullet::draw */


bool TGLobject_ship_pulsar_bullet::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_ship_pulsar_bullet")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_ship_pulsar_bullet::is_a */ 


bool TGLobject_ship_pulsar_bullet::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_ship_pulsar_bullet::is_a */ 


const char *TGLobject_ship_pulsar_bullet::get_class(void)
{
	return "TGLobject_ship_pulsar_bullet";
} /* TGLobject_ship_pulsar_bullet:get_class */ 

