#ifdef _WIN32
#include "windows.h"
#else
#include <stddef.h>
#include <sys/types.h>
#include <dirent.h>
#include "ctype.h"
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
#include "TGLobject_ship.h"
#include "TGLmap.h"
#include "TGL.h"
#include "TGLapp.h"
#include "TGLreplay.h"
#include "TGLinterface.h"

#include "LevelPack.h"
#include "PlayerProfile.h"


extern SDL_Window *appwindow;

int TGLapp::replaybrowser_cycle(KEYBOARDSTATE *k)
{
	bool check_for_replays_to_load=false;
	bool name_changed=false;

	if (m_game!=0) {
		delete m_game;
		m_game=0;
	} // if 

	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0) {

		m_rb_mouse_over_replay=-1;
		m_rb_replay_selected=-1;

		TGLinterface::reset();
		SDL_WarpMouseInWindow(appwindow,210,352);
		TGLinterface::add_element(new TGLframe(10,32,580,276));		
		TGLinterface::add_element(new TGLbutton("Back",m_font32,40,404,160,64,1));
		m_replay_play_button=new TGLbutton("View",m_font32,240,404,160,64,5);
		TGLinterface::add_element(m_replay_play_button);		
		m_replay_play_button->m_enabled=false;
		m_replay_rename_button=new TGLbutton("Rename",m_font16,430,340,200,32,0);
		TGLinterface::add_element(m_replay_rename_button);		
		m_replay_rename_button->m_enabled=false;
		m_replay_delete_button=new TGLbutton("Delete",m_font16,430,380,200,32,4);
		TGLinterface::add_element(m_replay_delete_button);		
		m_replay_delete_button->m_enabled=false;

		m_replay_name_inputframe=new TGLTextInputFrame("",32,m_font16,10,340,400,32,0);
		TGLinterface::add_element(m_replay_name_inputframe);
		m_replay_name_inputframe->m_focus=false;
		m_replay_name_inputframe->m_enabled=false;

		// Load the replay filenames:
		{
			char *tmp;
			int i;

            m_sr_first_replay=0;
            for(char *tmp:m_sr_replay_fullnames) delete []tmp;
            m_sr_replay_fullnames.clear();
            
            for(char *tmp:m_sr_replay_names) delete []tmp;
            m_sr_replay_names.clear();
            
            for(char *tmp:m_sr_replay_info) delete []tmp;
            m_sr_replay_info.clear();


#ifdef _WIN32
			/* Find files: */
			WIN32_FIND_DATA finfo;
			HANDLE h;

			h = FindFirstFile("replays/*.rpl", &finfo);
			if (h != INVALID_HANDLE_VALUE) {
				tmp=new char[strlen(finfo.cFileName)+1];
				strcpy(tmp,finfo.cFileName);
				m_sr_replay_names.push_back(tmp);

				while (FindNextFile(h, &finfo) == TRUE) {
					tmp=new char[strlen(finfo.cFileName)+1];
					strcpy(tmp,finfo.cFileName);
					m_sr_replay_names.push_back(tmp);
				} /* while */
			} /* if */
#else
			DIR *dp;
			struct dirent *ep;
			char dname[256];
			
			sprintf(dname,"%sreplays",m_player_data_path);

			dp = opendir (dname);
			if (dp != NULL) {
				while ((ep = readdir (dp))) {
					if (strlen(ep->d_name) > 4 &&
							ep->d_name[strlen(ep->d_name) - 4] == '.' &&
							ep->d_name[strlen(ep->d_name) - 3] == 'r' &&
							ep->d_name[strlen(ep->d_name) - 2] == 'p' &&
							ep->d_name[strlen(ep->d_name) - 1] == 'l') {

						tmp=new char[strlen(ep->d_name)+1];
						strcpy(tmp,ep->d_name);
						m_sr_replay_names.push_back(tmp);
					} /* if */

				} /* while */
				(void) closedir (dp);
			} /* if */
#endif

			for(i=0;i<m_sr_replay_names.size();i++) {
				char *tmp;

				tmp=new char[strlen(m_player_data_path) + strlen(m_sr_replay_names[i]) + 9 + 1];
				sprintf(tmp,"%sreplays/%s",m_player_data_path,m_sr_replay_names[i]);
				m_sr_replay_fullnames.push_back(tmp);
				m_sr_replay_info.push_back(0);
			} // for
			check_for_replays_to_load=true;
		}

		m_sr_replay_uparrow=new TGLbutton(m_GLTM->get("graphics/interface/uparrow.png"),600,32,30,100,2);
		TGLinterface::add_element(m_sr_replay_uparrow);
		if (m_sr_first_replay==0) m_sr_replay_uparrow->m_enabled=false;
							 else m_sr_replay_uparrow->m_enabled=true;
		m_sr_replay_downarrow=new TGLbutton(m_GLTM->get("graphics/interface/downarrow.png"),600,100+32+76,30,100,3);
		TGLinterface::add_element(m_sr_replay_downarrow);
		if (m_sr_replay_names.size()>SAVEREPLAY_REPLAYSPERPAGE) m_sr_replay_downarrow->m_enabled=true;
														     else m_sr_replay_downarrow->m_enabled=false;

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
			// Rename
			char tmp[256],*cp1,*cp2,*tmp2;

			cp1 = m_sr_replay_fullnames[m_rb_replay_selected];
			cp2 = m_sr_replay_names[m_rb_replay_selected];

			sprintf(tmp,"%sreplays/%s",m_player_data_path,m_replay_name_inputframe->m_editing);
			tmp2=new char[strlen(tmp)+1];
			strcpy(tmp2,tmp);
			
			rename(cp1,tmp);
			delete cp1;

			m_sr_replay_fullnames[m_rb_replay_selected] = tmp2;
			sprintf(tmp,"%s",m_replay_name_inputframe->m_editing);
			tmp2=new char[strlen(tmp)+1];
			strcpy(tmp2,tmp);
			delete cp2;
			m_sr_replay_names[m_rb_replay_selected] = tmp2;


		} // if 

		if (ID==1 || ID==5) {
			m_state_fading=2;
			m_state_fading_cycle=0;
			m_state_selection=ID;
		} // if 

		if (ID==2) {
			m_sr_first_replay--;
			if (m_sr_first_replay==0) m_sr_replay_uparrow->m_enabled=false;
								 else m_sr_replay_uparrow->m_enabled=true;
			if (m_sr_replay_names.size()>m_sr_first_replay+SAVEREPLAY_REPLAYSPERPAGE) m_sr_replay_downarrow->m_enabled=true;
																			  	   else m_sr_replay_downarrow->m_enabled=false;	
			check_for_replays_to_load=true;
		} // if 

		if (ID==3) {
			m_sr_first_replay++;
			if (m_sr_first_replay==0) m_sr_replay_uparrow->m_enabled=false;
								 else m_sr_replay_uparrow->m_enabled=true;
			if (m_sr_replay_names.size()>m_sr_first_replay+SAVEREPLAY_REPLAYSPERPAGE) m_sr_replay_downarrow->m_enabled=true;
																			  	   else m_sr_replay_downarrow->m_enabled=false;
			check_for_replays_to_load=true;
		} // if 

		if (ID==4) {
			// Delete
			char tmp[256],*cp1,*cp2,*cp3;
            
			cp1 = m_sr_replay_fullnames[m_rb_replay_selected];
			cp2 = m_sr_replay_names[m_rb_replay_selected];
			cp3 = m_sr_replay_info[m_rb_replay_selected];

            m_sr_replay_fullnames.erase(m_sr_replay_fullnames.begin()+m_rb_replay_selected);
            m_sr_replay_names.erase(m_sr_replay_names.begin()+m_rb_replay_selected);
            m_sr_replay_info.erase(m_sr_replay_info.begin()+m_rb_replay_selected);
            
			sprintf(tmp,"%sreplays/%s",m_player_data_path,cp2);
			remove(tmp);

			check_for_replays_to_load=true;

			delete []cp1;
			delete []cp2;
			delete []cp3;

			m_rb_replay_selected=-1;

			if (m_sr_first_replay>=m_sr_replay_names.size() && m_sr_first_replay>0) m_sr_first_replay--;

			m_replay_name_inputframe->m_editing[0]=0;
			m_replay_name_inputframe->m_editing_position=0;
			m_replay_name_inputframe->m_enabled=false;
			name_changed=true;

		} // if 

		// Check to see if the mouse is over a replay:
		m_rb_mouse_over_replay=-1;
		if (mouse_x>=20 && mouse_x<600) {
			if (mouse_y>=40) {
				int selected=(mouse_y-40)/22;

				if (selected>=0 && selected<(m_sr_replay_names.size()-m_sr_first_replay) && selected<SAVEREPLAY_REPLAYSPERPAGE) {
					m_rb_mouse_over_replay=selected;

					if (button==1) {
						if (m_rb_replay_selected == m_sr_first_replay+selected) {
							m_state_fading=2;
							m_state_fading_cycle=0;
							m_state_selection=5;
						} else {
							m_rb_replay_selected=m_sr_first_replay+selected;

							strcpy(m_replay_name_inputframe->m_editing, m_sr_replay_names[m_rb_replay_selected]);
							m_replay_name_inputframe->m_editing_position=int(strlen(m_replay_name_inputframe->m_editing));
							m_replay_name_inputframe->m_enabled=true;

							name_changed=true;
						} // if
					} // if 
				} // if 
			} // if
		} // if 

		if (m_replay_name_inputframe->m_change_in_last_cycle) name_changed=true;

		// Check if the name is a valid file to rename:
		if (name_changed) {
			FILE *fp;
			bool valid_replay_name=true;
			char tmp[256];

			sprintf(tmp,"%sreplays/%s",m_player_data_path,m_replay_name_inputframe->m_editing);

            fp=fopen(tmp,"rb");
            if (fp!=0) {
                valid_replay_name=false;
                fclose(fp);
            } /* if */

			if (valid_replay_name) {
				fp=fopen(tmp,"wb");
				if (fp!=0) {					
					fclose(fp);
					remove(tmp);
				} else {
					valid_replay_name=false;
				} /* if */
			} // if 

			if (valid_replay_name) m_replay_rename_button->m_enabled=true;
							  else m_replay_rename_button->m_enabled=false;
		} // if 

		// Check if the name is a valid file to delete:
		if (name_changed) {
			FILE *fp;
			bool valid_to_delete=false;
			char tmp[256];

			sprintf(tmp,"%sreplays/%s",m_player_data_path,m_replay_name_inputframe->m_editing);

            fp=fopen(tmp,"rb");
            if (fp!=0) {
                valid_to_delete=true;
                fclose(fp);
            } /* if */


			if (valid_to_delete) m_replay_delete_button->m_enabled=true;
							else m_replay_delete_button->m_enabled=false;

		} // if 

	}

	if (m_rb_replay_selected==-1) m_replay_play_button->m_enabled=false;
							 else m_replay_play_button->m_enabled=true;

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		switch(m_state_selection) {
		case 1:
				return TGL_STATE_MAINMENU;
				break;
		case 5:
				// View replay:
				{

					m_game_replay_mode=2;
					{
						FILE *fp;
						char tmp[512];
						sprintf(tmp,"%sreplays/%s",m_player_data_path,m_sr_replay_names[m_rb_replay_selected]);
						fp=fopen(tmp,"rb");
                        if (m_game_replay!=0) delete m_game_replay;
						m_game_replay=new TGLreplay(fp, m_GLTM);
						fclose(fp);
					}
                    if (m_game!=0) delete m_game;
					m_game=new TGL(m_game_replay, m_player_profile->m_sfx_volume, m_player_profile->m_music_volume, m_GLTM);
					m_game->reset();

                    
                    /*
                    // this is just temporary code that embeds a map into a replay and saves it again
                    {
                        FILE *fp;
                        char tmp[512];
                        sprintf(tmp,"%sreplays/%s",m_player_data_path,m_sr_replay_names[m_rb_replay_selected]);
                        fp=fopen(tmp,"rb");
                        TGLreplay *game_replay=new TGLreplay(fp, m_GLTM);
                        fclose(fp);
                        
                        while(game_replay->read_one_cycle());

                        game_replay->set_map(m_game->get_map());
                        sprintf(tmp,"%sreplays/%s.embedded",m_player_data_path,m_sr_replay_names[m_rb_replay_selected]);
                        FILE *fp2=fopen(tmp,"wb");
                        game_replay->save(fp2, true, true);
                        game_replay->set_map(0);
                        fclose(fp2);
                    }
                    */
                    

					m_game_state=0;
					m_game_state_cycle=0;

					m_game_previous_state=TGL_STATE_REPLAYBROWSER;
					m_game_reinit_previous_state=false;
					return TGL_STATE_GAME;
				}
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;


	if (check_for_replays_to_load) {
		int i,j;
		char *info,*name;

		for(j=0,i=m_sr_first_replay;i<m_sr_first_replay+SAVEREPLAY_REPLAYSPERPAGE && i<m_sr_replay_names.size();i++,j++) {
			name = m_sr_replay_names[i];
			info = m_sr_replay_info[i];

			if (info==0) {
				FILE *fp=fopen(m_sr_replay_fullnames[i],"rb");
				if (fp!=0) {
					TGLreplay *rpl=new TGLreplay(fp, m_GLTM);
					fclose(fp);
					
					char *tmp=new char[256];
					{
						int milis=rpl->get_length()*18;
						int hunds=(milis/10)%100;
						int secs=(milis/1000)%60;
						int mins=(milis/60000);

						sprintf(tmp,"%s - %i:%.2i:%.2i",TGL::ship_names[rpl->get_playership(rpl->get_playername(0))],mins,secs,hunds);
					}

					m_sr_replay_info[i] = tmp;
					
/*
					{
						FILE *fp;
						char tmp2[256];

						sprintf(tmp2,"%sreplays/update-%s",m_player_data_path,m_sr_replay_names[i]);
						fp=fopen(tmp2,"wb+");

						while(rpl->read_one_cycle());
						rpl->save(fp);
						fclose(fp);
					}
*/
					delete rpl;
				} // if 
			} // if 
		} // for
	} // if 


	return TGL_STATE_REPLAYBROWSER;
} /* TGLapp::replaybrowser_cycle */ 


