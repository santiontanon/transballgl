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
#include "PlayerProfile.h"


int TGLapp::highscores_times_cycle(KEYBOARDSTATE *k)
{
	bool recompute_table = false;
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);

	if (m_state_cycle==0) {
	
		// compute the highscore matrix:
		{
			// Load the list of level packs:
			{
                for(char *tmp:m_highscores_lp_names) delete []tmp;
				m_highscores_lp_names.clear();
				m_highscores_level_pack=0;

#ifdef _WIN32
				/* Find files: */
				WIN32_FIND_DATA finfo;
				HANDLE h;

				h = FindFirstFile(TGL_PLAY_MAPS_FOLDER "/*.lp", &finfo);
				if (h != INVALID_HANDLE_VALUE) {
					tmp=new char[strlen(finfo.cFileName)+1];
					strcpy(tmp,finfo.cFileName);
					m_highscores_lp_names.push_back(tmp);

					while (FindNextFile(h, &finfo) == TRUE) {
						tmp=new char[strlen(finfo.cFileName)+1];
						strcpy(tmp,finfo.cFileName);
						m_highscores_lp_names.push_back(tmp);
					} /* while */
				} // if
#else
				DIR *dp;
				struct dirent *ep;

				dp = opendir (TGL_PLAY_MAPS_FOLDER);
				if (dp != NULL) {
					while ((ep = readdir (dp))) {
						if (strlen(ep->d_name) > 4 &&
								ep->d_name[strlen(ep->d_name) - 3] == '.' &&
								ep->d_name[strlen(ep->d_name) - 2] == 'l' &&
								ep->d_name[strlen(ep->d_name) - 1] == 'p') {

							char *tmp=new char[strlen(ep->d_name)+1];
							strcpy(tmp,ep->d_name);
							m_highscores_lp_names.push_back(tmp);
						} // if

					} /* while */
					(void) closedir (dp);
				} // if
#endif
			}
			
			if (m_current_levelpack!=0) {
				// find which is the current level pack:
				LevelPack *lp;
                for( char *name:m_highscores_lp_names) {
					char *tmp=new char[strlen(name)+9];
					sprintf(tmp,TGL_PLAY_MAPS_FOLDER "/%s",name);
					{
						FILE *fp;
						fp=fopen(tmp,"rb");
						if (fp!=0) {
							lp=new LevelPack(fp, TGL_PLAY_MAPS_FOLDER, m_GLTM);
							fclose(fp);
							if (strcmp(lp->m_name,m_current_levelpack->m_name)==0) {
								m_highscores_level_pack=name;
							}
						} else {
							lp=0;
						} // if 
					}
					delete []tmp;
				}
			} else {
				m_highscores_level_pack = m_highscores_lp_names.front();
			}


			recompute_table=true;

		}

		m_highscores_first_ship=0;
		m_highscores_first_level=0;

		TGLinterface::reset();
		TGLinterface::add_element(new TGLbutton("Back",m_font32,10,10,160,50,0));
		TGLinterface::add_element(new TGLframe(10,70,580,325));
		TGLinterface::add_element(new TGLText("Top Times:",m_font32,410,20,true));
		TGLinterface::add_element(new TGLText("---",m_font16,410,50,true,-10));

		TGLinterface::add_element(new TGLbutton("Best Players",m_font32,10,410,200,50,4));
		TGLinterface::add_element(new TGLbutton("Best Times",m_font32,230,410,200,50,5));
		TGLinterface::enable(4);
		TGLinterface::disable(5);

		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/uparrow.png"),600,70,30,100,2));
		TGLinterface::disable(2);
		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/downarrow.png"),600,290,30,100,3));
		TGLinterface::disable(3);

		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/leftarrow.png"),280,20,40,40,6));
		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/rightarrow.png"),500,20,40,40,7));
		if (indexOf<char *>(m_highscores_lp_names,m_highscores_level_pack)<=0) TGLinterface::disable(6);
																	  else TGLinterface::enable(6);
		if (indexOf<char *>(m_highscores_lp_names, m_highscores_level_pack)>=m_highscores_lp_names.size()-1) TGLinterface::disable(7);
																		 						     else TGLinterface::enable(7);

		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/leftarrow.png"),360,85,40,40,8));
		TGLinterface::disable(8);
		TGLinterface::add_element(new TGLbutton(m_GLTM->get("graphics/interface/rightarrow.png"),520,85,40,40,9));
		TGLinterface::enable(9);

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

					// UP / DOWN
			case 2: m_highscores_first_level--;
					if (m_highscores_first_level<=0) TGLinterface::disable(2);
												else TGLinterface::enable(2);
					if (m_highscores_first_level>=m_highscores_times_names[0].size()-3) TGLinterface::disable(3);
																					 else TGLinterface::enable(3);
					break;
			case 3: m_highscores_first_level++;
					if (m_highscores_first_level<=0) TGLinterface::disable(2);
												else TGLinterface::enable(2);
					if (m_highscores_first_level>=m_highscores_times_names[0].size()-3) TGLinterface::disable(3);
																					 else TGLinterface::enable(3);					
					break;
			case 4:
					m_state_fading=2;
					m_state_fading_cycle=0;
					m_state_selection=ID;
					TGLinterface::disable(4);
					TGLinterface::disable(5);
					break;
			case 5:
					break;

					// CHANGE LEVEL PACK:
			case 6:	m_highscores_level_pack = m_highscores_lp_names[indexOf<char *>(m_highscores_lp_names,m_highscores_level_pack)-1];
					if (indexOf<char *>(m_highscores_lp_names,m_highscores_level_pack)<=0) TGLinterface::disable(6);
																		 		  else TGLinterface::enable(6);
					if (indexOf<char *>(m_highscores_lp_names, m_highscores_level_pack)>=m_highscores_lp_names.size()-1) TGLinterface::disable(7);
																		 										 else TGLinterface::enable(7);
					recompute_table = true;
					break;
			case 7:	m_highscores_level_pack = m_highscores_lp_names[indexOf<char *>(m_highscores_lp_names, m_highscores_level_pack)+1];
					if (indexOf<char *>(m_highscores_lp_names, m_highscores_level_pack)<=0) TGLinterface::disable(6);
																		 		  else TGLinterface::enable(6);
					if (indexOf<char *>(m_highscores_lp_names, m_highscores_level_pack)>=m_highscores_lp_names.size()-1) TGLinterface::disable(7);
																		 										 else TGLinterface::enable(7);
					recompute_table = true;
					break;

					// BROWSE SHIPS:
			case 8:	m_highscores_first_ship--;
					if (m_highscores_first_ship<=0) TGLinterface::disable(8);
									   		   else TGLinterface::enable(8);
					if (m_highscores_first_ship>=N_SHIPS-1) TGLinterface::disable(9);
													   else TGLinterface::enable(9);
					break;
			case 9:	m_highscores_first_ship++;
					if (m_highscores_first_ship<=0) TGLinterface::disable(8);
									   		   else TGLinterface::enable(8);
					if (m_highscores_first_ship>=N_SHIPS-1) TGLinterface::disable(9);
													   else TGLinterface::enable(9);
					break;

			} // switch
		} // if 
	}

	if (recompute_table) {
		LevelPack *lp;
		int i;
		for(i=0;i<N_SHIPS+1;i++) {
            for(std::vector<char *> *tmpv:m_highscores_times_names[i]) {
                for(char *tmp:*tmpv) delete []tmp;
                tmpv->clear();
                delete tmpv;
            }
            m_highscores_times_names[i].clear();
            for(std::vector<int> *tmpv:m_highscores_times_time[i]) {
                tmpv->clear();
                delete tmpv;
            }
            m_highscores_times_time[i].clear();
		} // for

		// Load the level pack
		{
			char *tmp=new char[strlen(m_highscores_level_pack)+9];
			sprintf(tmp,TGL_PLAY_MAPS_FOLDER "/%s",m_highscores_level_pack);
			{
				FILE *fp;
				fp=fopen(tmp,"rb");
				if (fp!=0) {
					lp=new LevelPack(fp, TGL_PLAY_MAPS_FOLDER, m_GLTM);
					fclose(fp);
				} else {
					lp=0;
				} // if 
			}
			delete []tmp;
		}

		{
			TGLInterfaceElement *e;
			char *tmp;
			tmp = new char[strlen(lp->m_name)+1];
			strcpy(tmp,lp->m_name);
			e = TGLinterface::get(-10);
			TGLinterface::substitute_element(e,new TGLText(tmp,m_font16,410,50,true,-10));
			delete e;
		}

		// Initialize the table:
		if (lp!=0) {
			int j;

			for(i=0;i<N_SHIPS+1;i++) {
				for(j=0;j<lp->m_levels.size();j++) {
					m_highscores_times_names[i].push_back(new std::vector<char *>);
					m_highscores_times_time[i].push_back(new std::vector<int>);
				} // for
			} // for

		} // if

		// Load player by player and fill the table:
		{
			int i;
			char *tmp;
			char buf[256];
			const char *short_ship_names[]={"V-P",
									  "X-T",
									  "S R",
									  "N B",
									  "V B",
									  "DDG",
									  "GRV",
									  "ACC",
									  "GYR",
									  "D-F",
									  "C-H",
									  "PA3"
									};
			char folders[2][256];
			
			sprintf(folders[0],"%splayers",m_player_data_path);
			sprintf(folders[1],"%sother-players",m_player_data_path);

            for(char *tmp:m_highscores_names) delete []tmp;
			m_highscores_names.clear();


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
					char *tmp;
					fclose(fp);

                    for(PlayerProfileLPProgress *lpp:pfl->m_progress) {
						if (strcmp(lpp->m_levelpack_id,lp->m_id)==0) {
                            for(PlayerProfileLevelResult *lr:lpp->m_results) {
								// Insert new time:
								tmp = new char[strlen(pfl->m_name)+1];
								strcpy(tmp,pfl->m_name);
								m_highscores_times_names[lr->m_ship][lr->m_level]->push_back(tmp);
								m_highscores_times_time[lr->m_ship][lr->m_level]->push_back(lr->m_best_time);

								tmp = new char[strlen(pfl->m_name)+7];
								sprintf(tmp,"%s [%s]",pfl->m_name,short_ship_names[lr->m_ship]);
								m_highscores_times_names[N_SHIPS][lr->m_level]->push_back(tmp);
								m_highscores_times_time[N_SHIPS][lr->m_level]->push_back(lr->m_best_time);

								// Sort the list:
								{
									// Bubble sort:
									bool changes = false;
									bool exchange = false;
									int i,j;
									char *cptr;

									do {
										changes = false;
										j = 0;
										for(i=j;i<m_highscores_times_names[lr->m_ship][lr->m_level]->size()-1;i++) {
											exchange = false;
											if ((m_highscores_times_time[lr->m_ship][lr->m_level]->operator[](i+1))<(m_highscores_times_time[lr->m_ship][lr->m_level]->operator[](i))) exchange = true;

											if (exchange) {
												cptr = m_highscores_times_names[lr->m_ship][lr->m_level]->operator[](i+1);
												(*(m_highscores_times_names[lr->m_ship][lr->m_level]))[i+1] =m_highscores_times_names[lr->m_ship][lr->m_level]->operator[](i);
												(*(m_highscores_times_names[lr->m_ship][lr->m_level]))[i] = cptr;
												
												int itmp = m_highscores_times_time[lr->m_ship][lr->m_level]->operator[](i+1);
												(*(m_highscores_times_time[lr->m_ship][lr->m_level]))[i+1]=m_highscores_times_time[lr->m_ship][lr->m_level]->operator[](i);
												(*(m_highscores_times_time[lr->m_ship][lr->m_level]))[i] = itmp;

												changes = true;
											} // if

										} // for
										j++;
									}while(changes);
								}

								{
									// Bubble sort:
									bool changes = false;
									bool exchange = false;
									int i,j;
									char *cptr;

									do {
										changes = false;
										j = 0;
										for(i=j;i<m_highscores_times_names[N_SHIPS][lr->m_level]->size()-1;i++) {
											exchange = false;
											if ((m_highscores_times_time[N_SHIPS][lr->m_level]->operator[](i+1))<(m_highscores_times_time[N_SHIPS][lr->m_level]->operator[](i))) exchange = true;

											if (exchange) {
                                                
                                                cptr = m_highscores_times_names[N_SHIPS][lr->m_level]->operator[](i+1);
                                                (*(m_highscores_times_names[N_SHIPS][lr->m_level]))[i+1] =m_highscores_times_names[N_SHIPS][lr->m_level]->operator[](i);
                                                (*(m_highscores_times_names[N_SHIPS][lr->m_level]))[i] = cptr;
                                                
                                                int itmp = m_highscores_times_time[N_SHIPS][lr->m_level]->operator[](i+1);
                                                (*(m_highscores_times_time[N_SHIPS][lr->m_level]))[i+1] = m_highscores_times_time[N_SHIPS][lr->m_level]->operator[](i);
                                                (*(m_highscores_times_time[N_SHIPS][lr->m_level]))[i] = itmp;
                                                
												changes = true;
											} // if

										} // for
										j++;
									}while(changes);
								}

							} // while 
						} // if 
					} // while
				
					delete pfl;
				} else {
				} // if 
			} // while
		}

		if (lp!=0) delete lp;
		recompute_table=false;
		m_highscores_first_level=0;
		if (m_highscores_first_level<=0) TGLinterface::disable(2);
									else TGLinterface::enable(2);
		if (m_highscores_first_level>=m_highscores_times_names[0].size()-3) TGLinterface::disable(3);
																		 else TGLinterface::enable(3);
	} // if


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

	return TGL_STATE_HIGHSCORES_TIMES;
} /* TGLapp::highscores_times_cycle */ 


