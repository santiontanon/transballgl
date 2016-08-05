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

int TGLapp::levelpackscreen_cycle(KEYBOARDSTATE *k)
{
	bool m_recheck_interface=false;
	bool m_reload_tutorial=false;
/*
    if (m_lp_music_channel == -1) {
        m_lp_music_channel = Sound_play_continuous(m_SFXM->get("levelpack"), m_player_profile->m_music_volume);
    } // if 
*/

	if (m_game!=0) {
		delete m_game;
		m_game=0;
	} // if 

	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0) {
		TGLInterfaceElement *e;
		m_selected_level=0;
		m_lp_replay_mode=0;
		m_lp_replay_timmer=0;

		if (m_lp_tutorial_game!=0) {
			delete m_lp_tutorial_game;
			m_lp_tutorial_game=0;
		} // if 
		if (m_lp_tutorial_replay!=0) {
			delete m_lp_tutorial_replay;
			m_lp_tutorial_replay=0;
		} // if 

		// Make sure that the current player has the selected ship unlocked:
		if (!contains<int>(m_player_profile->m_ships,m_selected_ship)) {
			m_selected_ship=(m_player_profile->m_ships.front());
		} // if 

		TGLinterface::reset();
		TGLinterface::add_element(new TGLbutton("Back",m_font32,10,10,180,60,0));
		e=new TGLbutton("Change Level Pack",m_font16,430,10,200,60,1);
		TGLinterface::add_element(e);

		TGLinterface::add_element(new TGLframe(20,120,330,340));
		TGLinterface::add_element(new TGLframe(370,120,250,340));
		TGLinterface::add_element(new TGLText("Select a level:",m_font16,180,105,true));
		TGLinterface::add_element(new TGLText(m_current_levelpack->m_name,m_font32,180,150,true));
		TGLinterface::add_element(new TGLText("Select a ship:",m_font16,500,105,true));

		// Add the levels to the inferface:
		{
			int i;
			LevelPack_Level *level;
			char tmp[128];

			m_lp_first_level=m_player_profile->progress_in_levelpack(m_current_levelpack->m_id)-1;
			if (m_lp_first_level+3>m_current_levelpack->m_levels.size()) m_lp_first_level=int(m_current_levelpack->m_levels.size()-3);
			if (m_lp_first_level<0) m_lp_first_level=0;

			for(i=0;i<3 && i<m_current_levelpack->m_levels.size();i++) {
				int time;

				level=m_current_levelpack->m_levels[i+m_lp_first_level];

				sprintf(tmp,"Level %i: %s",i+m_lp_first_level+1,level->m_name);
				m_lp_level_name[i]=new TGLText(tmp,m_font16,30,float(200+i*90),false);
				TGLinterface::add_element(m_lp_level_name[i]);

				time=m_player_profile->get_besttime(m_current_levelpack->m_id,i+m_lp_first_level,m_selected_ship);
				if (time==-1) {
					sprintf(tmp,"Best Time:: --:--:--");
				} else {
					int hunds=(time/10)%100;
					int secs=(time/1000)%60;
					int mins=time/60000;
					sprintf(tmp,"Best Time:: %i:%.2i:%.2i",mins,secs,hunds);
				} // if 
				m_lp_level_time[i]=new TGLText(tmp,m_font16,30,float(220+i*90),false);
				sprintf(tmp,"Points: %i [%i]",level->m_points,level->m_points*m_player_profile->number_of_times_completed(m_current_levelpack->m_id,i+m_lp_first_level));
				m_lp_level_points[i]=new TGLText(tmp,m_font16,30,float(240+i*90),false);
				TGLinterface::add_element(m_lp_level_time[i]);
				TGLinterface::add_element(m_lp_level_points[i]);
				m_lp_viewreplay_buttons[i]=new TGLbutton("View Replay",m_font16,180,float(210+i*90),120,20,10+i*2);
				TGLinterface::add_element(m_lp_viewreplay_buttons[i]);
				if (time==-1) m_lp_viewreplay_buttons[i]->m_enabled=false;
					     else m_lp_viewreplay_buttons[i]->m_enabled=true;
				m_lp_play_buttons[i]=new TGLbutton("Play",m_font16,180,float(235+i*90),120,20,11+i*2);
				TGLinterface::add_element(m_lp_play_buttons[i]);
				if (m_player_profile->progress_in_levelpack(m_current_levelpack->m_id)<i+m_lp_first_level) {
					m_lp_play_buttons[i]->m_enabled=false;
				} else {
					SDL_WarpMouseInWindow(appwindow, 240,245+i*90);
					m_lp_play_buttons[i]->m_enabled=true;
				} // if 
				
			} // for

			m_lp_level_uparrow=new TGLbutton(m_GLTM->get("graphics/interface/uparrow.png"),310,200,30,100,2);
			TGLinterface::add_element(m_lp_level_uparrow);
			if (m_lp_first_level==0) m_lp_level_uparrow->m_enabled=false;
			m_lp_level_downarrow=new TGLbutton(m_GLTM->get("graphics/interface/downarrow.png"),310,320,30,100,3);
			TGLinterface::add_element(m_lp_level_downarrow);
			if (m_lp_first_level+3>=m_current_levelpack->m_levels.size()) m_lp_level_downarrow->m_enabled=false;

		}

		// Add the ships to the interface:
		{
			m_lp_ship_leftarrow=new TGLbutton(m_GLTM->get("graphics/interface/leftarrow.png"),380,140,40,40,4);
			TGLinterface::add_element(m_lp_ship_leftarrow);
			if (indexOf(m_player_profile->m_ships, m_selected_ship)==0) m_lp_ship_leftarrow->m_enabled=false;
                                                                   else m_lp_ship_leftarrow->m_enabled=true;
			m_lp_ship_rightarrow=new TGLbutton(m_GLTM->get("graphics/interface/rightarrow.png"),570,140,40,40,5);
			TGLinterface::add_element(m_lp_ship_rightarrow);
            if (indexOf(m_player_profile->m_ships, m_selected_ship)==m_player_profile->m_ships.size()-1) {
                m_lp_ship_rightarrow->m_enabled=false;
            } else {
                m_lp_ship_rightarrow->m_enabled=true;
            }
		}

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

		if (m_lp_replay_mode==0) ID=TGLinterface::update_state(mouse_x,mouse_y,button,button_status,k);
					        else ID=-1;

		if (ID!=-1) {
			switch(ID) {
			case 0:
			case 1:
					m_state_fading=2;
					m_state_fading_cycle=0;
					m_state_selection=ID;
					break;
			case 2:
					// levels up
					{
						m_lp_first_level--;
						m_recheck_interface=true;
					}
					break;
			case 3:
					// levels down
					{
						m_lp_first_level++;
						m_recheck_interface=true;
					}
					break;
			case 4:
					{
						int pos=indexOf<int>(m_player_profile->m_ships, m_selected_ship);
						pos--;
						m_selected_ship=getAt<int>(m_player_profile->m_ships, pos);
						m_recheck_interface=true;
						m_reload_tutorial=true;
					}
					break;
			case 5:
					{
                        int pos=indexOf<int>(m_player_profile->m_ships, m_selected_ship);
                        pos++;
                        m_selected_ship=getAt<int>(m_player_profile->m_ships, pos);
                        m_recheck_interface=true;
						m_reload_tutorial=true;
					}
					break;
			case 10:
			case 12:
			case 14:
					// View replay:
					m_state_fading=2;
					m_state_fading_cycle=0;
					m_state_selection=ID;
					break;
			case 11:
			case 13:
			case 15:
					// Play:
					m_state_fading=2;
					m_state_fading_cycle=0;
					m_state_selection=ID;					
					break;
			} // switch
		} // if 
	}

	if (m_recheck_interface) {
		int i;
		LevelPack_Level *level;
		TGLInterfaceElement *old;
		char tmp[128];


		if (m_lp_first_level==0) m_lp_level_uparrow->m_enabled=false;
							else m_lp_level_uparrow->m_enabled=true;
		if (m_lp_first_level+3>=m_current_levelpack->m_levels.size()) m_lp_level_downarrow->m_enabled=false;
																   else m_lp_level_downarrow->m_enabled=true;

		for(i=0;i<3 && i<m_current_levelpack->m_levels.size();i++) {
			int time;
			level=m_current_levelpack->m_levels[i+m_lp_first_level];

			sprintf(tmp,"Level %i: %s",i+m_lp_first_level+1,level->m_name);
			old=m_lp_level_name[i];
			m_lp_level_name[i]=new TGLText(tmp,m_font16,30,float(200+i*90),false);
			TGLinterface::substitute_element(old,m_lp_level_name[i]);
			delete old;
			time=m_player_profile->get_besttime(m_current_levelpack->m_id,i+m_lp_first_level,m_selected_ship);
			if (time==-1) {
				sprintf(tmp,"Best Time:: --:--:--");
			} else {
				int hunds=(time/10)%100;
				int secs=(time/1000)%60;
				int mins=time/60000;
				sprintf(tmp,"Best Time:: %i:%.2i:%.2i",mins,secs,hunds);
			} // if 
			old=m_lp_level_time[i];
			m_lp_level_time[i]=new TGLText(tmp,m_font16,30,float(220+i*90),false);		
			TGLinterface::substitute_element(old,m_lp_level_time[i]);
			delete old;
	
			old=m_lp_level_points[i];
			sprintf(tmp,"Points: %i [%i]",level->m_points,level->m_points*m_player_profile->number_of_times_completed(m_current_levelpack->m_id,i+m_lp_first_level));
			m_lp_level_points[i]=new TGLText(tmp,m_font16,30,float(240+i*90),false);
			TGLinterface::substitute_element(old,m_lp_level_points[i]);
			delete old;

			if (time==-1) m_lp_viewreplay_buttons[i]->m_enabled=false;
				     else m_lp_viewreplay_buttons[i]->m_enabled=true;
			if (m_player_profile->progress_in_levelpack(m_current_levelpack->m_id)<i+m_lp_first_level) {
				m_lp_play_buttons[i]->m_enabled=false;
			} else {
				m_lp_play_buttons[i]->m_enabled=true;
			} // if 
		} // for

        int pos=indexOf<int>(m_player_profile->m_ships, m_selected_ship);
		if (pos==0) m_lp_ship_leftarrow->m_enabled=false;
			   else m_lp_ship_leftarrow->m_enabled=true;
		if (pos==m_player_profile->m_ships.size()-1) m_lp_ship_rightarrow->m_enabled=false;
												  else m_lp_ship_rightarrow->m_enabled=true;											
	} // if 

	if (m_reload_tutorial || m_lp_tutorial_game==0) {
		const char *ship_tutorial[]={"tutorial1-vp",
							   "tutorial1-xt",
							   "tutorial1-sr",
							   "tutorial1-nb",
							   "tutorial1-vb",
							   "tutorial1-dodg",
							   "tutorial1-gravis",
							   "tutorial1-acc",
							   "tutorial1-gyr",
							   "tutorial1-def",
							   "tutorial1-harp",
							   "tutorial1-pul",
								};

		if (m_lp_tutorial_game!=0) {
			delete m_lp_tutorial_game;
			m_lp_tutorial_game=0;
		} // if 
		if (m_lp_tutorial_replay!=0) {
			delete m_lp_tutorial_replay;
			m_lp_tutorial_replay=0;
		} // if 

		if (ship_tutorial[m_selected_ship]!=0) {
			char *replay_name=0;
			replay_name=new char[strlen(ship_tutorial[m_selected_ship])+15];
			sprintf(replay_name,"tutorials/%s.rpl",ship_tutorial[m_selected_ship]);

			FILE *fp=fopen(replay_name,"rb");
			if (fp!=0) {
				m_lp_tutorial_replay=new TGLreplay(fp, m_GLTM);
				fclose(fp);

                for(TextNode *tmp:m_lp_tutorial_replay_text) delete tmp;
				m_lp_tutorial_replay_text.clear();

				// Load the text messages:
				{
					char *replay_text_name;
					replay_text_name=new char[strlen(ship_tutorial[m_selected_ship])+15];
					sprintf(replay_text_name,"tutorials/%s.txt",ship_tutorial[m_selected_ship]);

					FILE *fp=fopen(replay_text_name,"rb");
					if (fp!=0) {
						int time;
						char line[256],*text,*tmp;

						fgets(line,255,fp);
						while(!feof(fp)) {
							if (1==sscanf(line,"%i",&time)) {
								TextNode *n;
								text = line;
								while((*text)!=' ' && (*text)!=0) text++;
								tmp = text;
								while((*tmp)!=0) {
									if ((*tmp)=='\n' || (*tmp)=='\r') *tmp=0;
									tmp++;
								} // while

								n = new TextNode();
								n->m_time = time;
								n->m_text = new char[strlen(text)+1];
								strcpy(n->m_text,text);
								m_lp_tutorial_replay_text.push_back(n);
							} // if

							fgets(line,255,fp);
						} // while 

						fclose(fp);
					} // if
					delete []replay_text_name;
				}

			} // if 
			delete []replay_name;
		} // if 
	
		
	} // if 


	if (m_lp_tutorial_replay!=0) {
		if (m_lp_tutorial_game==0) {
			m_lp_tutorial_game=new TGL(m_lp_tutorial_replay, 0, 0, m_GLTM);
			m_lp_tutorial_game->reset();
		} // if 

		std::list<TGLobject *> *l=m_lp_tutorial_game->get_map()->get_objects("TGLobject");
		std::list<TGLobject *> to_delete,to_add;
		TGLobject *o;
		bool retval;
		retval = m_lp_tutorial_replay->execute_cycle(&m_lvc,l,&to_delete,&to_add);
		l->clear();
		delete l;

		while(!to_delete.empty()) {
			o=to_delete.front();
            to_delete.pop_front();
			m_lp_tutorial_game->get_map()->remove_object(o);
			delete o;
		} // while
        to_delete.clear();

		while(!to_add.empty()) {
			o=to_add.front();
            to_add.pop_front();
			m_lp_tutorial_game->get_map()->add_object(o);
			//delete o;
		} // while
        to_add.clear();

		if (m_lp_replay_mode==2) {
			if (!m_lp_tutorial_game->cycle(&m_lvc,m_GLTM,m_SFXM,m_player_profile->m_sfx_volume)) retval=false;
		} else {
			if (!m_lp_tutorial_game->cycle(&m_lvc,m_GLTM,m_SFXM,0)) retval=false;
		} // if
		
		if (!retval) {
			delete m_lp_tutorial_game;
			m_lp_tutorial_game=0;
			delete m_lp_tutorial_replay;
			m_lp_tutorial_replay=0;
		} // if  
	} // if 

	if (k->key_press(SDL_SCANCODE_F)) {
		if (m_lp_replay_mode==0) {
			m_lp_replay_mode=1;
			m_lp_replay_timmer=0;
		} else if (m_lp_replay_mode==2) {
			m_lp_replay_mode=3;
			m_lp_replay_timmer=0;
		} // if 
	} // if 

	if (m_lp_replay_mode==1) {
		m_lp_replay_timmer++;
		if (m_lp_replay_timmer>=25) {
			m_lp_replay_mode=2;
			m_lp_replay_timmer=0;
		} // if 
	} // if 
	if (m_lp_replay_mode==3) {
		m_lp_replay_timmer++;
		if (m_lp_replay_timmer>=25) {
			m_lp_replay_mode=0;
			m_lp_replay_timmer=0;
		} // if 
	} // if 

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		SDL_ShowCursor(SDL_DISABLE);
		switch(m_state_selection) {
		case 0: if (m_lp_music_channel!=-1) Mix_HaltChannel(m_lp_music_channel);
				m_lp_music_channel=-1;
				return TGL_STATE_MAINMENU;
				break;
		case 1: return TGL_STATE_LEVELPACKBROWSER;
				break;
		case 10:
				// View replay:
				{
					// m_lp_first_level
					FILE *fp;
					char tmp[256];
					sprintf(tmp,"%splayers/%s/%s-level-%i-%i.rpl",
								m_player_data_path,
								m_player_profile->m_name,m_current_levelpack->m_id,m_lp_first_level,
								m_selected_ship);
					fp=fopen(tmp,"rb");
					if (fp!=0) {
						m_game_replay_mode=2;

						m_game_replay=new TGLreplay(fp, m_GLTM);
						fclose(fp);

						m_game=new TGL(m_game_replay,m_player_profile->m_sfx_volume,m_player_profile->m_music_volume,m_GLTM);
						m_game->reset();
						
						m_game_state=0;
						m_game_state_cycle=0;
						
						m_game_previous_state=TGL_STATE_LEVELPACKSCREEN;
						m_game_reinit_previous_state=false;
						return TGL_STATE_GAME;
					}
				}
				break;
		case 12:
				// View replay:
				{
					// m_lp_first_level
					FILE *fp;
					char tmp[256];
					sprintf(tmp,"%splayers/%s/%s-level-%i-%i.rpl",
								m_player_data_path,
								m_player_profile->m_name,m_current_levelpack->m_id,m_lp_first_level+1,
								m_selected_ship);
					fp=fopen(tmp,"rb");
					if (fp!=0) {
						m_game_replay_mode=2;

						m_game_replay=new TGLreplay(fp, m_GLTM);
						fclose(fp);

						m_game=new TGL(m_game_replay,m_player_profile->m_sfx_volume,m_player_profile->m_music_volume,m_GLTM);
						m_game->reset();
						
						m_game_state=0;
						m_game_state_cycle=0;
						
						m_game_previous_state=TGL_STATE_LEVELPACKSCREEN;
						m_game_reinit_previous_state=false;
						return TGL_STATE_GAME;
					}
				}
				break;
		case 14:
				// View replay:
				{
					// m_lp_first_level
					{
						FILE *fp;
						char tmp[256];
						sprintf(tmp,"%splayers/%s/%s-level-%i-%i.rpl",
									m_player_data_path,
									m_player_profile->m_name,m_current_levelpack->m_id,m_lp_first_level+2,
									m_selected_ship);
						fp=fopen(tmp,"rb");
						if (fp!=0) {
							m_game_replay_mode=2;

							m_game_replay=new TGLreplay(fp, m_GLTM);
							fclose(fp);

							m_game=new TGL(m_game_replay,m_player_profile->m_sfx_volume,m_player_profile->m_music_volume,m_GLTM);
							m_game->reset();
							
							m_game_state=0;
							m_game_state_cycle=0;
							
							m_game_previous_state=TGL_STATE_LEVELPACKSCREEN;
							m_game_reinit_previous_state=false;
							return TGL_STATE_GAME;
						}
					}
				}
				break;
		case 11:
				// Play:
				m_selected_level = m_lp_first_level;
				if (m_lp_music_channel!=-1) Mix_HaltChannel(m_lp_music_channel);
				m_lp_music_channel=-1;
				return TGL_STATE_PREGAME;					
		case 13:
				// Play:
				m_selected_level = m_lp_first_level + 1;
				if (m_lp_music_channel!=-1) Mix_HaltChannel(m_lp_music_channel);
				m_lp_music_channel=-1;
				return TGL_STATE_PREGAME;					
		case 15:
				// Play:
				m_selected_level = m_lp_first_level + 2;
				if (m_lp_music_channel!=-1) Mix_HaltChannel(m_lp_music_channel);
				m_lp_music_channel=-1;
				return TGL_STATE_PREGAME;					
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;

	if (m_state_fading==0 && m_lp_music_channel!=-1) Mix_Volume(m_lp_music_channel, int(m_player_profile->m_music_volume*(m_state_fading_cycle/25.0f)));
	if (m_state_fading==1 && m_lp_music_channel!=-1) Mix_Volume(m_lp_music_channel, m_player_profile->m_music_volume);
	if (m_state_fading==2 && m_lp_music_channel!=-1) Mix_Volume(m_lp_music_channel, int(m_player_profile->m_music_volume*((25-m_state_fading_cycle)/25.0f)));

	return TGL_STATE_LEVELPACKSCREEN;
} /* TGLapp::levelpackscreen_cycle */ 

