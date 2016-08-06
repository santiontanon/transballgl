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

int TGLapp::mainmenu_cycle(KEYBOARDSTATE *k)
{
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);

	if (m_state_cycle==0) {
		TGLInterfaceElement *e;

		m_mm_demo_state=0;
		m_mm_demo_timmer=0;

		TGLinterface::reset();
		SDL_WarpMouseInWindow(appwindow, SCREEN_X/2,SCREEN_Y/2-5);
		TGLinterface::add_element(new TGLbuttonTransparent("PLAY",m_ifont32,SCREEN_X/2-80,SCREEN_Y/2-15,160,30,0));

		e=new TGLbuttonTransparent("OPTIONS",m_ifont32,SCREEN_X/2 - 170,SCREEN_Y/2+40,160,30,2);
		TGLinterface::add_element(e);

        e=new TGLbuttonTransparent("EDITOR",m_ifont32,SCREEN_X/2 + 10,SCREEN_Y/2+40,160,30,6);
		TGLinterface::add_element(e);
	
        e=new TGLbuttonTransparent("HIGH SCORES",m_ifont32,SCREEN_X/2 - 170,SCREEN_Y/2+95,160,30,3);
		TGLinterface::add_element(e);
		TGLinterface::add_element(new TGLbuttonTransparent("REPLAYS",m_ifont32,SCREEN_X/2 + 10,SCREEN_Y/2+95,160,30,4));
		TGLinterface::add_element(new TGLbuttonTransparent("QUIT",m_ifont32,SCREEN_X/2-80,SCREEN_Y/2+150,160,30,5));
        
#ifdef __EMSCRIPTEN__
        TGLinterface::disable(6);
        TGLinterface::disable(5);
#endif
        
	} // if 

	{
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

		if (ID!=-1 && m_state_fading!=2) {
			m_state_fading=2;
			m_state_fading_cycle=0;
			m_state_selection=ID;
		} // if 
	} // if 


	if (m_state_fading==2 && m_state_fading_cycle>25) {
		SDL_ShowCursor(SDL_DISABLE);
		switch(m_state_selection) {
		case 0: 
				if (m_current_levelpack!=0) delete m_current_levelpack;
				{
					FILE *fp;
					fp=fopen(get_player_profile()->get_last_levelpack(),"rb");
					if (fp!=0) {
						m_current_levelpack=new LevelPack(fp, TGL_PLAY_MAPS_FOLDER, m_GLTM);
						fclose(fp);
						
						return TGL_STATE_LEVELPACKSCREEN;
					} else {
						return TGL_STATE_NONE;
					} // if 
				} 

				return TGL_STATE_MAINMENU;
				break;
        case 2: m_configure_previous_state = m_state;
				return TGL_STATE_CONFIGURE;
				break;
		case 3: 
				return TGL_STATE_HIGHSCORES;
				break;
		case 4: 
				return TGL_STATE_REPLAYBROWSER;
				break;
		case 6: 
				return TGL_STATE_EDITOR;
				break;
		case 5: 
				return TGL_STATE_NONE;
				break;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;

	if (m_mm_demo_state==0) m_mm_demo_timmer++;
	if (m_mm_demo_state==0 && m_mm_demo_timmer>100) {
		// load demo
		char tmp[512];

		if (m_mm_game!=0) delete m_mm_game;
		m_mm_game=0;
		if (m_mm_replay!=0) delete m_mm_replay;
		m_mm_replay=0;

		sprintf(tmp,"tutorials/demo%i.rpl",rand()%12);

		FILE *fp;
		fp=fopen(tmp,"rb");
		if (fp!=0) {
			m_mm_replay=new TGLreplay(fp, m_GLTM);
			fclose(fp);
			m_mm_game=new TGL(m_mm_replay, 0, 0,m_GLTM);
			m_mm_game->reset();
		} // if 
		m_mm_demo_state=1;
		m_mm_demo_timmer=0;
	} // if 

	if (m_mm_demo_state==1 || m_mm_demo_state==2 || m_mm_demo_state==3) {
		if (m_mm_replay!=0) {
	
			m_mm_demo_timmer++;

			std::list<TGLobject *> *l=m_mm_game->get_map()->get_objects("TGLobject");
			std::list<TGLobject *> to_delete,to_add;
			bool retval;
			retval = m_mm_replay->execute_cycle(&m_lvc,l,&to_delete,&to_add);
			l->clear();
			delete l;

            for(TGLobject *o:to_delete) {
				m_mm_game->get_map()->remove_object(o);
				delete o;
			} // while
            to_delete.clear();

            for(TGLobject *o:to_add) {
                m_mm_game->get_map()->add_object(o);
//                delete o;
            } // while
            to_add.clear();

			if (!m_mm_game->cycle(&m_lvc,m_GLTM,m_SFXM,0)) retval=false;
			
			if (!retval) {
				delete m_mm_game;
				delete m_mm_replay;
				m_mm_replay=0;

				m_mm_demo_state=0;
				m_mm_demo_timmer=0;
			} // if  
		}

		if (m_mm_demo_state==1 && m_mm_demo_timmer>100) {
			m_mm_demo_state=2;
			m_mm_demo_timmer=0;
		} // if 

	} // if 
	if (m_mm_demo_state==2 && m_mm_demo_timmer>(m_mm_replay->get_length()-500)) {
		// stop demo
		m_mm_demo_state=3;
		m_mm_demo_timmer=0;
	} // if 
	if (m_mm_demo_state==3 && m_mm_demo_timmer>100) {
		// stop demo
		m_mm_demo_state=0;
		m_mm_demo_timmer=0;
	} // if 


	return TGL_STATE_MAINMENU;
} /* TGLapp::mainmenu_cycle */ 


void TGLapp::mainmenu_draw(void)
{
    int old[4];
    bool old_scissor=false;
    
    glGetIntegerv(GL_SCISSOR_BOX,old);
    if (glIsEnabled(GL_SCISSOR_TEST)) old_scissor=true;
    
	glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

	if (m_mm_game!=0) {

		switch(m_mm_demo_state) {
		case 0:
				m_GLTM->get("graphics/interface/gl.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/2,0,0,1);
				m_GLTM->get("graphics/interface/gl-inverted.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/2,0,0,1);
				m_GLTM->get("graphics/interface/transball.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/4,0,0,0.5f);
				break;
		case 1:
				{
					float f=m_mm_demo_timmer/100.0f;

					if (f>1) f=1;
					if (f<0) f=0;
                    glEnable(GL_SCISSOR_TEST);
                    glScissor(SCREEN_X/2-310,SCREEN_Y/2-230,620,460);
					m_mm_game->draw(1.0, m_GLTM);
                    glScissor(old[0],old[1],old[2],old[3]);
                    if (!old_scissor) glDisable(GL_SCISSOR_TEST);
                    
                    m_GLTM->get("graphics/interface/gl.png")->draw(1,1,1,1*(1-f),SCREEN_X/2,SCREEN_Y/2,0,0,1);
					m_GLTM->get("graphics/interface/gl-inverted.png")->draw(1,1,1,1*(1-f)+0.85f*f,SCREEN_X/2,SCREEN_Y/2,0,0,1);
					m_GLTM->get("graphics/interface/transball.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/4,0,0,0.5f);
				}
				break;
		case 2:
                glEnable(GL_SCISSOR_TEST);
                glScissor(SCREEN_X/2-310,SCREEN_Y/2-230,620,460);
				m_mm_game->draw(1.0, m_GLTM);
                glScissor(old[0],old[1],old[2],old[3]);
                if (!old_scissor) glDisable(GL_SCISSOR_TEST);
                
                m_GLTM->get("graphics/interface/gl-inverted.png")->draw(1,1,1,0.85f,SCREEN_X/2,SCREEN_Y/2,0,0,1);
				m_GLTM->get("graphics/interface/transball.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/4,0,0,0.5f);
				break;
		case 3:
				{
					float f=(100-m_mm_demo_timmer)/100.0f;

					if (f>1) f=1;
					if (f<0) f=0;
                    glEnable(GL_SCISSOR_TEST);
                    glScissor(SCREEN_X/2-310,SCREEN_Y/2-230,620,460);
                    m_mm_game->draw(1.0, m_GLTM);
                    glScissor(old[0],old[1],old[2],old[3]);
                    if (!old_scissor) glDisable(GL_SCISSOR_TEST);
                    m_GLTM->get("graphics/interface/gl.png")->draw(1,1,1,1*(1-f),SCREEN_X/2,SCREEN_Y/2,0,0,1);
					m_GLTM->get("graphics/interface/gl-inverted.png")->draw(1,1,1,1*(1-f)+0.85f*f,SCREEN_X/2,SCREEN_Y/2,0,0,1);
					m_GLTM->get("graphics/interface/transball.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/4,0,0,0.5f);
				}
				break;
		} // switch
	} else {
		m_GLTM->get("graphics/interface/gl.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/2,0,0,1);
		m_GLTM->get("graphics/interface/gl-inverted.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/2,0,0,1);
		m_GLTM->get("graphics/interface/transball.png")->draw(1,1,1,1,SCREEN_X/2,SCREEN_Y/4,0,0,0.5f);
	} // if 

	if (m_state_cycle<50) {
		TGLinterface::draw(m_state_cycle/50.0f);
	} else {
		TGLinterface::draw();
	} // if 

	switch(m_state_fading) {
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

} /* TGLapp::mainmenu_draw */ 

