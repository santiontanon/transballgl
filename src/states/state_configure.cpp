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
#include "TGLinterface.h"

#include "LevelPack.h"
#include "PlayerProfile.h"

extern SDL_Window *appwindow;

int TGLapp::configure_cycle(KEYBOARDSTATE *k)
{
	if (SDL_ShowCursor(SDL_QUERY)!=SDL_ENABLE) SDL_ShowCursor(SDL_ENABLE);
	if (m_state_cycle==0 || m_configure_refresh_gui) {
        int x_offs = (SCREEN_X-640)/2;
        m_configure_refresh_gui = false;

		m_lpb_mouse_over_lp=-1;
		m_lpb_lp_selected=-1;

		TGLinterface::reset();
		SDL_WarpMouseInWindow(appwindow,x_offs+120,436);
		TGLinterface::add_element(new TGLbutton("Back",m_font32,x_offs+40,404,160,64,0));
        if (m_configure_previous_state==TGL_STATE_GAME) {
            TGLinterface::add_element(new TGLbutton("Quit",m_font32,x_offs+240,404,160,64,100));
        }

		TGLinterface::add_element(new TGLbutton("Thrust",m_font16,x_offs+40,20,100,32,1));
		TGLinterface::add_element(new TGLbutton("Special",m_font16,x_offs+40,60,100,32,2));
		TGLinterface::add_element(new TGLbutton("Left",m_font16,x_offs+40,100,100,32,3));
		TGLinterface::add_element(new TGLbutton("Right",m_font16,x_offs+40,140,100,32,4));
		TGLinterface::add_element(new TGLbutton("Fire",m_font16,x_offs+40,180,100,32,5));
		TGLinterface::add_element(new TGLbutton("Attractor",m_font16,x_offs+40,220,100,32,6));
		TGLinterface::add_element(new TGLbutton("Menu",m_font16,x_offs+40,260,100,32,7));

        
        // screen:
		m_configure_fullscreen=new TGLbutton("Full Screen",m_font16,x_offs+440,20,160,32,10);
		m_configure_window=new TGLbutton("Windowed",m_font16,x_offs+440,60,160,32,11);
		TGLinterface::add_element(m_configure_fullscreen);
		TGLinterface::add_element(m_configure_window);

//        TGLinterface::add_element(new TGLbutton("4x3",m_font16,x_offs+450,130,80,20,21));
//        TGLinterface::add_element(new TGLbutton("16x10",m_font16,x_offs+450,160,80,20,22));
//        TGLinterface::add_element(new TGLbutton("16x9",m_font16,x_offs+450,190,80,20,23));
        
        TGLinterface::add_element(new TGLText("Zoom",m_font16,x_offs+520,230,true));
        m_configure_zoom=new TGLslider(x_offs+440,240,160,32,16,32,50);
        ((TGLslider *)m_configure_zoom)->m_value=float((m_player_profile->m_zoom-MIN_ZOOM)/(MAX_ZOOM-MIN_ZOOM));
        TGLinterface::add_element(m_configure_zoom);
        
		TGLinterface::add_element(new TGLText("Music Volume",m_font16,x_offs+520,330,true));
		TGLinterface::add_element(new TGLText("SFX Volume",m_font16,x_offs+520,410,true));
		m_configure_music_volume=new TGLslider(x_offs+440,340,160,32,16,32,12);
		((TGLslider *)m_configure_music_volume)->m_value=float(m_player_profile->m_music_volume)/float(MIX_MAX_VOLUME);
		m_configure_sfx_volume=new TGLslider(x_offs+440,420,160,32,16,32,13);
		((TGLslider *)m_configure_sfx_volume)->m_value=float(m_player_profile->m_sfx_volume)/float(MIX_MAX_VOLUME);
		TGLinterface::add_element(m_configure_music_volume);
		TGLinterface::add_element(m_configure_sfx_volume);

		if (m_player_profile->m_fullscreen) {
			m_configure_fullscreen->m_enabled=false;
			m_configure_window->m_enabled=true;
		} else {
			m_configure_fullscreen->m_enabled=true;
			m_configure_window->m_enabled=false;
		} // if 
	
		m_configure_key_to_change=-1;
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
			m_state_fading=2;
			m_state_fading_cycle=0;
			m_state_selection=ID;
		} // if 

		if (ID>=1 && ID<=NUMBER_OF_REDEFINABLE_KEYS) m_configure_key_to_change=ID-1;

		if (ID>=10) {
			switch(ID) {
			case 10:m_player_profile->m_fullscreen=true;
					break;
			case 11:m_player_profile->m_fullscreen=false;
					break;
			case 12:m_player_profile->m_music_volume=int(((TGLslider *)m_configure_music_volume)->m_value*MIX_MAX_VOLUME);
					break;
			case 13:m_player_profile->m_sfx_volume=int(((TGLslider *)m_configure_sfx_volume)->m_value*MIX_MAX_VOLUME);
					break;
                    
//            case 21:m_player_profile->m_resolution_x = 640;
//                    m_player_profile->m_resolution_y = 480;
//                    m_configure_refresh_gui = true;
//                    break;
//            case 22:m_player_profile->m_resolution_x = 800;
//                    m_player_profile->m_resolution_y = 500;
//                    m_configure_refresh_gui = true;
//                    break;
//            case 23:m_player_profile->m_resolution_x = 1024;
//                    m_player_profile->m_resolution_y = 576;
//                    m_configure_refresh_gui = true;
//                    break;
                    
            case 50:m_player_profile->m_zoom=MIN_ZOOM + ((TGLslider *)m_configure_zoom)->m_value*(MAX_ZOOM - MIN_ZOOM);
                    break;
                    
            case 100: // QUIT
                    m_state_fading = 3; // quit
                    m_state_fading_cycle = 0;
                    m_state_selection = ID;
                    break;
			} // switch 

			if (m_player_profile->m_fullscreen) {
				m_configure_fullscreen->m_enabled=false;
				m_configure_window->m_enabled=true;
			} else {
				m_configure_fullscreen->m_enabled=true;
				m_configure_window->m_enabled=false;
			} // if 

			save_playerprofile();
		} // if 

		if (m_configure_key_to_change!=-1) {
			int i;

			for(i=0;i<SDL_NUM_SCANCODES && m_configure_key_to_change!=-1;i++) {
				if (k->key_press(i)) {
					m_player_profile->m_keys_configuration[0][m_configure_key_to_change]=i;
					m_configure_key_to_change=-1;
				} // if 
			} // if 

			save_playerprofile();
		} // if 
	}

	if ((m_state_fading==2 || m_state_fading==3) && m_state_fading_cycle>25) {
		switch(m_state_selection) {
		case 0:
				return m_configure_previous_state;
        case 100:
                return TGL_STATE_POSTGAME;
		} // switch
	} // if 

	if (m_state_fading==0 || m_state_fading==2 || m_state_fading==3) m_state_fading_cycle++;
	if (m_state_fading==0 && m_state_fading_cycle>25) m_state_fading=1;


	return TGL_STATE_CONFIGURE;
} /* TGLapp::configure_cycle */ 


