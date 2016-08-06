#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
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
#include "SDL/SDL_ttf.h"
#else
#include "SDL_ttf.h"
#endif

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
#include "TGLmap.h"
#include "TGL.h"
#include "TGLapp.h"
#include "TGLreplay.h"
#include "TGLinterface.h"

#include "LevelPack.h"
#include "PlayerProfile.h"


extern SDL_Window *appwindow;

int TGLapp::pregame_cycle(KEYBOARDSTATE *k)
{
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0) {

		if (m_game!=0) delete m_game;
		{
			char map_name[256];
			sprintf(map_name,TGL_PLAY_MAPS_FOLDER "/%s/%s",m_current_levelpack->m_id,m_current_levelpack->m_levels[m_selected_level]->m_map);
			m_game=new TGL(map_name,m_selected_ship,m_current_levelpack->m_levels[m_selected_level]->m_initial_fuel,m_player_profile->m_sfx_volume,m_player_profile->m_music_volume,m_GLTM);
			m_game->reset();
		}

		
		// SAVE REPLAY:
		m_game_replay_mode=1;
		if (m_game_replay!=0) delete m_game_replay;
		m_game_replay=new TGLreplay(m_game->get_map_name(), 0);
		m_game_replay->add_player(m_player_profile->m_name,m_selected_ship);

		m_game_state=0;
		m_game_state_cycle=0;


		TGLinterface::reset();
		SDL_WarpMouseInWindow(appwindow, 320,352);
		TGLinterface::add_element(new TGLbutton("Continue",m_font32,220,320,200,64,0));
	} // if 

	if (m_state_fading==1) {
		int mouse_x=0,mouse_y=0,button=0,button_status=0;
		int ID=-1;
		if (!m_mouse_click_x.empty()) {
            mouse_x = m_mouse_click_x.front();
            m_mouse_click_x.pop_front();
            mouse_y = m_mouse_click_y.front();
            m_mouse_click_y.pop_front();
            button=m_mouse_click_button.front();
            m_mouse_click_button.pop_front();
        } else {
			button_status=SDL_GetMouseState(&mouse_x,&mouse_y);
			button=0;
		} // if 

		if (k->key_press(SDL_SCANCODE_SPACE) || k->key_press(SDL_SCANCODE_RETURN)) button=1;

		ID=TGLinterface::update_state(mouse_x,mouse_y,button,button_status,k);

		if (ID==0) {
			m_state_fading=2;
			m_state_fading_cycle=0;
		} // if 
	}

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		SDL_ShowCursor(SDL_DISABLE);
		m_game_previous_state=TGL_STATE_POSTGAME;
		m_game_reinit_previous_state=true;
		return TGL_STATE_GAME;
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;

	return TGL_STATE_PREGAME;
} /* TGLapp::pregame_cycle */ 

void TGLapp::pregame_draw(void)
{
	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	char buffer[255];
	sprintf(buffer,"Level %i: %s",m_selected_level+1,m_current_levelpack->m_levels[m_selected_level]->m_name);
	TGLinterface::print_center(buffer,m_font32,320,176);
	{
		int i,j;
		float y;
		char *tmp=m_current_levelpack->m_levels[m_selected_level]->m_description;
		if (tmp!=0) {
			i=0;
			y=210;
			while(tmp[i]!=0) {
				for(j=0;tmp[i]!=0 && tmp[i]!='/';i++,j++) buffer[j]=tmp[i];
				buffer[j]=0;
				if (tmp[i]=='/') i++;
				TGLinterface::print_center(buffer,m_font16,320,y);
				y+=20;
			} // while

		} // if 
	}
	TGLinterface::draw();

	switch(m_state_fading) {
	case 0:	
			{
				float f=0;
				f=abs(int(25-m_state_fading_cycle))/25.0F;

				fade_in_alpha(f);
			}
			break;
	case 1:
			break;
	case 2:
			{
				float f=0;
				f=abs(int(m_state_fading_cycle))/25.0F;

				fade_in_alpha(f);
			}
			break;
	} // switch
} /* TGLapp::pregame_draw */ 
