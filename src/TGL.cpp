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

#ifdef __EMSCRIPTEN__
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif

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
#include "TGLobject_ship_shield.h"
#include "TGLobject_ship.h"
#include "TGLobject_ship_vpanther.h"
#include "TGLobject_ship_xterminator.h"
#include "TGLobject_ship_shadowrunner.h"
#include "TGLobject_ship_nitroblaster.h"
#include "TGLobject_ship_vipperbeam.h"
#include "TGLobject_ship_dodger.h"
#include "TGLobject_ship_gravis.h"
#include "TGLobject_ship_accura.h"
#include "TGLobject_ship_gyrus.h"
#include "TGLobject_ship_dflecter.h"
#include "TGLobject_ship_chain.h"
#include "TGLobject_ship_grabber.h"
#include "TGLobject_ship_c_harpoon.h"
#include "TGLobject_ship_pulsar.h"
#include "TGLmap.h"
#include "TGL.h"
#include "TGLapp.h"
#include "TGLinterface.h"
#include "TGLreplay.h"


#include "debug.h"


#define MAPPAUSE_TIME	25


const char *TGL::ship_tiles[N_SHIPS]={
	"graphics/objects/ship-vpanther-1.png",
	"graphics/objects/ship-xterminator-1.png",
	"graphics/objects/ship-srunner-1.png",
	"graphics/objects/ship-nblaster-1.png",
	"graphics/objects/ship-vbeam-1.png",
	"graphics/objects/ship-dodger-1.png",
	"graphics/objects/ship-gravis-1.png",
	"graphics/objects/ship-accura-1.png",
	"graphics/objects/ship-gyrus.png",
	"graphics/objects/ship-dflecter-1.png",
	"graphics/objects/ship-harpoon.png",
	"graphics/objects/ship-pulsar-1.png",
};
const char *TGL::ship_names[N_SHIPS]={
	"V-Panther",
	"X-Terminator",
	"Shadow Runner",
	"Nitro Blaster",
	"Vipper Beam",
	"Dodger K7",
	"Gravis T8",
	"Accura T5",
	"Gyrus-P",
	"D-Flecter",
	"C-Harpoon",
	"Pulsar A3",
};

int TGL::s_points_for_ship[N_SHIPS]={0,0,0,5,150,25,75,125,40,55,15,100};


