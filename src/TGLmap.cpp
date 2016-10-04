#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"
#include <algorithm>

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#include "SDL.h"
#ifdef __EMSCRIPTEN__
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif
#include "SDL_rotozoom.h"


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
#include "TGLmap.h"
#include "TGLobject.h"
#include "TGLobject_ballstand.h"
#include "TGLobject_redlight.h"
#include "TGLobject_radar.h"
#include "TGLobject_pipevscreen.h"
#include "TGLobject_pipehscreen.h"
#include "TGLobject_enemy.h"
#include "TGLobject_laser_left.h"
#include "TGLobject_laser_right.h"
#include "TGLobject_laser_up.h"
#include "TGLobject_laser_down.h"
#include "TGLobject_laser_horizontal.h"
#include "TGLobject_laser_vertical.h"
#include "TGLobject_cannon_left.h"
#include "TGLobject_cannon_right.h"
#include "TGLobject_cannon_down.h"
#include "TGLobject_cannon_up.h"
#include "TGLobject_fastcannon_left.h"
#include "TGLobject_fastcannon_right.h"
#include "TGLobject_fastcannon_down.h"
#include "TGLobject_fastcannon_up.h"
#include "TGLobject_ball.h"
#include "TGLobject_spike_left.h"
#include "TGLobject_spike_right.h"
#include "TGLobject_fuelrecharge.h"
#include "TGLobject_techno_computer.h"
#include "TGLobject_directionalcannon_left.h"
#include "TGLobject_directionalcannon_right.h"
#include "TGLobject_directionalcannon_up.h"
#include "TGLobject_directionalcannon_down.h"
#include "TGLobject_ha_directionalcannon_left.h"
#include "TGLobject_ha_directionalcannon_right.h"
#include "TGLobject_ha_directionalcannon_up.h"
#include "TGLobject_ha_directionalcannon_down.h"
#include "TGLobject_fast_directionalcannon_left.h"
#include "TGLobject_fast_directionalcannon_right.h"
#include "TGLobject_fast_directionalcannon_up.h"
#include "TGLobject_fast_directionalcannon_down.h"
#include "TGLobject_tank.h"
#include "TGLobject_tank_turret.h"
#include "TGLobject_tank_cannon.h"
#include "TGLobject_big_tank.h"
#include "TGLobject_big_tank_cannon.h"
#include "TGLobject_leftdoor.h"
#include "TGLobject_rightdoor.h"
#include "TGLobject_button.h"
#include "TGLobject_ship.h"

#include "collision.h"

#include "debug.h"


#define MAPPAUSE_TIME	25

// extern GLTile *last_collision;


TGLmap::TGLmap(GLTManager *GLTM)
{
	int i;

	m_bg=0;
	m_star_x=0;
	m_star_y=0;
	m_star_color=0;
	m_cycle=0;

	m_fg_dx = 40;
	m_fg_dy = 32;
	m_fg_cell_size=FG_TILE_SIZE;

	m_fg=new GLTile *[m_fg_dx*m_fg_dy];
	for(i=0;i<m_fg_dx*m_fg_dy;i++) m_fg[i]=0;

	set_background(0,GLTM);

    starField_VertexArrayID = 0;
    starField_Vertexbuffer = 0;
    starFieldGlow_VertexArrayID = 0;
    starFieldGlow_Vertexbuffer = 0;
    
    create_starField();
    
} /* TGLmap::TGLmap */


// loads from a memory buffer
TGLmap::TGLmap(char *buffer, GLTManager *GLTM)
{
    char linebuffer[2048];
    int i = 0;
    char *ptr = buffer;
    std::list<char *> lines;

    // break the buffer into lines:
    while(*ptr!=0) {
        char c = *ptr;
        linebuffer[i] = c;
        ptr++;
        i++;
        
        if (c=='\n' || c==0) {
            linebuffer[i-1] = 0;
            // new line:
            char *tmp = new char[i];
            strcpy(tmp,linebuffer);
            lines.push_back(tmp);
            i = 0;
        }
    }
    
    
    load(lines, GLTM);
    
    for(char *tmp:lines) delete []tmp;
    lines.clear();
    
    starField_VertexArrayID = 0;
    starField_Vertexbuffer = 0;
    starFieldGlow_VertexArrayID = 0;
    starFieldGlow_Vertexbuffer = 0;
    
    create_starField();

}


// loads from a file
TGLmap::TGLmap(FILE *fp, GLTManager *GLTM)
{
    char linebuffer[2048];
    int i = 0;
    std::list<char *> lines;
    
    // break the buffer into lines:
    while(true) {
        int c = (feof(fp) ? 0:getc(fp));
        linebuffer[i] = c;
        i++;
        
        if (c=='\n' || c==0) {
            linebuffer[i-1] = 0;
            // new line:
            char *tmp = new char[i];
            strcpy(tmp,linebuffer);
            lines.push_back(tmp);
            i = 0;
        }
        if (c==0) break;
    }
    
    load(lines, GLTM);
    
    for(char *tmp:lines) delete []tmp;
    lines.clear();
    
    starField_VertexArrayID = 0;
    starField_Vertexbuffer = 0;
    starFieldGlow_VertexArrayID = 0;
    starFieldGlow_Vertexbuffer = 0;
    
    create_starField();

}


// loads from a list of strings (each string corresponds to a line in the file)
bool TGLmap::load(std::list<char *> lines, GLTManager *GLTM)
{
    std::list<char *>::iterator next_line = lines.begin();
    int i;
    char tmp[256];
    char tmp2[256];
    
    output_debug_message("TGLmap::load: loading a map with %i lines\n",lines.size());

    m_bg=0;
    m_star_x=0;
    m_star_y=0;
    m_star_color=0;
    m_cycle=0;
    
    if (2!=sscanf(*next_line,"%i %i",&m_fg_dx,&m_fg_dy)) {
        output_debug_message("TGLmap::load: cannot read width and height of the map!\n");
        return false;
    }
    next_line++;
    m_fg_cell_size=32;
    
    if (1!=sscanf(*next_line,"%s",tmp)) {
        output_debug_message("TGLmap::load: failed to read the background of a map!\n");
        return false;
    }
    next_line++;
    
    if (strcmp(tmp,"rock")==0) set_background(0,GLTM);
    if (strcmp(tmp,"techno-red")==0) set_background(1,GLTM);
    if (strcmp(tmp,"techno-blue")==0) set_background(2,GLTM);
    if (strcmp(tmp,"techno-green")==0) set_background(3,GLTM);
    if (strcmp(tmp,"snow")==0) set_background(4,GLTM);
    
    m_fg=new GLTile *[m_fg_dx*m_fg_dy];
    for(i=0;i<m_fg_dx*m_fg_dy;i++) {
//        printf("'%s'\n",*next_line);
        if (1!=sscanf(*next_line,"%s",tmp)) {
            output_debug_message("TGLmap::load: failed to read the tile at position %i!\n",i);
            return false;
        }
        next_line++;
        int len = int(strlen(tmp));
        if (strcmp(tmp,"-")==0) {
            m_fg[i]=0;
        } else if (tmp[0]=='g' && tmp[1]=='r' && tmp[2]=='a' && tmp[3]=='p' &&
                   tmp[4]=='h' && tmp[5]=='i' && tmp[6]=='c' && tmp[7]=='s' &&
                   tmp[len-4]=='.' && tmp[len-3]=='p' && tmp[len-2]=='n' && tmp[len-1]=='g') {
            m_fg[i]=GLTM->get(tmp);
        } else {
            sprintf(tmp2,"graphics/%s.png",tmp);
            m_fg[i]=GLTM->get(tmp2);
        } // if
    } // for
    
    // objects:
    {
        int no = 0;
        int ix = 0,iy = 0;
        int p1 = 0,p2 = 0;
        int animation_offset;
        float x = 0,y = 0;
        if (1!=sscanf(*next_line,"%i",&no)) {
            output_debug_message("TGLmap::load: failed to read the number of objects!\n");
            return false;
        }
        next_line++;
        
        for(i=0;i<no;i++) {
            int nreadparameters = sscanf(*next_line,"%s %i %i %i %i %i",tmp,&animation_offset,&ix,&iy,&p1,&p2);
            if (nreadparameters<4) {
                output_debug_message("TGLmap::load: less than four elements when loading an object!\n");
                return false;
            }
            next_line++;
            x=float(ix*m_fg_cell_size);
            y=float(iy*m_fg_cell_size);
            y+=STARFIELD;
            TGLobject::addObjectToMap(tmp, x, y, animation_offset, p1, p2, this);
        } // for
    }
    
    create_starField();
    
    return true;
}