void TGLapp::highscores_times_draw(void)
{
	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	TGLinterface::draw();

	// show the "Best Times" highscores:
	{
		int j,k;
		int ship,level;
		int NLEVELS = 3, NRESULTS = 3;
		int start_y = 95;
		char tmp[256];

		for(j=0;j<NLEVELS && j+m_highscores_first_level<m_highscores_times_names[0].size();j++) {
			level=j+m_highscores_first_level;
			
			sprintf(tmp,"Level %i",level+1);
			TGLinterface::print_left(tmp,m_font16,15,float(start_y+60+j*80));

            drawLine(30,float(start_y+35+j*80),
                     560,float(start_y+35+j*80),
                     1,1,1,1);
		} // for

		{
			ship=m_highscores_first_ship;
			{
				GLTile *t;

				{
					t=m_GLTM->get(TGL::ship_tiles[ship]);

					t->draw(float(150+310),float(start_y),0,0,0.6f);

					TGLinterface::print_center(TGL::ship_names[ship],m_font16,float(150+310),float(start_y+30));
	
					TGLinterface::print_center("Any ship",m_font16,float(150),float(start_y+30));
				} // if
			}

			for(j=0;j<NLEVELS && j+m_highscores_first_level<m_highscores_times_names[0].size();j++) {
				level=j+m_highscores_first_level;
				
				for(k=0;k<NRESULTS;k++) {
					if (k<m_highscores_times_names[N_SHIPS][level]->size()) {
						int days,hours,mins,secs,milis;
						milis=m_highscores_times_time[N_SHIPS][level]->operator [](k);
						if (milis<0) {
							sprintf(tmp,"(%i) - %s - ---",k+1,m_highscores_times_names[N_SHIPS][level]->operator [](k));
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
								sprintf(tmp,"(%i) - %id %ih %im:%.2is:%.2i - %s",k+1,days,hours,mins,secs,milis/10,m_highscores_times_names[N_SHIPS][level]->operator [](k));
							} else {
								if (hours>0) {
									sprintf(tmp,"(%i) - %ih %im:%.2is:%.2i - %s",k+1,hours,mins,secs,milis/10,m_highscores_times_names[N_SHIPS][level]->operator [](k));
								} else {
									sprintf(tmp,"(%i) - %im:%.2is:%.2i - %s",k+1,mins,secs,milis/10,m_highscores_times_names[N_SHIPS][level]->operator [](k));
								} // if 
							} // if 
						} // if

						{
							char tmp2[128];
							strcpy(tmp2,m_highscores_times_names[N_SHIPS][level]->operator [](k));
							tmp2[strlen(tmp2)-6]=0;
							if (strcmp(tmp2,get_player_profile()->m_name)==0) {
								TGLinterface::print_left(tmp,m_font16,float(90),float(start_y+60+j*80+k*20),0,1,0,1);
							} else {
								TGLinterface::print_left(tmp,m_font16,float(90),float(start_y+60+j*80+k*20));
							} // if 
						}
					} else {
						sprintf(tmp,"(%i) - --- - ---",k+1);
						TGLinterface::print_left(tmp,m_font16,float(90),float(start_y+60+j*80+k*20));
					} // if
				} // for
			} // for

			for(j=0;j<NLEVELS && j+m_highscores_first_level<m_highscores_times_names[0].size();j++) {
				level=j+m_highscores_first_level;
				
				for(k=0;k<NRESULTS;k++) {
					if (k<m_highscores_times_names[ship][level]->size()) {
						int days,hours,mins,secs,milis;
						milis=m_highscores_times_time[ship][level]->operator [](k);
						if (milis<0) {
							sprintf(tmp,"(%i) - %s - ---",k+1,m_highscores_times_names[ship][level]->operator [](k));
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
								sprintf(tmp,"(%i) - %id %ih %im:%.2is:%.2i - %s",k+1,days,hours,mins,secs,milis/10,m_highscores_times_names[ship][level]->operator [](k));
							} else {
								if (hours>0) {
									sprintf(tmp,"(%i) - %ih %im:%.2is:%.2i - %s",k+1,hours,mins,secs,milis/10,m_highscores_times_names[ship][level]->operator [](k));
								} else {
									sprintf(tmp,"(%i) - %im:%.2is:%.2i - %s",k+1,mins,secs,milis/10,m_highscores_times_names[ship][level]->operator [](k));
								} // if 
							} // if 
						} // if

						if (strcmp(m_highscores_times_names[ship][level]->operator [](k),get_player_profile()->m_name)==0) {
							TGLinterface::print_left(tmp,m_font16,float(100+260),float(start_y+60+j*80+k*20),0,1,0,1);
						} else {
							TGLinterface::print_left(tmp,m_font16,float(100+260),float(start_y+60+j*80+k*20));
						} // if 
					} else {
						sprintf(tmp,"(%i) - --- - ---",k+1);
						TGLinterface::print_left(tmp,m_font16,float(100+260),float(start_y+60+j*80+k*20));
					} // if

				} // for

			} // for
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
} /* TGLapp::highscores_times_draw */ 

