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

#include "LevelPack.h"
#include "PlayerProfile.h"



int TGLapp::game_cycle(KEYBOARDSTATE *k)
{
	int i;
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_DISABLE) SDL_ShowCursor(SDL_DISABLE);

    if (m_state_cycle==0) m_game_fade_effect=rand()%3;

	switch(m_game_replay_mode) {
	case 1: // SAVING REPLAY
			i=0;
            for(VirtualController *m_vc:m_lvc) {
				m_vc->new_cycle();
                if (i<m_player_profile->m_n_players) {
                    if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_THRUST]]) m_vc->m_joystick[VC_UP]=true;
                                                                     else m_vc->m_joystick[VC_UP]=false;
                    if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_SPECIAL]]) m_vc->m_joystick[VC_DOWN]=true;
                                                                      else m_vc->m_joystick[VC_DOWN]=false;
                    if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_LEFT]]) m_vc->m_joystick[VC_LEFT]=true;
                                                                   else m_vc->m_joystick[VC_LEFT]=false;
                    if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_RIGHT]]) m_vc->m_joystick[VC_RIGHT]=true;
                                                                    else m_vc->m_joystick[VC_RIGHT]=false;
                    if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_FIRE]]) m_vc->m_button[0]=true;
                                                                   else m_vc->m_button[0]=false;
                    if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_ATTRACTOR]]) m_vc->m_button[1]=true;
                                                                        else m_vc->m_button[1]=false;
                    if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_MENU]]) m_vc->m_menu=true;
                                                                                     else m_vc->m_menu=false;
                }
				i++;
			} // while 

			{
				std::list<TGLobject *> *l=m_game->get_map()->get_objects("TGLobject");
				m_game_replay->store_cycle(&m_lvc,l);
                l->clear();
				delete l;
			}
			break;
	case 2: // REPLAYING REPLAY
    case 4: // REPLAYING REPLAY (waiting for ship explosion)
        {
            if (!m_game_replay->is_over()) {
				std::list<TGLobject *> *l=m_game->get_map()->get_objects("TGLobject");
				std::list<TGLobject *> to_delete,to_add;
				TGLobject *o;
				bool retval;
				retval = m_game_replay->execute_cycle(&m_lvc,l,&to_delete,&to_add);
				l->clear();
				delete l;

//                printf("add: %i, delete: %i\n", (int)to_add.size(), (int)to_delete.size());
				while(!to_delete.empty()) {
					o=to_delete.front();
                    to_delete.remove(o);
//                    printf("  %p\n",o);
					m_game->get_map()->remove_object(o);
					delete o;
				} // while
                to_delete.clear();
                
				while(!to_add.empty()) {
					o=to_add.front();
                    to_add.remove(o);
                    m_game->get_map()->add_object(o);
//					delete o;
				} // while
                to_add.clear();

				if (!retval) {
					m_game_state=4;
					m_game_state_cycle=0;
				} // if  
				i=0;
                for(VirtualController *m_vc:m_lvc) {
					if (k->keyboard[m_player_profile->m_keys_configuration[i][KEY_MENU]]) m_vc->m_menu=true;
                                                                                     else m_vc->m_menu=false;
				} // while
            }
        }
        break;
	} // switch 
			

	switch(m_game_state) {
	case 0:	// Appearing
			m_game_state_cycle++;
			if (m_game_state_cycle>50) {
				m_game_state=1;
				m_game_state_cycle=0;
			} // if 
			
			if (!m_game->cycle(&m_lvc,m_GLTM,m_SFXM,m_player_profile->m_sfx_volume)) {
				m_game_state=2;
				m_game_state_cycle=0;
			} // if 
			
			break;
	case 1:	// playing
            {
                for(VirtualController *m_vc:m_lvc) {
                    if (m_vc->m_menu && !m_vc->m_old_menu) {
                        if (m_game_replay_mode==1) {
                            m_configure_previous_state = m_state;
                            return TGL_STATE_CONFIGURE;
                        } else {
                            m_game_state = 3;
                        }
                    }
                } // while
                
                if (!m_game->cycle(&m_lvc,m_GLTM,m_SFXM,m_player_profile->m_sfx_volume)) {
                    if (m_game->get_game_result()==2) m_game_state=2;
                                                 else m_game_state=3;
                    m_game_state_cycle=0;
                } // if 
            }
			break;
	case 2:	// Just waiting some time for showing the ship explosion
			m_game->cycle(&m_lvc,m_GLTM,m_SFXM,m_player_profile->m_sfx_volume);
			m_game_state_cycle++;
			if (m_game_state_cycle>100) {
				m_game_state=4;
				m_game_state_cycle=0;
			} // if 
			break;
	case 3:	// Disappearing (playing)
    case 4:	// Disappearing (replay)
            m_game->cycle(&m_lvc,m_GLTM,m_SFXM,m_player_profile->m_sfx_volume);
			m_game_state_cycle++;
			if (m_game_state_cycle>50) {
				if (m_game_replay_mode==1 && m_game->get_game_result()==1) {
					// Update player profile!
					m_player_profile->level_completed(m_player_data_path,m_current_levelpack->m_id,m_selected_level,m_game_replay,m_current_levelpack);
					save_playerprofile();
				} // if 
				return m_game_previous_state;
			} // if 
			break;
	} // switch


	return TGL_STATE_GAME;
} /* TGLapp::game_cycle */ 


void TGLapp::game_draw(void)
{
	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	switch(m_game_state) {
	case 0:	
			{
				float f=0;
				f=abs(int(50-m_game_state_cycle))/50.0F;

				m_game->draw(m_player_profile->m_zoom, m_GLTM);

                if (m_game_fade_effect==-1) m_game_fade_effect=rand()%3;
				switch(m_game_fade_effect) {
				case 0: fade_in_alpha(f);
						break;
				case 1: fade_in_squares(f,64);
						break;
				case 2: fade_in_triangles(f,32);
						break;
				} // switch
			}
			break;
	case 1:
	case 2:
			m_game_fade_effect=-1;
			m_game->draw(m_player_profile->m_zoom, m_GLTM);
			break;
	case 3:
    case 4:
			{
				float f=0;
				f=abs(int(m_game_state_cycle))/50.0F;

				m_game->draw(m_player_profile->m_zoom, m_GLTM);

                if (m_game_fade_effect==-1) m_game_fade_effect=rand()%3;
				switch(m_game_fade_effect) {
				case 0: fade_in_alpha(f);
						break;
				case 1: fade_in_squares(f,64);
						break;
				case 2: fade_in_triangles(f,32);
						break;
				} // switch
			}
			break;
	} // switch

} /* TGLapp::game_draw */ 