void TGLmap::reset(void)
{
	TGLobject_ballstand *bs = (TGLobject_ballstand *)object_exists("TGLobject_ballstand");
    TGLobject_ballstand *ball = (TGLobject_ballstand *)object_exists("TGLobject_ball");
    if (ball!=0) {
        output_debug_message("Something weird happened! map is reset, but ball already exists!");
    }
    m_cycle=0;

	if (bs!=0) {
		TGLobject_ball *ball = new TGLobject_ball(bs->get_x()+16,bs->get_y()+5,0);
		TGLobject_ship *ship = (TGLobject_ship *)object_exists("TGLobject_ship");
		m_fg_objects.push_front(ball);
		if (ball!=0 && ship!=0) {
			ball->exclude_for_collision(ship);
			ship->exclude_for_collision(ball);
		} // if 

	} // if 

	create_laser_objects();
    create_starField();
    
} /* TGLmap::TGLmap */ 


void TGLmap::create_starField(void)
{
    if (starField_VertexArrayID!=0) {
        glDeleteBuffers(1,&starField_Vertexbuffer);
        glDeleteBuffers(1,&starFieldGlow_Vertexbuffer);
#ifdef __APPLE__
        glDeleteVertexArraysAPPLE(1,&starField_VertexArrayID);
        glDeleteVertexArraysAPPLE(1,&starFieldGlow_VertexArrayID);
#else
        glDeleteVertexArrays(1,&starField_VertexArrayID);
        glDeleteVertexArrays(1,&starFieldGlow_VertexArrayID);
#endif
        starField_VertexArrayID = 0;
        starFieldGlow_VertexArrayID = 0;
    }    
    
    // create the starfied background:
    GLfloat *starField_buffer_data = new GLfloat[m_nstars*5];
    for(int i = 0;i<m_nstars;i++) {
        starField_buffer_data[i*5] = m_star_x[i];
        starField_buffer_data[i*5+1] = m_star_y[i];
        starField_buffer_data[i*5+2] = 0;
        starField_buffer_data[i*5+3] = m_star_color[i];
        starField_buffer_data[i*5+4] = 1;
    }
    glGenVertexArrays(1, &starField_VertexArrayID);
    glBindVertexArray(starField_VertexArrayID);
    glGenBuffers(1, &starField_Vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, starField_Vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*m_nstars, starField_buffer_data, GL_DYNAMIC_DRAW);
    delete []starField_buffer_data;
    
    GLfloat *starFieldGlow_buffer_data = new GLfloat[m_nstars*5*8*3];
    for(int i = 0, k = 0;i<m_nstars;i++) {
        float radius = m_star_color[i]*8;
        for(int j = 0;j<8;j++) {
            float angle = (M_PI/4)*j;
            starFieldGlow_buffer_data[k*5] = m_star_x[i];
            starFieldGlow_buffer_data[k*5+1] = m_star_y[i];
            starFieldGlow_buffer_data[k*5+2] = 0;
            starFieldGlow_buffer_data[k*5+3] = m_star_color[i]/2;
            starFieldGlow_buffer_data[k*5+4] = 1;
            k++;
            starFieldGlow_buffer_data[k*5] = m_star_x[i] + cos(angle)*radius;
            starFieldGlow_buffer_data[k*5+1] = m_star_y[i] + sin(angle)*radius;
            starFieldGlow_buffer_data[k*5+2] = 0;
            starFieldGlow_buffer_data[k*5+3] = 0;
            starFieldGlow_buffer_data[k*5+4] = 0;
            k++;
            starFieldGlow_buffer_data[k*5] = m_star_x[i] + cos(angle+(M_PI/4))*radius;
            starFieldGlow_buffer_data[k*5+1] = m_star_y[i] + sin(angle+(M_PI/4))*radius;
            starFieldGlow_buffer_data[k*5+2] = 0;
            starFieldGlow_buffer_data[k*5+3] = 0;
            starFieldGlow_buffer_data[k*5+4] = 0;
            k++;
        }
    }
    glGenVertexArrays(1, &starFieldGlow_VertexArrayID);
    glBindVertexArray(starFieldGlow_VertexArrayID);
    glGenBuffers(1, &starFieldGlow_Vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, starFieldGlow_Vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*8*3*m_nstars, starFieldGlow_buffer_data, GL_DYNAMIC_DRAW);
    delete []starFieldGlow_buffer_data;
}


void TGLmap::create_laser_objects(void)
{
	std::list<TGLobject *> todelete;
	TGLobject *pair;


	// First remove all the laser objects existing in the map:
    for(TGLobject *o:m_fg_objects) {
		if (o->is_a("TGLobject_laser_horizontal") || o->is_a("TGLobject_laser_vertical")) todelete.push_back(o);
	} // while 

    for(TGLobject *o:todelete) {
		m_fg_objects.remove(o);
		delete o;
	} // while 

    for(TGLobject *o:m_fg_objects) {
        if (o->is_a("TGLobject_laser_left")) {
			// Fint pair:
			pair=0;
            for(TGLobject *o2:m_fg_objects) {
    			if (o2->is_a("TGLobject_laser_right") &&
					o2->get_y()==o->get_y() &&
					o2->get_x()>o->get_x()) {
					if (pair==0) pair=o2;
					if (pair->get_x()>o2->get_x()) pair=o2;
				} // if 
			} // while 

			if (pair!=0) {
				float i;
				for(i=o->get_x();i<=pair->get_x();i+=m_fg_cell_size) {
					m_fg_objects.push_front(new TGLobject_laser_horizontal(i,o->get_y(),o->get_animation_offset(),o,pair));
				} // for
			} // if 
		} // if 

		if (o->is_a("TGLobject_laser_up")) {
			// Fint pair:
			pair=0;
            for(TGLobject *o2:m_fg_objects) {
				if (o2->is_a("TGLobject_laser_down") &&
					o2->get_x()==o->get_x() &&
					o2->get_y()>o->get_y()) {
					if (pair==0) pair=o2;
					if (pair->get_y()>o2->get_y()) pair=o2;
				} // if 
			} // while 

			if (pair!=0) {
				float i;
				for(i=o->get_y();i<=pair->get_y();i+=m_fg_cell_size) {
					m_fg_objects.push_front(new TGLobject_laser_vertical(o->get_x(),i,o->get_animation_offset(),o,pair));
				} // for
			} // if 
		} // if 
	} // while 
} /* TGLmap::create_laser_objects */ 


