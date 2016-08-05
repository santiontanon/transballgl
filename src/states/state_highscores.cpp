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


int TGLapp::highscores_cycle(KEYBOARDSTATE *k)
{
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);

	if (m_state_cycle==0) {
//		TGLInterfaceElement *e;
	
		// compute the highscore matrix:
		{
            for(char *tmp:m_highscores_names) delete []tmp;
			m_highscores_names.clear();
			m_highscores_points.clear();
			m_highscores_time.clear();

			// Look for players:
			{
				int i;
				char *tmp;
				char buf[256];
				char folders[2][256];
				
				sprintf(folders[0],"%splayers",m_player_data_path);
				sprintf(folders[1],"%sother-players",m_player_data_path);

				for(i=0;i<2;i++) {
#ifdef _WIN32
					/* Find files: */
					WIN32_FIND_DATA finfo;
					HANDLE h;

					sprintf(buf,"%s/*.pp",folders[i]);
					h = FindFirstFile(buf, &finfo);
					if (h != INVALID_HANDLE_VALUE) {
						tmp=new char[strlen(finfo.cFileName)+1];
						strcpy(tmp,finfo.cFileName);
						tmp[strlen(tmp)-3]=0;
						m_highscores_names.push_back(tmp);

						while (FindNextFile(h, &finfo) == TRUE) {
							tmp=new char[strlen(finfo.cFileName)+1];
							strcpy(tmp,finfo.cFileName);
							tmp[strlen(tmp)-3]=0;
							m_highscores_names.push_back(tmp);
						} /* while */
					} // if
#else
					DIR *dp;
					struct dirent *ep;

					dp = opendir (folders[i]);
					if (dp != NULL) {
						while ((ep = readdir (dp))) {
							if (strlen(ep->d_name) > 4 &&
									ep->d_name[strlen(ep->d_name) - 3] == '.' &&
									ep->d_name[strlen(ep->d_name) - 2] == 'p' &&
									ep->d_name[strlen(ep->d_name) - 1] == 'p') {

								tmp=new char[strlen(ep->d_name)+1];
								strcpy(tmp,ep->d_name);
								tmp[strlen(tmp)-3]=0;
								m_highscores_names.push_back(tmp);
							} // if

						} /* while */
						(void) closedir (dp);
					} // if
#endif
				} // for

                for(char *tmp:m_highscores_names) {
					sprintf(buf,"%splayers/%s.pp",m_player_data_path,tmp);
					FILE *fp=fopen(buf,"rb");
					if (fp==0) {
						sprintf(buf,"%sother-players/%s.pp",m_player_data_path,tmp);
						fp=fopen(buf,"rb");
					} // if 
					if (fp!=0) {
						PlayerProfile *pfl=new PlayerProfile(fp);
						fclose(fp);
						
						{
							m_highscores_points.push_back((pfl->get_points()));
							m_highscores_time.push_back((pfl->get_accumulated_time()));
						}
					
						delete pfl;
					} else {
						m_highscores_points.push_back((-1));
						m_highscores_time.push_back((-1));
					} // if 
				} // while
			}

			// Sort the players:
			{
				// Bubble sort:
				bool changes = false;
				bool exchange = false;
				int i,j,itmp;
				char *cptr;

				do {
					changes = false;
					j = 0;
					for(i=j;i<m_highscores_names.size()-1;i++) {
						exchange = false;
						if ((m_highscores_points[i+1])>(m_highscores_points[i])) exchange = true;
						if ((m_highscores_points[i+1])==(m_highscores_points[i]) &&
							(m_highscores_time[i+1])<(m_highscores_time[i])) exchange = true;

						if (exchange) {
							cptr = m_highscores_names[i+1];
							m_highscores_names[i+1] = m_highscores_names[i];
							m_highscores_names[i] = cptr;
							
							itmp = m_highscores_points[i+1];
							m_highscores_points[i+1] = m_highscores_points[i];
							m_highscores_points[i] = itmp;

							itmp = m_highscores_time[i+1];
							m_highscores_time[i+1] = m_highscores_time[i];
							m_highscores_time[i] = itmp;

							changes = true;
						} // if

					} // for
					j++;
				}while(changes);
			}
		}

		m_highscores_first_name=0;

		TGLinterface::reset();
		TGLinterface::add_element(new TGLbutton("Back",m_font32,10,10,160,50,0));
		TGLinterface::add_element(new TGLframe(10,70,580,325));
		TGLinterface::add_element(new TGLText("Top Players:",m_font32,410,30,true));

		TGLinterface::add_element(new TGLbutton("Best Players",m_font32,10,410,200,50,4));
		TGLinterface::add_element(new TGLbutton("Best Times",m_font32,230,410,200,50,5));
		TGLinterface::disable(4);
		TGLinterface::enable(5);

		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/uparrow.png"),600,70,30,100,2));
		if (m_highscores_first_name==0) TGLinterface::disable(2);
								   else TGLinterface::enable(2);
		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/downarrow.png"),600,290,30,100,3));
		if (m_highscores_names.size()>HIGHSCORE_GLOBAL_PERPAGE) TGLinterface::enable(3);
														     else TGLinterface::disable(3);

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
			switch(ID) {
			case 0:
					m_state_fading=2;
					m_state_fading_cycle=0;
					m_state_selection=ID;
					break;
			case 2:
					m_highscores_first_name--;
					if (m_highscores_first_name==0) TGLinterface::disable(2);
											   else TGLinterface::enable(2);
					if (m_highscores_names.size()>m_highscores_first_name+HIGHSCORE_GLOBAL_PERPAGE) TGLinterface::enable(3);
																			  					 else TGLinterface::disable(3);	
					break;
			case 3:
					m_highscores_first_name++;
					if (m_highscores_first_name==0) TGLinterface::disable(2);
											   else TGLinterface::enable(2);
					if (m_highscores_names.size()>m_highscores_first_name+HIGHSCORE_GLOBAL_PERPAGE) TGLinterface::enable(3);
																			  					 else TGLinterface::disable(3);	
					break;
			case 4:
					break;
			case 5:
					m_state_fading=2;
					m_state_fading_cycle=0;
					m_state_selection=ID;
					TGLinterface::disable(4);
					TGLinterface::disable(5);
					break;

			} // switch
		} // if 
	}

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		SDL_ShowCursor(SDL_DISABLE);
		switch(m_state_selection) {
		case 0: if (m_lp_music_channel!=-1) Mix_HaltChannel(m_lp_music_channel);
				m_lp_music_channel=-1;
				return TGL_STATE_MAINMENU;
				break;
		case 4: return TGL_STATE_HIGHSCORES;
				break;
		case 5: return TGL_STATE_HIGHSCORES_TIMES;
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;

	if (m_state_fading==0 && m_lp_music_channel!=-1) Mix_Volume(m_lp_music_channel, int(m_player_profile->m_music_volume*(m_state_fading_cycle/25.0f)));
	if (m_state_fading==1 && m_lp_music_channel!=-1) Mix_Volume(m_lp_music_channel, m_player_profile->m_music_volume);
	if (m_state_fading==2 && m_lp_music_channel!=-1) Mix_Volume(m_lp_music_channel, int(m_player_profile->m_music_volume*((25-m_state_fading_cycle)/25.0f)));

	return TGL_STATE_HIGHSCORES;
} /* TGLapp::highscores_cycle */ 


