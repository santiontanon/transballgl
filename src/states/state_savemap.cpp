#ifdef _WIN32
#include "windows.h"
#include "direct.h"
#else
#include <stddef.h>
#include <sys/types.h>
#include <dirent.h>
#include "ctype.h"
#include "unistd.h"
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
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_image.h"
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
#include "TGLinterface.h"

#include "LevelPack.h"
#include "PlayerProfile.h"

//#include "savepng.h"

extern SDL_Window *appwindow;

int TGLapp::savemap_cycle(KEYBOARDSTATE *k)
{
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0) {

        //#ifdef _WIN32
        //		_getcwd(m_mb_current_path,256);
        //#else
        //		getcwd(m_mb_current_path,256);
        //#endif
        //		strcat(m_mb_current_path,"/" TGL_EDIT_MAPS_FOLDER);
        sprintf(m_mb_current_path, "%s%s",m_player_data_path, TGL_EDIT_MAPS_FOLDER);
		m_mb_recheckfiles=true;

		m_mb_mouse_over_m=-1;
		m_mb_m_selected=-1;

		TGLinterface::reset();
		SDL_WarpMouseInWindow(appwindow,120,436);
		TGLinterface::add_element(new TGLframe(10,32,580,280));		
		TGLinterface::add_element(new TGLbutton("Back",m_font32,40,404,160,64,1));
		m_mb_select_button=new TGLbutton("Save",m_font32,240,404,160,64,0);
		TGLinterface::add_element(m_mb_select_button);		
        if (m_save_map_type==0) {
            TGLinterface::add_element(new TGLTextInputFrame("map.tgl",32,m_font16,100,340,440,32,4));
        } else {
            TGLinterface::add_element(new TGLTextInputFrame("map.png",32,m_font16,100,340,440,32,4));
        }
		((TGLTextInputFrame *)(TGLinterface::get(4)))->m_focus=true;

		m_mb_mouse_over_m=-1;
		m_mb_m_selected=-1;
		m_mb_m_uparrow=new TGLbutton(m_GLTM->get("graphics/interface/uparrow.png"),600,32,30,100,2);
		TGLinterface::add_element(m_mb_m_uparrow);
		if (m_mb_first_m==0) m_mb_m_uparrow->m_enabled=false;
						  else m_mb_m_uparrow->m_enabled=true;
		m_mb_m_downarrow=new TGLbutton(m_GLTM->get("graphics/interface/downarrow.png"),600,100+32+36,30,100,3);
		TGLinterface::add_element(m_mb_m_downarrow);
		if (m_mb_m_names.size()+m_mb_folders_names.size()>MAPBROWSER_MPERPAGE_SAVE) m_mb_m_downarrow->m_enabled=true;
																					 else m_mb_m_downarrow->m_enabled=false;
	} // if

	if (m_mb_recheckfiles) {

		m_mb_recheckfiles=false;

		// Save the map filenames:
		{
			m_mb_first_m=0;
            for(char *tmp: m_mb_folders_names) delete []tmp;
			m_mb_folders_names.clear();
            
            for(char *tmp: m_mb_m_names) delete []tmp;
			m_mb_m_names.clear();
            
            char extension[4];
            
            if (m_save_map_type==0) {
                extension[0] = 't';
                extension[1] = 'g';
                extension[2] = 'l';
                extension[3] = 0;
            } else {
                extension[0] = 'p';
                extension[1] = 'n';
                extension[2] = 'g';
                extension[3] = 0;
            }

#ifdef _WIN32
			/* Find files: */
			WIN32_FIND_DATA finfo;
			HANDLE h;
			char tmp_path[4096], *tmp;

			sprintf(tmp_path,"%s/*.*",m_mb_current_path);

			h = FindFirstFile(tmp_path, &finfo);
			if (h != INVALID_HANDLE_VALUE) {

				if ((finfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) {
					if (strlen(finfo.cFileName) > 4 &&
						finfo.cFileName[strlen(finfo.cFileName) - 4] == '.' &&
						finfo.cFileName[strlen(finfo.cFileName) - 3] == extension[0] &&
						finfo.cFileName[strlen(finfo.cFileName) - 2] == extension[1] &&
						finfo.cFileName[strlen(finfo.cFileName) - 1] == extension[2]) {
						tmp=new char[strlen(finfo.cFileName)+1];
						strcpy(tmp,finfo.cFileName);
						m_mb_m_names.push_back(tmp);
					} // if 
				} else {
					tmp=new char[strlen(finfo.cFileName)+1];
					strcpy(tmp,finfo.cFileName);
					m_mb_folders_names.push_back(tmp);
				} // if 

				while (FindNextFile(h, &finfo) == TRUE) {
					if ((finfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) {
						if (strlen(finfo.cFileName) > 4 &&
							finfo.cFileName[strlen(finfo.cFileName) - 4] == '.' &&
							finfo.cFileName[strlen(finfo.cFileName) - 3] == extension[0] &&
							finfo.cFileName[strlen(finfo.cFileName) - 2] == extension[1] &&
							finfo.cFileName[strlen(finfo.cFileName) - 1] == extension[2]) {
							tmp=new char[strlen(finfo.cFileName)+1];
							strcpy(tmp,finfo.cFileName);
							m_mb_m_names.push_back(tmp);
						} // if 
					} else {
						tmp=new char[strlen(finfo.cFileName)+1];
						strcpy(tmp,finfo.cFileName);
						m_mb_folders_names.push_back(tmp);
					} // if 
				} /* while */
			} // if
#else
			DIR *dp;
			struct dirent *ep;

			dp = opendir (m_mb_current_path);
			if (dp != NULL) {
				while ((ep = readdir (dp))) {
					if (strlen(ep->d_name) > 4 &&
							ep->d_name[strlen(ep->d_name) - 4] == '.' &&
							ep->d_name[strlen(ep->d_name) - 3] == extension[0] &&
							ep->d_name[strlen(ep->d_name) - 2] == extension[1] &&
							ep->d_name[strlen(ep->d_name) - 1] == extension[2]) {

						char *tmp=new char[strlen(ep->d_name)+1];
						strcpy(tmp,ep->d_name);
						m_mb_m_names.push_back(tmp);
					} // if

				} /* while */
				(void) closedir (dp);
			} // if
#endif
		}

		m_mb_mouse_over_m=-1;
		m_mb_m_selected=-1;
		if (m_mb_first_m==0) m_mb_m_uparrow->m_enabled=false;
  					    else m_mb_m_uparrow->m_enabled=true;
		if (m_mb_m_names.size()+m_mb_folders_names.size()>MAPBROWSER_MPERPAGE_SAVE) m_mb_m_downarrow->m_enabled=true;
																					 else m_mb_m_downarrow->m_enabled=false;

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

		if (ID==0 || ID==1) {
			m_state_fading=2;
			m_state_fading_cycle=0;
			m_state_selection=ID;
		} // if 

		if (ID==2) {
			m_mb_first_m--;
			if (m_mb_first_m==0) m_mb_m_uparrow->m_enabled=false;
							  else m_mb_m_uparrow->m_enabled=true;
			if (m_mb_m_names.size()+m_mb_folders_names.size()>m_mb_first_m+MAPBROWSER_MPERPAGE_SAVE) m_mb_m_downarrow->m_enabled=true;
																									  else m_mb_m_downarrow->m_enabled=false;	
		} // if 

		if (ID==3) {
			m_mb_first_m++;
			if (m_mb_first_m==0) m_mb_m_uparrow->m_enabled=false;
							  else m_mb_m_uparrow->m_enabled=true;
			if (m_mb_m_names.size()+m_mb_folders_names.size()>m_mb_first_m+MAPBROWSER_MPERPAGE_SAVE) m_mb_m_downarrow->m_enabled=true;
																									  else m_mb_m_downarrow->m_enabled=false;
		} // if m_mb_m_downarrow


		// Check to see if the mouse is over a map:
		m_mb_mouse_over_m=-1;
		if (mouse_x>=20 && mouse_x<600) {
			if (mouse_y>=40) {
				int selected=(mouse_y-40)/22;

				if (selected>=0 && selected<(m_mb_m_names.size()+m_mb_folders_names.size()-m_mb_first_m) && selected<MAPBROWSER_MPERPAGE_SAVE) {
					m_mb_mouse_over_m=selected;

					if (button==1) {
						int old_selected = m_mb_m_selected;
						m_mb_m_selected=m_mb_first_m+selected;

						if (old_selected == m_mb_m_selected) {
							// Clicked over the same file twice:

							if (m_mb_m_selected<m_mb_folders_names.size()) {

								// Selected a folder:
								char *folder = m_mb_folders_names[m_mb_m_selected];

								if (strcmp(folder,".")==0) {
								} else if (strcmp(folder,"..")==0) {
									int i,last=-1;
									for(i=0;m_mb_current_path[i]!=0;i++) {
										if (m_mb_current_path[i]=='/' || m_mb_current_path[i]=='\\') {
											last=i;
										} // if 
									} // for 
									if (last!=-1) m_mb_current_path[last]=0;
									m_mb_recheckfiles=true;
									m_mb_m_selected=-1;
									m_mb_first_m=0;
								} else {
									char tmp[256];
									sprintf(tmp,"%s/%s",m_mb_current_path,folder);
									strcpy(m_mb_current_path,tmp);
									m_mb_recheckfiles=true;
									m_mb_m_selected=-1;
									m_mb_first_m=0;
								} // if 

							} else {

								// Selected a map:
								strcpy(((TGLTextInputFrame *)(TGLinterface::get(4)))->m_editing, m_mb_m_names[int(m_mb_m_selected-m_mb_folders_names.size())]);
								((TGLTextInputFrame *)(TGLinterface::get(4)))->m_editing_position = int(strlen(m_mb_m_names[int(m_mb_m_selected-m_mb_folders_names.size())]));

							} // if 

						} // if 
					} // if
				} // if 
			} // if
		} // if 

	}

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		switch(m_state_selection) {
		case 0:
				// save:
                char tmp[256];
                sprintf(tmp,"%s/%s",m_mb_current_path,((TGLTextInputFrame *)(TGLinterface::get(4)))->m_editing);
 
                if ( m_save_map_type==0 ) {
					FILE *fp;
					fp=fopen(tmp,"w+");
					if (fp!=0) {
						m_editor_level_editing->save(fp,m_GLTM);
						fclose(fp);
					} // if 
                } else if (m_save_map_type == 1) {
#ifndef __EMSCRIPTEN__
                    // create the surface:
                    SDL_Surface *map_sfc = m_editor_level_editing->drawToSurface(m_GLTM);
                    IMG_SavePNG(map_sfc, tmp);
                    SDL_FreeSurface(map_sfc);
#endif
                }
	
				return TGL_STATE_MAPEDITOR;
				break;
		case 1:
				return TGL_STATE_MAPEDITOR;
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;


	return TGL_STATE_SAVEMAP;
} /* TGLapp::savemap_cycle */ 


void TGLapp::savemap_draw(void)
{
//	char buffer[255];

	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	TGLinterface::draw();

	if (m_mb_m_selected!=-1) {
		int i=m_mb_m_selected-m_mb_first_m;
		if (i>=0 && i<MAPBROWSER_MPERPAGE_SAVE) {
            drawQuad(15,float(40+i*22),570,20, 0.5f,0.5f,1,0.7f);
		} // if
	} // if 

	if (m_mb_mouse_over_m!=-1) {
		float f=float(0.7+0.2*sin((m_state_cycle)*0.3F));
        drawQuad(15,float(40+m_mb_mouse_over_m*22),570,20, 0.5f,1,0.5f,f);
	} // if 

	// show the list of files:
	{
		int i,j;

		for(j=0,i=m_mb_first_m;j<MAPBROWSER_MPERPAGE_SAVE && i<m_mb_folders_names.size();i++,j++) {
			{
				int old[4];
				bool old_scissor=false;

				glGetIntegerv(GL_SCISSOR_BOX,old);
				if (glIsEnabled(GL_SCISSOR_TEST)) old_scissor=true;
				glEnable(GL_SCISSOR_TEST);
				glScissor(10,0,200,480);
				TGLinterface::print_left(m_mb_folders_names[i],m_font16,20,float(60+j*22),1.0,1.0,0.0,1.0);
				glScissor(old[0],old[1],old[2],old[3]);
				if (!old_scissor) glDisable(GL_SCISSOR_TEST);
			}

			TGLinterface::print_left("-",m_font16,220,float(60+j*22));			
		} // for

		for(i-=m_mb_folders_names.size();j<MAPBROWSER_MPERPAGE_SAVE && i<m_mb_m_names.size();i++,j++) {
			{
				int old[4];
				bool old_scissor=false;

				glGetIntegerv(GL_SCISSOR_BOX,old);
				if (glIsEnabled(GL_SCISSOR_TEST)) old_scissor=true;
				glEnable(GL_SCISSOR_TEST);
				glScissor(10,0,200,480);
				TGLinterface::print_left(m_mb_m_names[i],m_font16,20,float(60+j*22),1.0,1.0,1.0,1.0);
				glScissor(old[0],old[1],old[2],old[3]);
				if (!old_scissor) glDisable(GL_SCISSOR_TEST);
			}

			TGLinterface::print_left("-",m_font16,220,float(60+j*22));			
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
} /* TGLapp::savemap_draw */ 