int TGLmap::get_cycle(void)
{
	return m_cycle;
} /* TGLmap::get_cycle */ 


void TGLmap::set_background(int type,GLTManager *GLTM)
{
	int i,j;

	m_bg_code = type;

	if (type==0) {
		// Create a rock backgorund:
		if (m_bg!=0) {
			for(i=0;i<m_bg_dx*m_bg_dy;i++) m_bg[i]=0;
			delete []m_bg;
		} // if
		
		m_bg_dx=(m_fg_dx+1)/2;
		m_bg_dy=(m_fg_dy+1)/2;
		m_bg_cell_size=64;

		m_bg=new GLTile *[m_bg_dx*m_bg_dy];
		for(i=0;i<m_bg_dx;i++) m_bg[i]=GLTM->get("graphics/background/top-rock.png");
		for(i=0;i<m_bg_dx;i++) m_bg[i+m_bg_dx]=GLTM->get("graphics/background/middle-rock.png");
		for(j=2;j<m_bg_dy;j++) {
			for(i=0;i<m_bg_dx;i++) m_bg[i+m_bg_dx*j]=GLTM->get("graphics/background/bottom-rock.png");
		} // for
	} // if 

	if (type==1) {
		// Create a techno-red backgorund:
		if (m_bg!=0) {
			for(i=0;i<m_bg_dx*m_bg_dy;i++) m_bg[i]=0;
			delete []m_bg;
		} // if

		m_bg_dx=(m_fg_dx+1)/2;
		m_bg_dy=(m_fg_dy+1)/2;
		m_bg_cell_size=64;

		m_bg=new GLTile *[m_bg_dx*m_bg_dy];
		for(i=0;i<m_bg_dx;i++) m_bg[i]=GLTM->get("graphics/background/top-techno-red.png");
		for(j=1;j<m_bg_dy;j++) {
			for(i=0;i<m_bg_dx;i++) m_bg[i+m_bg_dx*j]=GLTM->get("graphics/background/bottom-techno-red.png");
		} // for
	} // if 

	if (type==2) {
		// Create a techno-blue backgorund:
		if (m_bg!=0) {
			for(i=0;i<m_bg_dx*m_bg_dy;i++) m_bg[i]=0;
			delete []m_bg;
		} // if

		m_bg_dx=(m_fg_dx+1)/2;
		m_bg_dy=(m_fg_dy+1)/2;
		m_bg_cell_size=64;

		m_bg=new GLTile *[m_bg_dx*m_bg_dy];
		for(i=0;i<m_bg_dx;i++) m_bg[i]=GLTM->get("graphics/background/top-techno-blue.png");
		for(j=1;j<m_bg_dy;j++) {
			for(i=0;i<m_bg_dx;i++) m_bg[i+m_bg_dx*j]=GLTM->get("graphics/background/bottom-techno-blue.png");
		} // for
	} // if 

	if (type==3) {
		// Create a techno-green backgorund:
		if (m_bg!=0) {
			for(i=0;i<m_bg_dx*m_bg_dy;i++) m_bg[i]=0;
			delete []m_bg;
		} // if

		m_bg_dx=(m_fg_dx+1)/2;
		m_bg_dy=(m_fg_dy+1)/2;
		m_bg_cell_size=64;

		m_bg=new GLTile *[m_bg_dx*m_bg_dy];
		for(i=0;i<m_bg_dx;i++) m_bg[i]=GLTM->get("graphics/background/top-techno-green.png");
		for(j=1;j<m_bg_dy;j++) {
			for(i=0;i<m_bg_dx;i++) m_bg[i+m_bg_dx*j]=GLTM->get("graphics/background/bottom-techno-green.png");
		} // for
	} // if 

	if (type==4) {
		// Create a snow backgorund:
		if (m_bg!=0) {
			for(i=0;i<m_bg_dx*m_bg_dy;i++) m_bg[i]=0;
			delete []m_bg;
		} // if

		m_bg_dx=(m_fg_dx+1)/2;
		m_bg_dy=(m_fg_dy+1)/2;
		m_bg_cell_size=64;

		m_bg=new GLTile *[m_bg_dx*m_bg_dy];
		for(i=0;i<m_bg_dx;i++) m_bg[i]=0;
		for(i=0;i<m_bg_dx;i++) m_bg[i+m_bg_dx]=0;
		for(i=0;i<m_bg_dx;i+=2) m_bg[i]=GLTM->get("graphics/background/top-snow.png");
		for(j=2;j<m_bg_dy;j++) {
			for(i=0;i<m_bg_dx;i++) m_bg[i+m_bg_dx*j]=GLTM->get("graphics/background/bottom-snow.png");
		} // for
	} // if 

	if (m_star_x!=0) {
		delete []m_star_x;
		delete []m_star_y;
		delete []m_star_color;
	} // if

	{
		m_nstars=STARFIELD_STARS*m_fg_dx;
		m_star_x=new int[m_nstars];
		m_star_y=new int[m_nstars];
		m_star_color=new float[m_nstars];
		
		for(i=0;i<m_nstars;i++) {
			m_star_x[i]=rand()%(m_fg_dx*m_fg_cell_size);
			m_star_y[i]=rand()%(STARFIELD+32);
			m_star_color[i]=((rand()%1000)+1)*0.001f;
			m_star_color[i]*=m_star_color[i];
		} // for
	}


} /* TGLmap::set_background */ 


TGLmap::~TGLmap()
{
	int i;

//    printf("deleting m_fg_objects:\n");
    for(TGLobject *o:m_fg_objects) {
//        printf("  %p - %s\n", o, o->get_class());
        delete o;
    }
    m_fg_objects.clear();
    m_fg_ships.clear();
    for(TGLobject *o:m_auxiliary_front_objects) delete o;
    m_auxiliary_front_objects.clear();
    for(TGLobject *o:m_auxiliary_back_objects) delete o;
    m_auxiliary_back_objects.clear();

	for(i=0;i<m_bg_dx*m_bg_dy;i++) m_bg[i]=0;
	delete []m_bg;

	for(i=0;i<m_fg_dx*m_fg_dy;i++) m_fg[i]=0;
	delete []m_fg;

	delete []m_star_x;
	delete []m_star_y;
	delete []m_star_color;
    
    if (starField_VertexArrayID!=0) {
        glDeleteBuffers(1,&starField_Vertexbuffer);
        glDeleteBuffers(1,&starFieldGlow_Vertexbuffer);
#ifdef __APPLE__
        glDeleteVertexArraysAPPLE(1,&starField_VertexArrayID);
        glDeleteVertexArraysAPPLE(1,&starFieldGlow_VertexArrayID);
#else
        glDeleteVertexArrays(1,&starField_VertexArrayID);
        glDeleteVertexArrays(1,&starFieldGlow_VertexArrayID);
#endif
        starField_VertexArrayID = 0;
        starFieldGlow_VertexArrayID = 0;
    }
    
} /* TGLmap::~TGLmap */ 