TGL::TGL(char *map, int ship,int initial_fuel,int sfx_volume, int music_volume, GLTManager *GLTM)
{	
	m_font=TTF_OpenFont("fonts/Krungthep.ttf",12);

	m_sfx_volume=sfx_volume;
	m_music_volume=music_volume;

	m_music_channel=-1;

	m_game_result=0;

	{
		FILE *fp;
		fp=fopen(map,"rb");
		if (fp!=0) {
			m_map=new TGLmap(fp,GLTM);
			fclose(fp);
		} else {
			m_map=0;
		} // if

		m_map_file=new char[strlen(map)+1];
		strcpy(m_map_file,map);
	}

	if (m_map!=0) {
		switch(ship) {
		case 0:	m_ship=new TGLobject_ship_vpanther(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 1:	m_ship=new TGLobject_ship_xterminator(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 2:	m_ship=new TGLobject_ship_shadowrunner(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 3:	m_ship=new TGLobject_ship_nitroblaster(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 4:	m_ship=new TGLobject_ship_vipperbeam(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 5:	m_ship=new TGLobject_ship_dodger(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 6:	m_ship=new TGLobject_ship_gravis(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 7:	m_ship=new TGLobject_ship_accura(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 8:	m_ship=new TGLobject_ship_gyrus(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 9:	m_ship=new TGLobject_ship_dflecter(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 10:m_ship=new TGLobject_ship_c_harpoon(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		case 11:m_ship=new TGLobject_ship_pulsar(float(m_map->get_dx()/2),32,initial_fuel);
				break;
		} // switch
		m_map->add_object(m_ship);
		m_focus_x=m_map->get_dx()/2;
		m_focus_y=32;
	} else {
		m_ship=0;
		m_focus_x=0;
		m_focus_y=0;
	} // if 

	m_cycle=0;

} /* TGL::TGL */ 


TGL::TGL(TGLreplay *replay,int sfx_volume, int music_volume, GLTManager *GLTM)
{
    m_font=TTF_OpenFont("fonts/Krungthep.ttf",12);
    
    m_sfx_volume=sfx_volume;
    m_music_volume=music_volume;
    
    m_music_channel=-1;
    
    m_game_result=0;
    
    {
        // load map:
        if (replay->get_map(GLTM)==0) {
            FILE *fp;
            fp=fopen(replay->get_map_name(),"rb");
            if (fp!=0) {
                m_map=new TGLmap(fp,GLTM);
                fclose(fp);
            } else {
                m_map=0;
            } // if
        } else {
            m_map = replay->get_map_copy(GLTM);
        }
        
        m_map_file=new char[strlen(replay->get_map_name())+1];
        strcpy(m_map_file,replay->get_map_name());
    }
    
    if (m_map!=0) {
        int ship = replay->get_playership(replay->get_playername(0));
        int initial_fuel = replay->get_initial_fuel();
        switch(ship) {
            case 0:	m_ship=new TGLobject_ship_vpanther(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 1:	m_ship=new TGLobject_ship_xterminator(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 2:	m_ship=new TGLobject_ship_shadowrunner(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 3:	m_ship=new TGLobject_ship_nitroblaster(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 4:	m_ship=new TGLobject_ship_vipperbeam(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 5:	m_ship=new TGLobject_ship_dodger(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 6:	m_ship=new TGLobject_ship_gravis(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 7:	m_ship=new TGLobject_ship_accura(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 8:	m_ship=new TGLobject_ship_gyrus(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 9:	m_ship=new TGLobject_ship_dflecter(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 10:m_ship=new TGLobject_ship_c_harpoon(float(m_map->get_dx()/2),32,initial_fuel);
                break;
            case 11:m_ship=new TGLobject_ship_pulsar(float(m_map->get_dx()/2),32,initial_fuel);
                break;
        } // switch
        m_map->add_object(m_ship);
        m_focus_x=m_map->get_dx()/2;
        m_focus_y=32;
    } else {
        m_ship=0;
        m_focus_x=0;
        m_focus_y=0;
    } // if 
    
    m_cycle=0;
}



TGL::~TGL()
{
    TTF_CloseFont(m_font);
    
    if (m_map_file!=0) delete []m_map_file;
	m_map_file=0;

	if (m_map!=0) delete m_map;
	m_map=0;

	if (m_music_channel!=-1) Mix_HaltChannel(m_music_channel);
} /* TGL::~TGL */ 


void TGL::reset(void)
{
	m_map->reset();
	m_ball = m_map->object_exists("TGLobject_ball");
} /* TGL::reset */ 


int TGL::get_game_result()
{
	return m_game_result;
} /* TGL::get_game_result */ 



char *TGL::get_map_name(void)
{
	return m_map_file;
} /* TGL::get_map_name */ 


TGLmap *TGL::get_map(void)
{
	return m_map;
} /* TGL::get_object_list */ 



bool TGL::editor_cycle(GLTManager *GLTM)
{
	if (m_map!=0) {
		m_map->editor_cycle(GLTM);
		m_cycle++;
	} // if 
	
	return true;
} /* TGL::editor_cycle */ 


bool TGL::cycle(std::list<VirtualController *> *lv,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume)
{
	if (m_cycle==0) {
		int sound=rand()%2;

		switch(sound) {
		case 0: Sound_play(SFXM->get(TGL_SFX_FOLDER "/start"),sfx_volume);
				break;
		case 1: Sound_play(SFXM->get(TGL_SFX_FOLDER "/start2"),sfx_volume);
				break;
		} // switch 
	} // if 


	if (m_map!=0) {
		m_map->cycle(lv,GLTM,SFXM,sfx_volume);
		m_cycle++;
	} // if 
	
	if (m_map->object_exists(m_ship)) {
		m_focus_x=int(m_ship->get_x());
		m_focus_y=int(m_ship->get_y());		
	} else {
		// player dead
		m_ship=0;
		m_game_result=2;
		return false;
	} // if 

	if (m_map->object_exists(m_ball) && 
		m_ball->get_y()<=0) {
		m_game_result=1;
		return false;
	} // if 

	return true;
} /* TGL::cycle */ 


void TGL::draw(float zoom, GLTManager *GLTM)
{
	// Draw the game:
	if (m_map!=0) m_map->draw(m_focus_x,m_focus_y,SCREEN_X,SCREEN_Y,GLTM, zoom, false);

	// Draw the score board:
	// Fuel:
	{
		int fuel=0*64;
		int n_bars;
		int i;

		TGLinterface::print_left("FUEL",m_font,8,21);
		if (m_ship!=0) fuel=m_ship->get_fuel();
		n_bars=fuel/128;

		for(i=0;i<n_bars;i++) {
            drawQuad(50+i*4,7,2,12,0,0,1,1);
		} // for

	}

	// Time:
	{
		char time_str[80];
		int seconds=0;
		int minutes=0;

		seconds=(m_cycle*18)/1000;
		minutes=seconds/60;
		seconds=seconds%60;
		sprintf(time_str,"%.2i:%.2i",minutes,seconds);

		TGLinterface::print_left(time_str,m_font,600,21);
	}

} /* TGL::draw */ 



void TGL::set_music_volume(int v)
{
	m_music_volume=v;
	if (m_music_channel!=-1) {
		Mix_Volume(m_music_channel,m_music_volume);
	} // if
} /* TGL::set_music_volume */ 


void TGL::set_sfx_volume(int v)
{
	m_sfx_volume=v;
} /* TGL::set_sfx_volume */ 


int TGL::get_cycle()
{
	return m_cycle;
} /* TGL::get_cycle */ 