void TGLapp::replaybrowser_draw(void)
{
//	char buffer[255];

	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	TGLinterface::draw();

	if (m_rb_replay_selected!=-1) {
		int i=m_rb_replay_selected-m_sr_first_replay;
		if (i>=0 && i<SAVEREPLAY_REPLAYSPERPAGE) {
            drawQuad(15,float(40+i*22),570,20,0.5f,0.5f,1,0.7f);
		} // if
	} // if 

	if (m_rb_mouse_over_replay!=-1) {
		float f;
		f=float(0.7+0.2*sin((m_state_cycle)*0.3F));
        drawQuad(15,float(40+m_rb_mouse_over_replay*22),570,20,0.5f,1,0.5f,f);
	} // if

	// show the list of files:
	{
		int i,j;
		char *info;		

		for(j=0,i=m_sr_first_replay;i<m_sr_first_replay+SAVEREPLAY_REPLAYSPERPAGE && i<m_sr_replay_names.size();i++,j++) {
			{
				int old[4];
				bool old_scissor=false;

				glGetIntegerv(GL_SCISSOR_BOX,old);
				if (glIsEnabled(GL_SCISSOR_TEST)) old_scissor=true;
				glEnable(GL_SCISSOR_TEST);
				glScissor(10,0,200,480);
				TGLinterface::print_left(m_sr_replay_names[i],m_font16,20,float(60+j*22));
				glScissor(old[0],old[1],old[2],old[3]);
				if (!old_scissor) glDisable(GL_SCISSOR_TEST);
			}

			TGLinterface::print_left("-",m_font16,220,float(60+j*22));			

			info = m_sr_replay_info[i];
			if (info!=0) {

				TGLinterface::print_left(info,m_font16,250,float(60+j*22));

			} // if 
		} // for
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
} /* TGLapp::replaybrowser_draw */ 