bool TGLmap::editor_cycle(GLTManager *GLTM)
{
	std::list<TGLobject *> todelete;

    for(TGLobject *o:m_fg_objects) {
		if (!o->editor_cycle(this,GLTM)) {
			todelete.push_back(o);
		} // if 
	} // while 

    for(TGLobject *o:todelete) {
		m_fg_ships.remove(o);
		m_fg_objects.remove(o);
		delete o;
	} // while
    todelete.clear();

    for(TGLobject *o:m_auxiliary_front_objects) {
		if (!o->editor_cycle(this,GLTM)) {
			todelete.push_back(o);
		} // if 
	} // while 

    for(TGLobject *o:todelete) {
		m_auxiliary_front_objects.remove(o);
		delete o;
	} // while 

    for(TGLobject *o:m_auxiliary_back_objects) {
		if (!o->editor_cycle(this,GLTM)) {
			todelete.push_back(o);
		} // if 
	} // while 

    for(TGLobject *o:todelete) {
        m_auxiliary_back_objects.remove(o);
		delete o;
	} // while 

	m_cycle++;

	return true;
} /* TGLmap::editor_cycle */ 


bool TGLmap::cycle(std::list<VirtualController *> *lv,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	std::list<TGLobject *> todelete;

    for(TGLobject *o:m_fg_objects) {
		if (!o->cycle(getAt<VirtualController *>(*lv,o->get_controller()),this,GLTM,SFXM,sfx_volume)) {
			todelete.push_back(o);
		} // if 
	} // while 

    for(TGLobject *o:todelete) {
		m_fg_ships.remove(o);
		m_fg_objects.remove(o);
		delete o;
	} // while
    todelete.clear();

    for(TGLobject *o:m_auxiliary_front_objects) {
		if (!o->cycle(getAt<VirtualController *>(*lv,o->get_controller()),this,GLTM,SFXM,sfx_volume)) {
            todelete.push_back(o);
        } // if
	} // while 

    for(TGLobject *o:todelete) {
		m_auxiliary_front_objects.remove(o);
		delete o;
	} // while 
    todelete.clear();
    
    for(TGLobject *o:m_auxiliary_back_objects) {
		if (!o->cycle(getAt<VirtualController *>(*lv,o->get_controller()),this,GLTM,SFXM,sfx_volume)) {
			todelete.push_back(o);
		} // if 
	} // while 

    for(TGLobject *o:todelete) {
		m_auxiliary_back_objects.remove(o);
		delete o;
	} // while 
    todelete.clear();

	m_cycle++;

	return true;
} /* TGLmap::cycle */ 