void TGLapp::configure_draw(void)
{

	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    float f1 = 1.0f;
    float f2 = 1.0f;
	switch(m_state_fading) {
	case 0:	
            f1 = abs(m_state_fading_cycle)/25.0F;
            f2 = f1/2.0f;
			break;
	case 1:
            f1 = 1.0f;
            f2 = 0.5f;
            break;
	case 2:
            f1 = abs(25-m_state_fading_cycle)/25.0F;
            f2 = f1/2.0f;
            break;
    case 3:
            f1 = abs(25-m_state_fading_cycle)/25.0F;
            f2 = 1.0-(f1/2.0f);
            break;
    } // switch

    if (f1<0) f1 = 0;
    if (f1>1) f1 = 1;
    if (f2<0) f2 = 0;
    if (f2>1) f2 = 1;
    if (m_configure_previous_state == TGL_STATE_GAME) {
        game_draw();
        fade_in_alpha(f2);
    }
    TGLinterface::draw(f1);
    
    {
        int x_offs = (SCREEN_X-640)/2;
        int i=0;
        char tmp[256];
        
        for(i=0;i<NUMBER_OF_REDEFINABLE_KEYS;i++) {
            
            if (m_configure_key_to_change==i) {
                TGLinterface::print_left("press a new key",m_font16,x_offs+160,float(44+i*40),1,0,0,f1);
            } else {
                
                strcpy(tmp,SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)m_player_profile->m_keys_configuration[0][i])));
                //				strcpy(tmp,SDL_GetKeyName((SDLKey)(m_player_profile->m_keys_configuration[0][i])));
                TGLinterface::print_left(tmp,m_font16,x_offs+160,float(44+i*40),1,1,1,f1);
            } // if 
        } // if 
    }
    
} /* TGLapp::configure_draw */ 