void TGLapp::highscores_draw(void)
{
	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	TGLinterface::draw();

	// show the "Best Players" highscores:
	{
		int i,j;
		int start_y = 100;
		float r=1,g=1,b=1;

		for(j=0,i=m_highscores_first_name;i<m_highscores_first_name+HIGHSCORE_GLOBAL_PERPAGE && i<m_highscores_names.size();i++,j++) {

			if (strcmp(m_highscores_names[i],get_player_profile()->m_name)==0) {
				r=0;
				g=1;
				b=0;
			} else {
				r=g=b=1;
			} // if 	

			{
				char buf[256];
				sprintf(buf,"(%i)",i+1);
				TGLinterface::print_left(buf,m_font16,20,float(start_y+j*22),r,g,b,1);
			}

			{
				int old[4];
				bool old_scissor=false;

				glGetIntegerv(GL_SCISSOR_BOX,old);
				if (glIsEnabled(GL_SCISSOR_TEST)) old_scissor=true;
				glEnable(GL_SCISSOR_TEST);
				glScissor(10,0,200,480);
				TGLinterface::print_left(m_highscores_names[i],m_font16,70,float(start_y+j*22),r,g,b,1);
				glScissor(old[0],old[1],old[2],old[3]);
				if (!old_scissor) glDisable(GL_SCISSOR_TEST);
			}

			{
				char buf[256];
				sprintf(buf,"%i",(m_highscores_points[i]));
				TGLinterface::print_left(buf,m_font16,250,float(start_y+j*22),r,g,b,1);

				{
					int days,hours,mins,secs,milis;
					milis=(m_highscores_time[i]);
					if (milis<0) {
						TGLinterface::print_left("---",m_font16,350,float(start_y+j*22),r,g,b,1);
					} else {
						secs=milis/1000;
						milis=milis%1000;
						mins=secs/60;
						secs=secs%60;
						hours=mins/60;
						mins=mins%60;
						days=hours/24;
						hours=hours%24;
						if (days>0) {
							sprintf(buf,"%id %ih %im:%.2is:%.2i",days,hours,mins,secs,milis/10);
						} else {
							if (hours>0) {
								sprintf(buf,"%ih %im:%.2is:%.2i",hours,mins,secs,milis/10);
							} else {
								sprintf(buf,"%im:%.2is:%.2i",mins,secs,milis/10);
							} // if 
						} // if 
						TGLinterface::print_left(buf,m_font16,350,float(start_y+j*22),r,g,b,1);
					} // if
				}
			}

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
} /* TGLapp::highscores_draw */ 