void TGLmap::draw(int focus_x,int focus_y,int dx,int dy,GLTManager *GLTM,float zoom, bool draw_for_editor)
{
    if (!draw_for_editor) {
        if (m_fg_dx*m_fg_cell_size*zoom < dx) {
            focus_x = m_fg_dx*m_fg_cell_size/2;
        } else {
            if (focus_x>(m_fg_dx*m_fg_cell_size-dx/(2*zoom))) focus_x=(m_fg_dx*m_fg_cell_size-dx/(2*zoom));
            if (focus_x<dx/(2*zoom)) focus_x=dx/(2*zoom);
        }
        if (focus_y>(m_fg_dy*m_fg_cell_size+STARFIELD-dy/(2*zoom))) focus_y=(m_fg_dy*m_fg_cell_size+STARFIELD-dy/(2*zoom));
        if (focus_y<dy/(2*zoom)) focus_y=dy/(2*zoom);
    }
    int offsx=int((focus_x)-(dx/(2*zoom)));
    int offsy=int((focus_y)-(dy/(2*zoom)));
    
	// Draw starfield:
    if (starField_VertexArrayID!=0) {
        int sf_offsx = offsx/4;
        int sf_offsy = offsy/4;
        // this is the case when the level is narrower than the screen:
        if (offsx<0) sf_offsx = offsx;
        
        int loc1 = glGetAttribLocation(programID, "vPosition");
        int loc2 = glGetAttribLocation(programID, "UV");
        // this is the case when the level is narrower than the screen:
        glUniform4f(inColorID, 1, 1, 1, 1);
        glm::mat4 tmp = glm::mat4(1.0f);
        if (zoom!=1) tmp = glm::scale(tmp, glm::vec3(zoom, zoom, zoom));
        tmp = glm::translate(tmp, glm::vec3(-sf_offsx,-sf_offsy/4,0));
        glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
        glUniform1i(useTextureID, 2);
        
        glBindVertexArray(starFieldGlow_VertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, starFieldGlow_Vertexbuffer);
        glEnableVertexAttribArray(loc1);
        glEnableVertexAttribArray(loc2);
        glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
        glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
        glDrawArrays(GL_TRIANGLES, 0, m_nstars*5*8*3);
        glDisableVertexAttribArray(loc1);
        glDisableVertexAttribArray(loc2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        glBindVertexArray(starField_VertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, starField_Vertexbuffer);
        glEnableVertexAttribArray(loc1);
        glEnableVertexAttribArray(loc2);
        glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
        glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
        glDrawArrays(GL_POINTS, 0, m_nstars);
        glDisableVertexAttribArray(loc1);
        glDisableVertexAttribArray(loc2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

	// Draw background:
	{
		int i,j,k;
        int bg_offsx = offsx/2;
        int bg_offsy = offsy/2;
        // this is the case when the level is narrower than the screen:
        if (offsx<0) bg_offsx = offsx;
 
        glm::mat4 transform = glm::mat4(1.0f);
        if (zoom!=1) transform = glm::scale(transform, glm::vec3(zoom, zoom, zoom));
        transform = glm::translate(transform, glm::vec3(-bg_offsx,-bg_offsy,0));
        
        int left_most = std::max(0,bg_offsx/m_bg_cell_size-1);
        int right_most = std::min(m_bg_dx, int(left_most + (dx/zoom)/m_bg_cell_size)+3);
        int top_most = std::max(0,(bg_offsy-STARFIELD)/m_bg_cell_size-1);
        int bottom_most = std::min(m_bg_dy, int(top_most + (dy/zoom)/m_bg_cell_size)+3);
        
        for(i=top_most;i<bottom_most;i++) {
            k=i*m_bg_dx+left_most;
            for(j=left_most;j<right_most;j++,k++) {
                if (m_bg[k]!=0) {
                    glm::mat4 tmp2 = glm::translate(transform, glm::vec3(j*m_bg_cell_size,i*m_bg_cell_size+STARFIELD,0));
                    m_bg[k]->draw(tmp2);
                } // if
            } // for
        } // for
        
	}

    glm::mat4 transform = glm::mat4(1.0f);
    if (zoom!=1) transform = glm::scale(transform, glm::vec3(zoom, zoom, zoom));
    transform = glm::translate(transform, glm::vec3(-offsx,-offsy,0));

    // Draw auxiliary back objects:
	{
        for(TGLobject *o:m_auxiliary_back_objects) {
			o->draw(transform,GLTM);
        } // while
	}


	// Draw objects:
	{
        for(TGLobject *o:m_fg_objects) {
            o->draw(transform,GLTM);
		} // while
	}

	// Draw foreground:
	{
		int i,j,k;
        
        int left_most = std::max(0,offsx/m_fg_cell_size-1);
        int right_most = std::min(m_fg_dx, int(left_most + (dx/zoom)/m_fg_cell_size)+3);
        int top_most = std::max(0,(offsy-STARFIELD)/m_fg_cell_size-1);
        int bottom_most = std::min(m_fg_dy, int(top_most + (dy/zoom)/m_fg_cell_size)+3);
        
		for(i=top_most;i<bottom_most;i++) {
			k=i*m_fg_dx+left_most;
			for(j=left_most;j<right_most;j++,k++) {
				if (m_fg[k]!=0) {
                    glm::mat4 tmp2 = glm::translate(transform, glm::vec3(j*m_fg_cell_size,i*m_fg_cell_size+STARFIELD,0));
                    m_fg[k]->draw(tmp2);
				} // if
			} // for
		} // for
	}

	// Draw auxiliary front objects:
	{
        for(TGLobject *o:m_auxiliary_front_objects) {
            o->draw(transform,GLTM);
		} // while 
	}


	// Draw the bounding box of the editable space:
	if (draw_for_editor) {
        drawLine(float(-offsx)*zoom,float(STARFIELD-offsy)*zoom,
                 float(get_dx()-offsx)*zoom,float(STARFIELD-offsy)*zoom,
                 1,1,1,0.5f);
        drawLine(float(get_dx()-offsx)*zoom,float(STARFIELD-offsy)*zoom,
                 float(get_dx()-offsx)*zoom,float(get_dy()-offsy)*zoom,
                 1,1,1,0.5f);
        drawLine(float(get_dx()-offsx)*zoom,float(get_dy()-offsy)*zoom,
                 float(-offsx)*zoom,float(get_dy()-offsy)*zoom,
                 1,1,1,0.5f);
        drawLine(float(-offsx)*zoom,float(get_dy()-offsy)*zoom,
                 float(-offsx)*zoom,float(STARFIELD-offsy)*zoom,
                 1,1,1,0.5f);
	}

	
} /* TGLmap::draw */ 


SDL_Surface *TGLmap::drawToSurface(GLTManager *GLTM)
{
    SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE,m_fg_dx * m_fg_cell_size, m_fg_dy * m_fg_cell_size, 32,RMASK,GMASK,BMASK,AMASK);
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = m_fg_dx * m_fg_cell_size;
    r.h = m_fg_dy * m_fg_cell_size;
    SDL_FillRect(sfc, &r, AMASK);
    
    // background tiles:
    for(int i=0;i<m_bg_dy;i++) {
        int k=i*m_bg_dx;
        for(int j=0;j<m_bg_dx;j++,k++) {
            if (m_bg[k]!=0) {
                GLTile *t = m_bg[k];
                if (t!=0) {
                    SDL_Rect r;
                    r.x=j*m_bg_cell_size - t->get_hot_x();
                    r.y=i*m_bg_cell_size - t->get_hot_y();
                    r.w=t->getSurface()->w;
                    r.h=t->getSurface()->h;
                    SDL_BlitSurface(t->getSurface(),0,sfc,&r);
                }
            } // if
        } // for
    } // for
    
    // note: this assumes the obejct has been drawn before (since this is always called from the editor, it should be fine):
    for(TGLobject *o:m_fg_objects) {
        GLTile *t = o->get_last_tile();
        if (t!=0) {
            SDL_Rect r;
            
            r.x=(int)o->get_x() - t->get_hot_x();
            r.y=(int)(o->get_y() - STARFIELD) - t->get_hot_y();
            r.w=t->getSurface()->w;
            r.h=t->getSurface()->h;
            SDL_BlitSurface(t->getSurface(),0,sfc,&r);
        }
    }
    
    // foreground tiles:
    for(int i = 0;i<m_fg_dy;i++) {
        for(int j = 0;j<m_fg_dx;j++) {
            GLTile *t = m_fg[j+i*m_fg_dx];
            
            if (t!=0) {
                SDL_Rect r;
                
                r.x=j*m_fg_cell_size - t->get_hot_x();
                r.y=i*m_fg_cell_size - t->get_hot_y();
                r.w=t->getSurface()->w;
                r.h=t->getSurface()->h;
                SDL_BlitSurface(t->getSurface(),0,sfc,&r);
            }
        }
    }
    
    return sfc;
}


void TGLmap::add_object(TGLobject *o)
{
	m_fg_objects.push_back(o);
	if (o->is_a("TGLobject_ship")) m_fg_ships.push_back(o);
	o->set_cycle(get_cycle());
} /* TGLmap::add_object */ 


void TGLmap::add_object_back(TGLobject *o)
{
	m_fg_objects.push_front(o);
	if (o->is_a("TGLobject_ship")) m_fg_ships.push_back(o);
	o->set_cycle(get_cycle());
} /* TGLmap::add_object_back */ 


void TGLmap::remove_object(TGLobject *o)
{
	m_fg_objects.remove(o);
	if (o->is_a("TGLobject_ship")) m_fg_ships.remove(o);
} /* TGLmap::remove_object */ 


void TGLmap::remove_auxiliary_front_object(class TGLobject *o)
{
	m_auxiliary_front_objects.remove(o);
} /* TGLmap::remove_auxiliary_front_object */ 


void TGLmap::remove_auxiliary_back_object(class TGLobject *o)
{
	m_auxiliary_back_objects.remove(o);
} /* TGLmap::remove_auxiliary_back_object */ 


void TGLmap::add_auxiliary_front_object(TGLobject *o)
{
	m_auxiliary_front_objects.push_back(o);
} /* TGLmap::add_auxiliary_front_object */ 


void TGLmap::insert_auxiliary_front_object(TGLobject *o)
{
	m_auxiliary_front_objects.push_front(o);
} /* TGLmap::add_auxiliary_front_object */ 


void TGLmap::add_auxiliary_back_object(TGLobject *o)
{
	m_auxiliary_back_objects.push_back(o);
} /* TGLmap::add_auxiliary_back_object */ 


bool TGLmap::object_exists(TGLobject *o)
{
    for(TGLobject *o2:m_fg_objects) {
        if (o==o2) return true;
    }
    return false;
} /* TGLmap::object_exists */


std::list<TGLobject *> *TGLmap::get_objects(const char *type)
{
	std::list<TGLobject *> *l=new std::list<TGLobject *>;
    for(TGLobject *o:m_fg_objects) {
		if (o->is_a(type)) l->push_back(o);
	} // while 

	return l;
} /* TGLmap::get_objects */ 



TGLobject *TGLmap::object_exists(const char *type)
{
    if (strcmp(type,"TGLobject_ship")==0) {
        for(TGLobject *o:m_fg_ships) {
            if (o->is_a(type)) return o;
        }
    } else {
        for(TGLobject *o:m_fg_objects) {
            if (o->is_a(type)) return o;
        }
    }
	return 0;
} /* TGLmap::object_exists */ 


TGLobject *TGLmap::object_exists(const char *type,float x1,float y1,float x2,float y2)
{

    if (strcmp(type,"TGLobject_ship")==0) {
        for(TGLobject *o:m_fg_ships) {
            if (o->get_x()>=x1 &&
                o->get_y()>=y1 &&
                o->get_x()<x2 &&
                o->get_y()<y2 &&
                o->is_a(type)) return o;
        } // while
    } else {
        for(TGLobject *o:m_fg_objects) {
            if (o->get_x()>=x1 &&
                o->get_y()>=y1 &&
                o->get_x()<x2 &&
                o->get_y()<y2 &&
                o->is_a(type)) return o;
        } // while
    }
	return 0;
} /* TGLmap::object_exists */ 


bool TGLmap::collision(TGLobject *o,float offsx,float offsy,int offs_alpha)
{
	GLTile *mask;
	SDL_Surface *sfc=0;
	float x,y;
	int hx,hy,angle;
	float s1;

	mask=o->get_last_mask();
	if (mask==0) return false;
	sfc=mask->getSurface();
	x=o->get_x()+offsx;
	y=o->get_y()+offsy;
	hx=mask->get_hot_x();
	hy=mask->get_hot_y();
	angle=o->get_angle()+offs_alpha;
	s1=o->get_scale();

	// Collision against objects:
	{
        for(TGLobject *o2:m_fg_objects) {
			if (o2!=o && o->check_collision(o2) && o2->check_collision(o)) {
				GLTile *mask2;
				SDL_Surface *sfc2=0;
				float x2,y2;
				int hx2,hy2,angle2;
				float s2;

				mask2=o2->get_last_mask();
				if (mask2!=0) {
					sfc2=mask2->getSurface();
					x2=o2->get_x();
					y2=o2->get_y();
					hx2=mask2->get_hot_x();
					hy2=mask2->get_hot_y();
					angle2=o2->get_angle();
					s2=o2->get_scale();

					if (::collision(o,sfc,x,y,hx,hy,angle,s1,o2,sfc2,x2,y2,hx2,hy2,angle2,s2)) return true;
				} // if 

			} // if 
		} // while 
	}

	// Collision with foreground:
	{
		int i,j,k;
		int start_x,start_y,end_x,end_y;
		
		// This assumes that the objects are small! (64x64 at most)
		start_x=int((x/m_fg_cell_size)-3);
		end_x=int((x/m_fg_cell_size)+3);
		start_y=int(((y-STARFIELD)/m_fg_cell_size)-3);
		end_y=int(((y-STARFIELD)/m_fg_cell_size)+3);

		if (start_x<0) start_x=0;
		if (start_y<0) start_y=0;
		if (end_x>m_fg_dx) end_x=m_fg_dx;
		if (end_y>m_fg_dy) end_y=m_fg_dy;

		for(i=start_y;i<end_y;i++) {
			k=i*m_fg_dx+start_x;
			for(j=start_x;j<end_x;j++,k++) {
				if (m_fg[k]!=0) {
					SDL_Surface *sfc2=m_fg[k]->getSurface();
					if (::collision(o,sfc,x,y,hx,hy,angle,s1,0,sfc2,float(j*m_fg_cell_size),float(i*m_fg_cell_size+STARFIELD),0,0,0,1)) return true;
				} // if 
			} // for
		} // for
	}

	return false;
} /* TGLmap::collision */ 


bool TGLmap::collision_vector(TGLobject *o,float *vx,float *vy)
{
	GLTile *mask;
	SDL_Surface *sfc=0;
	float x,y;
	int hx,hy,angle;
	float s1;

	mask=o->get_last_mask();
	if (mask==0) return false;
	sfc=mask->getSurface();
	x=o->get_x();
	y=o->get_y();
	hx=mask->get_hot_x();
	hy=mask->get_hot_y();
	angle=o->get_angle();
	s1=o->get_scale();

	// Collision against objects:
	{
        for(TGLobject *o2:m_fg_objects) {
			if (o2!=o && o->check_collision(o2) && o2->check_collision(o)) {
				GLTile *mask2;
				SDL_Surface *sfc2=0;
				float x2,y2;
				int hx2,hy2,angle2;
				float s2;

				mask2=o2->get_last_mask();
				if (mask2!=0) {
					sfc2=mask2->getSurface();
					x2=o2->get_x();
					y2=o2->get_y();
					hx2=mask2->get_hot_x();
					hy2=mask2->get_hot_y();
					angle2=o2->get_angle();
					s2=o2->get_scale();

					if (::collision_vector(o,sfc,x,y,hx,hy,angle,s1,o2,sfc2,x2,y2,hx2,hy2,angle2,s2,vx,vy)) return true;
				} // if 

			} // if 
		} // while 
	}

	// Collision with foreground:
	{
		int i,j,k;
		int start_x,start_y,end_x,end_y;
		
		// This assumes that the objects are small! (64x64 at most)
		start_x=int((x/m_fg_cell_size)-3);
		end_x=int((x/m_fg_cell_size)+3);
		start_y=int(((y-STARFIELD)/m_fg_cell_size)-3);
		end_y=int(((y-STARFIELD)/m_fg_cell_size)+3);

		if (start_x<0) start_x=0;
		if (start_y<0) start_y=0;
		if (end_x>m_fg_dx) end_x=m_fg_dx;
		if (end_y>m_fg_dy) end_y=m_fg_dy;

		for(i=start_y;i<end_y;i++) {
			k=i*m_fg_dx+start_x;
			for(j=start_x;j<end_x;j++,k++) {
				if (m_fg[k]!=0) {
					SDL_Surface *sfc2=m_fg[k]->getSurface();
					if (::collision_vector(o,sfc,x,y,hx,hy,angle,s1,0,sfc2,float(j*m_fg_cell_size),float(i*m_fg_cell_size+STARFIELD),0,0,0,1,vx,vy)) return true;
				} // if 
			} // for
		} // for
	}

	return false;
} /* TGLmap::collision_vector */ 



TGLobject *TGLmap::collision_with_object(TGLobject *o)
{
	GLTile *mask;
	SDL_Surface *sfc=0;
	float x,y;
	int hx,hy,angle;
	float s1;

	mask=o->get_last_mask();
	if (mask==0) return 0;
	sfc=mask->getSurface();
	x=o->get_x();
	y=o->get_y();
	hx=mask->get_hot_x();
	hy=mask->get_hot_y();
	angle=o->get_angle();
	s1=o->get_scale();

	// Collision against objects:
	{
        for(TGLobject *o2:m_fg_objects) {
			if (o2!=o && o->check_collision(o2) && o2->check_collision(o)) {
				GLTile *mask2;
				SDL_Surface *sfc2=0;
				float x2,y2;
				int hx2,hy2,angle2;
				float s2;

				mask2=o2->get_last_mask();
				if (mask2!=0) {
					sfc2=mask2->getSurface();
					x2=o2->get_x();
					y2=o2->get_y();
					hx2=mask2->get_hot_x();
					hy2=mask2->get_hot_y();
					angle2=o2->get_angle();
					s2=o2->get_scale();

					if (::collision(o,sfc,x,y,hx,hy,angle,s1,o2,sfc2,x2,y2,hx2,hy2,angle2,s2)) return o2;
				} // if 

			} // if 
		} // while 
	}

	return 0;
} /* TGLmap::collision_with_object */ 


TGLobject *TGLmap::collision_with_object(float x,float y)
{
    for(TGLobject *o2:m_fg_objects) {
		GLTile *mask2;
		SDL_Surface *sfc2=0;
		float x2,y2;
		int hx2,hy2,angle2;
		float s2;

		mask2=o2->get_last_mask();
		if (mask2!=0) {
			sfc2=mask2->getSurface();
			x2=o2->get_x();
			y2=o2->get_y();
			hx2=mask2->get_hot_x();
			hy2=mask2->get_hot_y();
			angle2=o2->get_angle();
			s2=o2->get_scale();

			if (::collision_with_point(o2,sfc2,x2,y2,hx2,hy2,angle2,s2,x,y)) return o2;
		} // if 

	} // while 

	return 0;
} /* TGLmap::collision_with_object */ 


bool TGLmap::collision_with_foreground(TGLobject *o,float offsx,float offsy,int offs_alpha)
{
	GLTile *mask;
	SDL_Surface *sfc=0;
	float x,y;
	int hx,hy,angle;
	float s1;
	bool transformation_done=false;

	mask=o->get_last_mask();
	if (mask==0) return false;
	sfc=mask->getSurface();
	x=o->get_x()+offsx;
	y=o->get_y()+offsy;
	hx=mask->get_hot_x();
	hy=mask->get_hot_y();
	angle=o->get_angle()+offs_alpha;
	s1=o->get_scale();

	// Collision with foreground:
	{
		int i,j,k;
		int start_x,start_y,end_x,end_y;
		
		// This assumes that the objects are small! (64x64 at most)
		start_x=int((x/m_fg_cell_size)-3);
		end_x=int((x/m_fg_cell_size)+3);
		start_y=int(((y-STARFIELD)/m_fg_cell_size)-3);
		end_y=int(((y-STARFIELD)/m_fg_cell_size)+3);

		if (start_x<0) start_x=0;
		if (start_y<0) start_y=0;
		if (end_x>m_fg_dx) end_x=m_fg_dx;
		if (end_y>m_fg_dy) end_y=m_fg_dy;

		for(i=start_y;i<end_y;i++) {
			k=i*m_fg_dx+start_x;
			for(j=start_x;j<end_x;j++,k++) {
				if (m_fg[k]!=0) {

					if ((angle!=0 || s1!=1) && !transformation_done) {
						// Transform the object only once for not repeating the transformation many times in the collision routine:
						SDL_Surface *obj1_sfc;
//						int minx=0,maxx=0;
//						int miny=0,maxy=0;
						int new_hot_x1,new_hot_y1;
						SDL_Rect r1;

						// Draw the first object:
						if (angle==0 && s1==1) obj1_sfc=sfc;
										  else obj1_sfc=rotozoomSurface(sfc,-angle,s1,0);
						{
							float rad_ang1=float((float(-angle)*M_PI))/180;
							float tmpx1=hx-float(sfc->w)/2;
							float tmpy1=hy-float(sfc->h)/2;
							float cx1=float(obj1_sfc->w)/2;
							float cy1=float(obj1_sfc->h)/2;
							new_hot_x1=int(cx1+((cos(rad_ang1)*tmpx1+sin(rad_ang1)*tmpy1))*s1);
							new_hot_y1=int(cy1+((-sin(rad_ang1)*tmpx1+cos(rad_ang1)*tmpy1))*s1);

							r1.x=int(x-new_hot_x1);
							r1.y=int(y-new_hot_y1);
							r1.w=obj1_sfc->w;
							r1.h=obj1_sfc->h;

							sfc=obj1_sfc;
							x=r1.x;
							y=r1.y;
							hx=0;
							hy=0;
							angle=0;
							s1=1;

							transformation_done=true;
						}
					} // if 

					SDL_Surface *sfc2=m_fg[k]->getSurface();
					if (::collision(o,sfc,x,y,hx,hy,angle,s1,0,sfc2,float(j*m_fg_cell_size),float(i*m_fg_cell_size+STARFIELD),0,0,0,1)) {
						if (transformation_done) SDL_FreeSurface(sfc);
						return true;
					} // if 
				} // if 
			} // for
		} // for
	}

	if (transformation_done) SDL_FreeSurface(sfc);
	return false;
} /* TGLmap::collision_with_foreground */ 


void TGLmap::action(int action)
{
    for(TGLobject *o:m_fg_objects) {
		if (o->is_a("TGLobject_leftdoor")) {
			((TGLobject_leftdoor *)o)->action(action);
		} // if 
		if (o->is_a("TGLobject_rightdoor")) {
			((TGLobject_rightdoor *)o)->action(action);
		} // if 
	} // while 

} /* TGLmap::action */ 


void TGLmap::resize(int dx,int dy,GLTManager *GLTM)
{
	int i;
	int x,y;
	GLTile **m_fg_tmp;

	m_fg_tmp=new GLTile *[dx*dy];
	for(i=0;i<dx*dy;i++) {
		x = i%dx;
		y = i/dx;
		if ( x<m_fg_dx && y<m_fg_dy ) {
			m_fg_tmp[i]=m_fg[x+y*m_fg_dx];
			m_fg[x+y*m_fg_dx]=0;
		} else {
			m_fg_tmp[i]=0;
		} // if 
	} // for 

	delete []m_fg;
	m_fg = m_fg_tmp;
	m_fg_dx = dx;
	m_fg_dy = dy;


    std::list<TGLobject *> to_delete;
    for(TGLobject *o:m_fg_objects) {
		if (o->get_x()>m_fg_dx*m_fg_cell_size ||
			o->get_y()>m_fg_dy*m_fg_cell_size) {
			to_delete.push_back(o);
		} // if
	} // while 

    for(TGLobject *o:to_delete) {
		remove_object(o);
		delete o;
	} // while 
    to_delete.clear();
    
	set_background(m_bg_code,GLTM);
    
    create_starField();
    
} /* TGLmap::resize */ 


GLTile *TGLmap::getFGTile(int i)
{
	return m_fg[i];
}


void TGLmap::save(FILE *fp,GLTManager *GLTM)
{
	int i;

	fprintf(fp,"%i %i\n",m_fg_dx,m_fg_dy);
	switch(m_bg_code) {
	case 1: fprintf(fp,"techno-red\n");	break;
	case 2: fprintf(fp,"techno-blue\n");	break;
	case 3: fprintf(fp,"techno-green\n");	break;
	case 4: fprintf(fp,"snow\n");	break;
	default: fprintf(fp,"rock\n");	break;
	} // switch
	
	for(i=0;i<m_fg_dx*m_fg_dy;i++) {
		if (m_fg[i]==0) {
			fprintf(fp,"-\n");
		} else {
			fprintf(fp,"%s\n",GLTM->get_name(m_fg[i]));
		} // if 
	} // for 

	/*

		Infer the number of objects and save them...

	*/ 

	{
		int no=0;
        for(TGLobject *o:m_fg_objects) {
			if (o->is_a("TGLobject_ballstand")) no++;
			if (o->is_a("TGLobject_redlight")) no++;
			if (o->is_a("TGLobject_radar")) no++;
			if (o->is_a("TGLobject_pipevscreen")) no++;
			if (o->is_a("TGLobject_pipehscreen")) no++;
			if (o->is_a("TGLobject_laser_left")) no++;
			if (o->is_a("TGLobject_laser_right")) no++;
			if (o->is_a("TGLobject_laser_up")) no++;
			if (o->is_a("TGLobject_laser_down")) no++;
			if (o->is_a("TGLobject_cannon_left")) no++;
			if (o->is_a("TGLobject_cannon_right")) no++;
			if (o->is_a("TGLobject_cannon_down")) no++;
			if (o->is_a("TGLobject_cannon_up")) no++;
			if (o->is_a("TGLobject_fastcannon_left")) no++;
			if (o->is_a("TGLobject_fastcannon_right")) no++;
			if (o->is_a("TGLobject_fastcannon_down")) no++;
			if (o->is_a("TGLobject_fastcannon_up")) no++;
			if (o->is_a("TGLobject_spike_left")) no++;
			if (o->is_a("TGLobject_spike_right")) no++;
			if (o->is_a("TGLobject_fuelrecharge")) no++;
			if (o->is_a("TGLobject_techno_computer")) no++;
			if (o->is_a("TGLobject_directionalcannon_left")) no++;
			if (o->is_a("TGLobject_directionalcannon_right")) no++;
			if (o->is_a("TGLobject_directionalcannon_up")) no++;
			if (o->is_a("TGLobject_directionalcannon_down")) no++;
			if (o->is_a("TGLobject_ha_directionalcannon_left")) no++;
			if (o->is_a("TGLobject_ha_directionalcannon_right")) no++;
			if (o->is_a("TGLobject_ha_directionalcannon_up")) no++;
			if (o->is_a("TGLobject_ha_directionalcannon_down")) no++;
			if (o->is_a("TGLobject_fast_directionalcannon_left")) no++;
			if (o->is_a("TGLobject_fast_directionalcannon_right")) no++;
			if (o->is_a("TGLobject_fast_directionalcannon_up")) no++;
			if (o->is_a("TGLobject_fast_directionalcannon_down")) no++;
			if (o->is_a("TGLobject_tank")) no++;
			if (o->is_a("TGLobject_big_tank")) no++;
			if (o->is_a("TGLobject_leftdoor")) no++;
			if (o->is_a("TGLobject_rightdoor")) no++;
			if (o->is_a("TGLobject_button")) no++;
		} // while

		fprintf(fp,"%i\n",no);

        for(TGLobject *o:m_fg_objects) {
			if (o->is_a("TGLobject_ballstand")) fprintf(fp,"ball-stand %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_redlight")) fprintf(fp,"red-light %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_radar")) fprintf(fp,"radar %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_pipevscreen")) fprintf(fp,"pipe-vertical-screen %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_pipehscreen")) fprintf(fp,"pipe-horizontal-screen %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_laser_left")) fprintf(fp,"laser-left %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_laser_right")) fprintf(fp,"laser-right %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_laser_up")) fprintf(fp,"laser-up %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_laser_down")) fprintf(fp,"laser-down %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_left") && ((TGLobject_cannon_left *)o)->get_type()==0) fprintf(fp,"cannon-rock-left %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_right") && ((TGLobject_cannon_left *)o)->get_type()==0) fprintf(fp,"cannon-rock-right %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_down") && ((TGLobject_cannon_left *)o)->get_type()==0) fprintf(fp,"cannon-rock-down %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_up") && ((TGLobject_cannon_left *)o)->get_type()==0) fprintf(fp,"cannon-rock-up %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_left") && ((TGLobject_cannon_left *)o)->get_type()==1) fprintf(fp,"cannon-techno-left %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_right") && ((TGLobject_cannon_left *)o)->get_type()==1) fprintf(fp,"cannon-techno-right %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_down") && ((TGLobject_cannon_left *)o)->get_type()==1) fprintf(fp,"cannon-techno-down %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_cannon_up") && ((TGLobject_cannon_left *)o)->get_type()==1) fprintf(fp,"cannon-techno-up %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fastcannon_left")) fprintf(fp,"fastcannon-techno-left %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fastcannon_right")) fprintf(fp,"fastcannon-techno-right %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fastcannon_down")) fprintf(fp,"fastcannon-techno-down %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fastcannon_up")) fprintf(fp,"fastcannon-techno-up %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_spike_left")) fprintf(fp,"spike-left %i %i %i\n",o->get_animation_offset(),int(((TGLobject_spike_left *)o)->get_start_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_spike_right")) fprintf(fp,"spike-right %i %i %i\n",o->get_animation_offset(),int(((TGLobject_spike_left *)o)->get_start_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fuelrecharge")) fprintf(fp,"fuel-recharge %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_techno_computer")) fprintf(fp,"wall-techno-computer %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_directionalcannon_left")) fprintf(fp,"directional-cannon-left %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_directionalcannon_right")) fprintf(fp,"directional-cannon-right %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_directionalcannon_up")) fprintf(fp,"directional-cannon-up %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_directionalcannon_down")) fprintf(fp,"directional-cannon-down %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_ha_directionalcannon_left")) fprintf(fp,"ha-directional-cannon-left %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_ha_directionalcannon_right")) fprintf(fp,"ha-directional-cannon-right %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_ha_directionalcannon_up")) fprintf(fp,"ha-directional-cannon-up %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_ha_directionalcannon_down")) fprintf(fp,"ha-directional-cannon-down %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fast_directionalcannon_left")) fprintf(fp,"fast-directional-cannon-left %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fast_directionalcannon_right")) fprintf(fp,"fast-directional-cannon-right %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fast_directionalcannon_up")) fprintf(fp,"fast-directional-cannon-up %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_fast_directionalcannon_down")) fprintf(fp,"fast-directional-cannon-down %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));

			if (o->is_a("TGLobject_tank") && ((TGLobject_tank *)o)->get_type()==0) fprintf(fp,"grey-tank %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_tank") && ((TGLobject_tank *)o)->get_type()==1) fprintf(fp,"red-tank %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_tank") && ((TGLobject_tank *)o)->get_type()==2) fprintf(fp,"green-tank %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_big_tank")) fprintf(fp,"big-tank %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size));
			if (o->is_a("TGLobject_leftdoor")) fprintf(fp,"door-left %i %i %i %i %i\n",o->get_animation_offset(),int(((TGLobject_leftdoor *)o)->get_start_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_state()),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_rightdoor")) fprintf(fp,"door-right %i %i %i %i %i\n",o->get_animation_offset(),int(((TGLobject_leftdoor *)o)->get_start_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_state()),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==0) fprintf(fp,"button-red-left %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==1) fprintf(fp,"button-red-right %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==2) fprintf(fp,"button-red-up %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==3) fprintf(fp,"button-red-down %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==4) fprintf(fp,"button-purple-left %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==5) fprintf(fp,"button-purple-right %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==6) fprintf(fp,"button-purple-up %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==7) fprintf(fp,"button-purple-down %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==8) fprintf(fp,"button-blue-left %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==9) fprintf(fp,"button-blue-right %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==10) fprintf(fp,"button-blue-up %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
			if (o->is_a("TGLobject_button") && ((TGLobject_button *)o)->get_type()==11) fprintf(fp,"button-blue-down %i %i %i %i\n",o->get_animation_offset(),int(o->get_x()/m_fg_cell_size),int((o->get_y()-STARFIELD)/m_fg_cell_size),int(((TGLobject_leftdoor *)o)->get_action()));
		} // while
	
	}

} /* TGLmap::save */ 



