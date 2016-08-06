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

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
extern const char *persistent_data_folder;
#endif


extern SDL_Window *appwindow;

int TGLapp::savereplay_cycle(KEYBOARDSTATE *k)
{
	bool check_for_replays_to_load=false;

	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0) {
		TGLinterface::reset();
		SDL_WarpMouseInWindow(appwindow,210,352);
		TGLinterface::add_element(new TGLframe(10,32,580,276));		
		m_replay_save_button=new TGLbutton("Save",m_font32,110,404,200,64,0);
		TGLinterface::add_element(m_replay_save_button);
		TGLinterface::add_element(new TGLbutton("Cancel",m_font32,330,404,200,64,1));

		m_rb_mouse_over_replay=-1;
		m_rb_replay_selected=-1;

		// Find an initial name for the replay:
		{
			int i=0;
			FILE *fp;
			char tmp[255],tmp2[255];
			bool found=false;

			do {
				if (i==0) {
					sprintf(tmp2,"replay.rpl");
				} else {
					sprintf(tmp2,"replay_%i.rpl",i);
				} // if 

				sprintf(tmp,"%sreplays/%s",m_player_data_path, tmp2);
				fp=fopen(tmp,"rb");
				if (fp!=0) {
					fclose(fp);
					found=true;
				} else {
					found=false;
				} // if 
				i++;
			} while(found);

			m_replay_name_inputframe=new TGLTextInputFrame(tmp2,32,m_font16,100,340,440,32,0);
			TGLinterface::add_element(m_replay_name_inputframe);
			m_replay_name_inputframe->m_focus=true;
		}

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
			} // if
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
					} // if

				} /* while */
				(void) closedir (dp);
			} // if
#endif

			for(i=0;i<m_sr_replay_names.size();i++) {
				char *tmp;

				tmp=new char[strlen(m_player_data_path) + strlen(m_sr_replay_names[i]) + 10];
				sprintf(tmp,"%sreplays/%s",m_player_data_path, m_sr_replay_names[i]);
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

		if (k->key_press(SDL_SCANCODE_SPACE)) button=1;

		ID=TGLinterface::update_state(mouse_x,mouse_y,button,button_status,k);

		if ((ID==0 && m_replay_save_button->m_enabled) || ID==1) {
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

		// Check to see if the mouse is over a replay:
		m_rb_mouse_over_replay=-1;
		if (mouse_x>=20 && mouse_x<600) {
			if (mouse_y>=40) {
				int selected=(mouse_y-40)/22;

				if (selected>=0 && selected<(m_sr_replay_names.size()-m_sr_first_replay) && selected<SAVEREPLAY_REPLAYSPERPAGE) {
					m_rb_mouse_over_replay=selected;

					if (button!=0) {
						m_rb_replay_selected=selected;
						strcpy(m_replay_name_inputframe->m_editing,m_sr_replay_names[m_rb_replay_selected]);
						m_replay_name_inputframe->m_editing_position=int(strlen(m_replay_name_inputframe->m_editing));
						m_replay_name_inputframe->m_enabled=true;
					} // if
				} // if 
			} // if
		} // if 

		// Check if the name is a valid file:
		{
			FILE *fp;
			bool valid_replay_name=true;
			char tmp[256];

			sprintf(tmp,"%sreplays/%s",m_player_data_path,m_replay_name_inputframe->m_editing);

            fp=fopen(tmp,"rb");
            if (fp!=0) {
                valid_replay_name=false;
                fclose(fp);
            } // if

			if (valid_replay_name) {
				fp=fopen(tmp,"wb");
				if (fp!=0) {					
					fclose(fp);
					remove(tmp);
				} else {
					valid_replay_name=false;
				} // if
            } // if

			if (valid_replay_name) m_replay_save_button->m_enabled=true;
							  else m_replay_save_button->m_enabled=false;

		}

	}

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		switch(m_state_selection) {
		case 0:
				{
					FILE *fp;
					char tmp[512];
//                    FILE *fp_uncompressed;
//                    char tmp_uncompressed[512];

					sprintf(tmp,"%sreplays/%s",m_player_data_path,m_replay_name_inputframe->m_editing);
//                    sprintf(tmp_uncompressed,"%sreplays/%s.xml",m_player_data_path,m_replay_name_inputframe->m_editing);

					fp=fopen(tmp,"wb");
					if (fp!=0) {
						m_game_replay->save(fp, true, true);
						fclose(fp);
					} // if
//                    fp_uncompressed=fopen(tmp_uncompressed,"wb");
//                    if (fp_uncompressed!=0) {
//                        m_game_replay->save(fp_uncompressed, false, true);
//                        fclose(fp);
//                    } // if
                    
#ifdef __EMSCRIPTEN__
                    EM_ASM(
                           Module.print("Start File sync..");
                           Module.syncdone = 0;
                           FS.syncfs(false, function(err) {
                        assert(!err);
                        Module.print("End File sync..");
                        Module.syncdone = 1;
                    });
                           );
#endif
                }

				return TGL_STATE_POSTGAME;
				break;
		case 1:
				return TGL_STATE_POSTGAME;
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;

	if (check_for_replays_to_load) {
		int i,j;
		char *info,*name;

		for(j=0,i=m_sr_first_replay;i<m_sr_first_replay+SAVEREPLAY_REPLAYSPERPAGE && i<m_sr_replay_names.size();i++,j++) {
			name=m_sr_replay_names[i];
			info=m_sr_replay_info[i];

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



	return TGL_STATE_SAVEREPLAY;
} /* TGLapp::savereplay_cycle */ 

void TGLapp::savereplay_draw(void)
{
//	char buffer[255];

	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	TGLinterface::draw();

	if (m_rb_mouse_over_replay!=-1) {
		float f=float(0.7+0.2*sin((m_state_cycle)*0.3F));
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

			info=m_sr_replay_info[i];
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
} /* TGLapp::savereplay_draw */ 