void TGLapp::levelpackscreen_draw(void)
{
	char buffer[256];
	float replay_full_factor=0;

	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	TGLinterface::draw();

	// Draw scores:
	sprintf(buffer,"Total Points: %i",m_player_profile->get_points());
	TGLinterface::print_left(buffer,m_font16,230,40);
	sprintf(buffer,"Level Pack Points: %i",m_player_profile->get_points(m_current_levelpack->m_id));
	TGLinterface::print_left(buffer,m_font16,230,60);

	
	// Draw Selected ship:
	{
		int i,s;
		GLTile *t;

		for(i=-1;i<2;i++) {
			s= indexOf<int>(m_player_profile->m_ships, m_selected_ship)+i;
			if (s>=0 && s<m_player_profile->m_ships.size()) {
				t=m_GLTM->get(TGL::ship_tiles[getAt<int>(m_player_profile->m_ships, s)]);

				if (i==0) {
					t->draw(495,160,0,0,1);
				} else {
					t->draw(1,1,1,0.33f,float(495+i*48),160,0,0,0.66f);
				} // if 

			} // if
		} // for

		TGLinterface::print_center(TGL::ship_names[m_selected_ship],m_font32,495,220);
	}

	switch(m_lp_replay_mode) {
	case 0:
			replay_full_factor=0;
			break;
	case 1:
			{
				float f=0;
				f=abs(m_lp_replay_timmer)/25.0F;
				fade_in_alpha(f);
				replay_full_factor=f;
			}
			break;
	case 2: fade_in_alpha(1);
			replay_full_factor=1;
			break;
	case 3:
			{
				float f=0;
				f=abs(25-m_lp_replay_timmer)/25.0F;
				fade_in_alpha(f);
				replay_full_factor=f;
			}
			break;
	} // switch

	// Draw tutorial:
	{
		if (m_lp_tutorial_game!=0) {
			int old[4];

			glGetIntegerv(GL_VIEWPORT,old);
			glViewport(int(380*(1-replay_full_factor)),int(80*(1-replay_full_factor)), 
					   int(SCREEN_X*replay_full_factor+230*(1-replay_full_factor)),int(SCREEN_Y*replay_full_factor+172*(1-replay_full_factor)));
            glScissor(int(380*(1-replay_full_factor)),int(80*(1-replay_full_factor)),
                      int(SCREEN_X*replay_full_factor+230*(1-replay_full_factor)),int(SCREEN_Y*replay_full_factor+172*(1-replay_full_factor)));
            glEnable(GL_SCISSOR_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_lp_tutorial_game->draw(1.0, m_GLTM);
            glDisable(GL_SCISSOR_TEST);
			glViewport(old[0],old[1],old[2],old[3]);
            
			{
				float f=0.6f+0.4f*float(sin(m_state_cycle*0.1));

				if (m_lp_replay_mode==0 || m_lp_replay_mode==1) TGLinterface::print_center("Press F to maximize",m_font16,320*replay_full_factor+495*(1-replay_full_factor),20*replay_full_factor+250*(1-replay_full_factor),1,1,1,f);
				if (m_lp_replay_mode==2 || m_lp_replay_mode==3) TGLinterface::print_center("Press F to minimize",m_font16,320*replay_full_factor+495*(1-replay_full_factor),20*replay_full_factor+250*(1-replay_full_factor),1,1,1,f);
			}

			{
				int i,j;
				float y;
				char buffer[128];
				char *tmp=0;

				/* Text messages in replays */ 
				{
                    for(TextNode *n:m_lp_tutorial_replay_text) {
						if (n->m_time<m_lp_tutorial_game->get_cycle()) tmp = n->m_text;
					} // while 
				}

				if (tmp!=0) {
					i=0;
					y=445*replay_full_factor+425*(1-replay_full_factor);
					while(tmp[i]!=0) {
						for(j=0;tmp[i]!=0 && tmp[i]!='/';i++,j++) buffer[j]=tmp[i];
						buffer[j]=0;
						if (tmp[i]=='/') i++;
						TGLinterface::print_center(buffer,m_font16,320*replay_full_factor+495*(1-replay_full_factor),y);
						y+=20;
					} // while

				} // if 
			} 
		} else {
			TGLinterface::print_center("No tutorial available",m_font16,320*replay_full_factor+495*(1-replay_full_factor),240*replay_full_factor+300*(1-replay_full_factor));
		} // if 
	} 

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
} /* TGLapp::levelpackscreen_draw */ 

