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
#include "TGLinterface.h"

#include "LevelPack.h"
#include "PlayerProfile.h"

extern SDL_Window *appwindow;


int TGLapp::loadlevelpack_cycle(KEYBOARDSTATE *k)
{
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0) {

		m_lpb_mouse_over_lp=-1;
		m_lpb_lp_selected=-1;

		TGLinterface::reset();
		SDL_WarpMouseInWindow(appwindow, 120, 436);
		TGLinterface::add_element(new TGLframe(10,32,580,236));		
		TGLinterface::add_element(new TGLbutton("Back",m_font32,40,404,160,64,1));
		m_lpb_select_button=new TGLbutton("Load",m_font32,240,404,160,64,0);
		TGLinterface::add_element(m_lpb_select_button);		
		m_lpb_select_button->m_enabled=false;

		// Load the lp filenames:
		{
			char *tmp;
			int i;

			m_lpb_first_lp=0;
            for(char *tmp:m_lpb_lp_fullnames) delete []tmp;
			m_lpb_lp_fullnames.clear();

            for(char *tmp:m_lpb_lp_names) delete []tmp;
            m_lpb_lp_names.clear();

            for(char *tmp:m_lpb_lp_info) delete []tmp;
            m_lpb_lp_info.clear();

            for(char *tmp:m_lpb_lp_longinfo) delete []tmp;
            m_lpb_lp_longinfo.clear();

            char foldername[512];
            sprintf(foldername,"%s%s", m_player_data_path, TGL_EDIT_MAPS_FOLDER);

#ifdef _WIN32
			/* Find files: */
			WIN32_FIND_DATA finfo;
			HANDLE h;

            char query[512];
            sprintf(query,"%s/*.lp", foldername);

            h = FindFirstFile(query, &finfo);
			if (h != INVALID_HANDLE_VALUE) {
				tmp=new char[strlen(finfo.cFileName)+1];
				strcpy(tmp,finfo.cFileName);
				m_lpb_lp_names.push_back(tmp);

				while (FindNextFile(h, &finfo) == TRUE) {
					tmp=new char[strlen(finfo.cFileName)+1];
					strcpy(tmp,finfo.cFileName);
					m_lpb_lp_names.push_back(tmp);
				} /* while */
			} // if
#else
			DIR *dp;
			struct dirent *ep;

            dp = opendir (foldername);
			if (dp != NULL) {
				while ((ep = readdir (dp))) {
					if (strlen(ep->d_name) > 4 &&
							ep->d_name[strlen(ep->d_name) - 3] == '.' &&
							ep->d_name[strlen(ep->d_name) - 2] == 'l' &&
							ep->d_name[strlen(ep->d_name) - 1] == 'p') {

						tmp=new char[strlen(ep->d_name)+1];
						strcpy(tmp,ep->d_name);
						m_lpb_lp_names.push_back(tmp);
					} // if

				} /* while */
				(void) closedir (dp);
			} // if
#endif

			for(i=0;i<m_lpb_lp_names.size();i++) {
				char *tmp;

				tmp=new char[512];
				sprintf(tmp,"%s/%s",foldername, getAt<char *>(m_lpb_lp_names,i));
				m_lpb_lp_fullnames.push_back(tmp);
				{
					LevelPack *lp;

					FILE *fp;
					fp=fopen(tmp,"rb");
					if (fp!=0) {
						lp=new LevelPack(fp, foldername, m_GLTM);
						fclose(fp);

						if (lp!=0) {
							char *tmp2;

							tmp2=new char[strlen(lp->m_name)+1];
							strcpy(tmp2,lp->m_name);
							m_lpb_lp_info.push_back(tmp2);

							tmp2=new char[(lp->m_creator_name==0 ? 0:strlen(lp->m_creator_name))+(lp->m_creator_nickname==0 ? 0:strlen(lp->m_creator_nickname))+strlen(lp->m_description)+32];
							sprintf(tmp2,"Creator: %s (%s)/%s",(lp->m_creator_name==0 ? "-":lp->m_creator_name),(lp->m_creator_nickname==0 ? "-":lp->m_creator_nickname),lp->m_description);
							m_lpb_lp_longinfo.push_back(tmp2);
							delete lp;
						} else {
							m_lpb_lp_info.push_back(0);
							m_lpb_lp_longinfo.push_back(0);
						} // if 
					} else {
						m_lpb_lp_info.push_back(0);
						m_lpb_lp_longinfo.push_back(0);
					} // if 
				}
			} // for
		}

		m_lpb_lp_uparrow=new TGLbutton(m_GLTM->get("graphics/interface/uparrow.png"),600,32,30,100,2);
		TGLinterface::add_element(m_lpb_lp_uparrow);
		if (m_lpb_first_lp==0) m_lpb_lp_uparrow->m_enabled=false;
						  else m_lpb_lp_uparrow->m_enabled=true;
		m_lpb_lp_downarrow=new TGLbutton(m_GLTM->get("graphics/interface/downarrow.png"),600,100+32+36,30,100,3);
		TGLinterface::add_element(m_lpb_lp_downarrow);
		if (m_lpb_lp_names.size()>LEVELPACKBROWSER_LPPERPAGE) m_lpb_lp_downarrow->m_enabled=true;
														   else m_lpb_lp_downarrow->m_enabled=false;

	} // if 

	if (m_state_fading==1) {
		int mouse_x=0,mouse_y=0,button=0,button_status=0;
		int ID=-1;
		if (!m_mouse_click_x.empty()) {
			mouse_x=m_mouse_click_x.front();
            m_mouse_click_x.pop_front();
			mouse_y=m_mouse_click_y.front();
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
			m_lpb_first_lp--;
			if (m_lpb_first_lp==0) m_lpb_lp_uparrow->m_enabled=false;
							  else m_lpb_lp_uparrow->m_enabled=true;
			if (m_lpb_lp_names.size()>m_lpb_first_lp+LEVELPACKBROWSER_LPPERPAGE) m_lpb_lp_downarrow->m_enabled=true;
																			  else m_lpb_lp_downarrow->m_enabled=false;	
		} // if 

		if (ID==3) {
			m_lpb_first_lp++;
			if (m_lpb_first_lp==0) m_lpb_lp_uparrow->m_enabled=false;
							  else m_lpb_lp_uparrow->m_enabled=true;
			if (m_lpb_lp_names.size()>m_lpb_first_lp+LEVELPACKBROWSER_LPPERPAGE) m_lpb_lp_downarrow->m_enabled=true;
																			  else m_lpb_lp_downarrow->m_enabled=false;
		} // if m_lpb_lp_downarrow


		// Check to see if the mouse is over a lvelpack:
		m_lpb_mouse_over_lp=-1;
		if (mouse_x>=20 && mouse_x<600) {
			if (mouse_y>=40) {
				int selected=(mouse_y-40)/22;

				if (selected>=0 && selected<(m_lpb_lp_names.size()-m_lpb_first_lp) && selected<LEVELPACKBROWSER_LPPERPAGE) {
					m_lpb_mouse_over_lp=selected;

					if (button==1) {
						if (m_lpb_lp_selected == m_lpb_first_lp+selected) {
							// Double click:
							m_state_fading=2;
							m_state_fading_cycle=0;
							m_state_selection=0;
						} else {
							m_lpb_lp_selected=m_lpb_first_lp+selected;
						} // if
					} // if 
				} // if 
			} // if
		} // if 

	}

	if (m_lpb_lp_selected==-1) m_lpb_select_button->m_enabled=false;
						  else m_lpb_select_button->m_enabled=true;

	if (m_state_fading==2 && m_state_fading_cycle>25) {
		switch(m_state_selection) {
		case 0:
				// Select level pack:			
				{
					LevelPack *lp;

					FILE *fp;
					fp=fopen(getAt<char *>(m_lpb_lp_fullnames,m_lpb_lp_selected),"rb");
					if (fp!=0) {
                        char foldername[512];
                        sprintf(foldername,"%s%s", m_player_data_path, TGL_EDIT_MAPS_FOLDER);
						lp=new LevelPack(fp, foldername, m_GLTM);
						fclose(fp);

						if (lp!=0) {
							if (m_editor_levelpack!=0) delete m_editor_levelpack;
							m_editor_levelpack=lp;
							m_editor_level=0;
						} // if 
						return TGL_STATE_EDITOR;
					} else {
						return TGL_STATE_NONE;
					} // if 
				} 
	
				return TGL_STATE_EDITOR;
				break;
		case 1:
				return TGL_STATE_EDITOR;
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;


	return TGL_STATE_LOADLEVELPACK;
} /* TGLapp::loadlevelpack_cycle */ 


void TGLapp::loadlevelpack_draw(void)
{
//	char buffer[255];

	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

	TGLinterface::draw();

	if (m_lpb_lp_selected!=-1) {
		int i=m_lpb_lp_selected-m_lpb_first_lp;
		if (i>=0 && i<LEVELPACKBROWSER_LPPERPAGE) drawQuad(15,float(40+i*22),SCREEN_X-80,20,0.5f,0.5f,1,0.7f);
	} // if 

	if (m_lpb_mouse_over_lp!=-1) {
		float f=float(0.7+0.2*sin((m_state_cycle)*0.3F));
        drawQuad(15,float(40+m_lpb_mouse_over_lp*22),SCREEN_X-80,20,0.5f,1.0f,0.5f,f);
	} // if

	// show the list of files:
	{
		int i,j;
		char *info;		

		for(j=0,i=m_lpb_first_lp;i<m_lpb_first_lp+LEVELPACKBROWSER_LPPERPAGE && i<m_lpb_lp_names.size();i++,j++) {
			{
				int old[4];
				bool old_scissor=false;

				glGetIntegerv(GL_SCISSOR_BOX,old);
				if (glIsEnabled(GL_SCISSOR_TEST)) old_scissor=true;
				glEnable(GL_SCISSOR_TEST);
				glScissor(10,0,200,480);
				TGLinterface::print_left(getAt<char *>(m_lpb_lp_names,i),m_font16,20,float(60+j*22));
				glScissor(old[0],old[1],old[2],old[3]);
				if (!old_scissor) glDisable(GL_SCISSOR_TEST);
			}

			TGLinterface::print_left("-",m_font16,220,float(60+j*22));			

            info=getAt<char *>(m_lpb_lp_info,i);
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
} /* TGLapp::loadlevelpack_draw */ 

