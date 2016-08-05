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

int TGLapp::postgame_cycle(KEYBOARDSTATE *k)
{
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0) {
        // ship unlocking messages:
		TGLInterfaceElement *e;
		int i;
        int points_to_next_ship = 0;
        
        m_next_ship_to_unlock = -1;
		m_ship_unlocked=-1;
        
		TGLinterface::reset();
		if (m_game->get_game_result()!=1) SDL_WarpMouseInWindow(appwindow,210,352);
									 else SDL_WarpMouseInWindow(appwindow,430,352);
		TGLinterface::add_element(new TGLbutton("Retry",m_font32,110,320,200,64,0));
		e=new TGLbutton("Continue",m_font32,330,320,200,64,1);
		if (m_game->get_game_result()!=1) e->m_enabled=false;
		TGLinterface::add_element(e);
		TGLinterface::add_element(new TGLbutton("Back",m_font32,110,404,200,64,2));
		TGLinterface::add_element(new TGLbutton("Save Replay",m_font32,330,404,200,64,3));

		for(i=0;i<N_SHIPS && m_ship_unlocked==-1;i++) {
            if (!m_player_profile->has_ship(i) && m_player_profile->get_points()>=TGL::s_points_for_ship[i]) {
				m_ship_unlocked = i;
				m_player_profile->m_ships.push_back((m_ship_unlocked));
			} // if
            if (!m_player_profile->has_ship(i)) {
                int points_left = TGL::s_points_for_ship[i] - m_player_profile->get_points();
                if (m_next_ship_to_unlock==-1 || points_left<points_to_next_ship) {
                    m_next_ship_to_unlock = i;
                    points_to_next_ship = points_left;
                }
            }
		} // for
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

		if (ID!=-1) {
			m_state_fading=2;
			m_state_fading_cycle=0;
			m_state_selection=ID;
		} // if 
	}

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		switch(m_state_selection) {
		case 0:
				SDL_ShowCursor(SDL_DISABLE);
				return TGL_STATE_PREGAME;
				break;
		case 1:
				m_selected_level++;
				if (m_selected_level>=m_current_levelpack->m_levels.size()) {
					// ... level pack complete
					return TGL_STATE_MAINMENU;
				} else {
					return TGL_STATE_PREGAME;
				} // if 
				break;
		case 2:
				return TGL_STATE_LEVELPACKSCREEN;
				break;
		case 3:
				return TGL_STATE_SAVEREPLAY;
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;

	return TGL_STATE_POSTGAME;
} /* TGLapp::postgame_cycle */ 

void TGLapp::postgame_draw(void)
{
    if (m_state_cycle==0) return;
    
	char buffer[255];
	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	{
		int time=-1;
		int fuel_used = -1;
		int shots = -1;
		float max_speed = 0.0f;
		float average_speed = 0.0f;
		//int enemies_killed = -1;
		//float accuracy = 0.0f;

		if (m_game->get_game_result()==1) {
			TGLinterface::print_center("Congratulations!",m_font32,320,96);
			time=m_game_replay->get_length();
		} else {
			TGLinterface::print_center("Better luck next time...",m_font16,320,96);
		} // if 

		fuel_used = m_game_replay->get_fuel_used();
		shots = m_game_replay->get_n_shots();
		max_speed = m_game_replay->get_max_speed();
		average_speed = m_game_replay->get_average_speed();

		if (time!=-1) {
			int mins,secs,milis;
			milis=time*18;
			secs=milis/1000;
			milis=milis%1000;
			mins=secs/60;
			secs=secs%60;
			sprintf(buffer,"Time taken: %i:%.2i:%.2i",mins,secs,milis/10);
		} else {
			sprintf(buffer,"Time taken: --:--:--");
		} // if 
		TGLinterface::print_center(buffer,m_font16,320,125);

		sprintf(buffer,"Fuel used: %i    Shots: %i",fuel_used,shots);
		TGLinterface::print_center(buffer,m_font16,320,145);

		sprintf(buffer,"Max speed: %.2f    Average Speed: %.2f",max_speed,average_speed);
		TGLinterface::print_center(buffer,m_font16,320,165);

		sprintf(buffer,"Current score: %i",m_player_profile->get_points());
		TGLinterface::print_center(buffer,m_font16,320,200);
		
		if (m_ship_unlocked!=-1) {
            float f = float(0.5 + 0.5*cos(m_state_cycle*0.1f));
			sprintf(buffer,"New ship unlocked: '%s'!",TGL::ship_names[m_ship_unlocked]);
            TGLinterface::print_center(buffer,m_font16,320,225, 1, 1, 1, f);
        } else if (m_next_ship_to_unlock!=-1) {
            sprintf(buffer,"Points to next ship: %i", TGL::s_points_for_ship[m_next_ship_to_unlock] - m_player_profile->get_points());
            TGLinterface::print_center(buffer,m_font16,320,225);
        }
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
} /* TGLapp::postgame_draw */ 

