#ifdef _WIN32
#include <windows.h>
#else
#include "unistd.h"
#include "sys/stat.h"
#include "sys/types.h"
#endif

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
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
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif
#include "SDL_glutaux.h"

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

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
extern const char *persistent_data_folder;
#endif


/* Frames per second counter: */ 
extern int frames_per_sec;
extern int frames_per_sec_tmp;
extern int init_time;
extern bool show_fps;
extern bool current_fullscreen;
extern int current_cycle;

extern SDL_Window *appwindow;


TGLapp::TGLapp()
{
	int i;
	
#ifdef _WIN32
	strcpy(m_player_data_path,"");
#else
#ifdef __EMSCRIPTEN__
    sprintf(m_player_data_path,"/%s/",persistent_data_folder);
    EM_ASM(
           try{
           FS.mkdir(Module.persistent_storage_path + "/players");
           FS.mkdir(Module.persistent_storage_path + "/other-players");
           FS.mkdir(Module.persistent_storage_path + "/replays");
           }catch(err)
           {
           }
           );
#else
	{
		char tmp[256];
		output_debug_message("CWD %s\n",getcwd(tmp,256));
	}
	sprintf(m_player_data_path,"%s/.transballGL/",getenv("HOME"));
	system("mkdir ~/.transballGL");
	system("mkdir ~/.transballGL/players");
	system("mkdir ~/.transballGL/other-players");
	system("mkdir ~/.transballGL/replays");
    
    // check if we need to copy over the maps:
    {
        char tmp[512];
        struct stat sb;
        sprintf(tmp,"%smymaps",m_player_data_path);
        if (!(stat(tmp, &sb)==0 && S_ISDIR(sb.st_mode))) {
            system("cp -R mymaps ~/.transballGL/mymaps");
        }
    }
#endif
#endif

	m_font32=TTF_OpenFont("fonts/Krungthep.ttf",28);
	m_font16=TTF_OpenFont("fonts/Krungthep.ttf",14);
	m_ifont32=TTF_OpenFont("fonts/impact.ttf",32);

	m_current_levelpack=0;
	m_selected_level=0;
	m_selected_ship=0;

	m_mm_game=0;
	m_mm_replay=0;

	m_lp_tutorial_game=0;
	m_lp_tutorial_replay=0;
	m_lp_music_channel=-1;

	m_state=TGL_STATE_PLAYERPROFILE;
	m_previous_state = m_state;
	m_state_cycle=0;
	m_state_fading=0;
	m_state_fading_cycle=0;
	m_state_selection=0;

	m_game_state=0;
	m_game_fade_effect=-1;
	m_game_replay_mode=0;	
	m_game_replay=0;
	m_game_previous_state=-1;
	m_game_reinit_previous_state=true;

	m_replay_name_inputframe=0;
	m_replay_save_button=0;
	m_sr_first_replay=0;
	m_replay_rename_button=0;
	m_replay_play_button=0;
	m_replay_delete_button=0;
	m_rb_mouse_over_replay=-1;
	m_rb_replay_selected=-1;

	m_lpb_select_button=0;
	m_lpb_lp_uparrow=0;
	m_lpb_lp_downarrow=0;
	m_lpb_first_lp=0;
	m_lpb_mouse_over_lp=-1;
	m_lpb_lp_selected=-1;

	m_configure_fullscreen = 0;
	m_configure_window = 0;
    m_configure_zoom = 0;
	m_configure_music_volume = 0;
	m_configure_sfx_volume = 0;
    m_configure_refresh_gui = false;

	m_profile_first_profile=0;

	m_highscores_level_pack=0;
	m_highscores_first_ship=0;
	m_highscores_first_level=0;

	m_editor_levelpack=0;
    m_editor_level_editing = 0;
	m_editor_level=0;

	current_fullscreen=false;
	
	m_GLTM=new GLTManager();
	m_SFXM=new SFXManager();
	m_SFXM->cache("sound/sfx");

    output_debug_message("creating OpenGL shaders...\n");
    
    resetOpenGLState();
    
    output_debug_message("done.\n");    
    
	m_player_profile=0;

	m_game=0;

	for(i=0;i<MAXLOCAL_PLAYERS;i++) {
		VirtualController *vc = new VirtualController();
		vc->reset();
		m_lvc.push_back(vc);
	} // for 
	
} /* TGLapp::TGLapp */ 


TGLapp::~TGLapp()
{
	TGLinterface::clear_print_cache();
	m_GLTM->clear();

	TTF_CloseFont(m_font32);
	TTF_CloseFont(m_font16);
	TTF_CloseFont(m_ifont32);

	save_playerprofile();
	
	// save which was the last open player profile:
	if (m_player_profile!=NULL) {
		char tmp[256];
		sprintf(tmp,"%slastplayer",m_player_data_path);
		FILE *fp=fopen(tmp,"w+");
		if (fp!=NULL) {
			fprintf(fp, "%s", m_player_profile->m_name);
			fclose(fp);
		}
	}

	if (m_mm_game!=0) delete m_mm_game;
	m_mm_game=0;
	if (m_mm_replay!=0) delete m_mm_replay;
	m_mm_replay=0;

	if (m_lp_music_channel!=-1) Mix_HaltChannel(m_lp_music_channel);

	if (m_player_profile!=0) delete m_player_profile;
	m_player_profile=0;

	if (m_current_levelpack!=0) delete m_current_levelpack;
	m_current_levelpack=0;

	if (m_game!=0) delete m_game;
	m_game=0;

	if (m_lp_tutorial_game!=0) {
		delete m_lp_tutorial_game;
		m_lp_tutorial_game=0;
	} // if 
	if (m_lp_tutorial_replay!=0) {
		delete m_lp_tutorial_replay;
		m_lp_tutorial_replay=0;
	} // if 

	if (m_game_replay!=0) delete m_game_replay;
	m_game_replay=0;

	if (m_editor_levelpack!=0) delete m_editor_levelpack;
	m_editor_levelpack=0;
    
    if (m_editor_level_editing!=0) delete m_editor_level_editing;
    m_editor_level_editing = 0;

	m_editor_tiles.clear();
	m_editor_object_tiles.clear();
	m_editor_smart_tiles.clear();
	m_editor_smart_tile_palette.clear();

	delete m_GLTM;
	delete m_SFXM;
    
    for(VirtualController *vc:m_lvc) delete vc;
    m_lvc.clear();

} /* TGLapp::TGLapp */ 


bool TGLapp::cycle(KEYBOARDSTATE *k)
{
	int old_state=m_state;
  
#ifdef __DEBUG_MESSAGES
	if (m_state_cycle==0) {
		output_debug_message("First Cycle started for state %i...\n",m_state);
	} // if 
#endif

	switch(m_state) {
	case TGL_STATE_PLAYERPROFILE:m_state=playerprofile_cycle(k);
								break;
	case TGL_STATE_INTRO:m_state=intro_cycle(k);
								break;
	case TGL_STATE_MAINMENU:m_state=mainmenu_cycle(k);
									break;
	case TGL_STATE_CONFIGURE:m_state=configure_cycle(k);
							 break;
	case TGL_STATE_LEVELPACKSCREEN:m_state=levelpackscreen_cycle(k);
									break;
	case TGL_STATE_LEVELPACKBROWSER:m_state=levelpackbrowser_cycle(k);
									break;
	case TGL_STATE_PREGAME:m_state=pregame_cycle(k);
							   break;
	case TGL_STATE_GAME:m_state=game_cycle(k);
							   break;
	case TGL_STATE_POSTGAME:m_state=postgame_cycle(k);
							   break;
	case TGL_STATE_SAVEREPLAY: m_state=savereplay_cycle(k);
							   break;
	case TGL_STATE_REPLAYBROWSER: m_state=replaybrowser_cycle(k);
							   break;
	case TGL_STATE_HIGHSCORES: m_state=highscores_cycle(k);
							   break;
	case TGL_STATE_HIGHSCORES_TIMES: m_state=highscores_times_cycle(k);
							   break;
	case TGL_STATE_EDITOR: m_state=editor_cycle(k);
							   break;
	case TGL_STATE_MAPEDITOR: m_state=mapeditor_cycle(k);
							   break;
	case TGL_STATE_LOADLEVELPACK: m_state=loadlevelpack_cycle(k);
							   break;
	case TGL_STATE_LOADMAP: m_state=loadmap_cycle(k);
							   break;
	case TGL_STATE_SAVEMAP: m_state=savemap_cycle(k);
							   break;
	default:return false;
	} /* switch */ 

	if (old_state==m_state) {
		m_state_cycle++;
	} else {
		if (old_state==TGL_STATE_GAME) {
			if (m_game_reinit_previous_state) {
				m_state_cycle=0;
			} else {
				m_state_cycle=1;
			} // if 
			m_game_reinit_previous_state=true;
		} else {
			m_state_cycle=0;
		} // if 

		m_state_fading=0;
		m_state_fading_cycle=0;

#ifdef __DEBUG_MESSAGES
		output_debug_message("State change: %i -> %i\n",old_state,m_state);
#endif
	} // if 

	m_SFXM->next_cycle();

    if (m_player_profile!=0) {
        current_fullscreen=m_player_profile->m_fullscreen;
        SCREEN_X = m_player_profile->m_resolution_x;
        SCREEN_Y = m_player_profile->m_resolution_y;
    }

	m_previous_state = old_state;

	return true;
} /* TGLapp::cycle */ 


void TGLapp::draw(int SCREEN_X,int SCREEN_Y)
{
    if (m_state_cycle==0) return;
    
    glViewport(0,0,SCREEN_X,SCREEN_Y);
    glClearColor(0,0,0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
#ifndef __EMSCRIPTEN__
    glEnable( GL_TEXTURE_2D );
#endif
    glDisable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // set up model, view and projection matrices:
    glm::mat4x4 Projection = glm::ortho(0.f, (float)SCREEN_X, (float)SCREEN_Y, 0.f, -1.0f, 1.0f);
    glm::mat4 PV = Projection;
    glUniformMatrix4fv(PVMatrixID, 1, GL_FALSE, &PV[0][0]);
    glUniform4f(inColorID, 1.0,1.0,1.0,1.0);
    glUniform1i(useTextureID, 0);
    
    /*
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
*/
    
	switch(m_state) {
	case TGL_STATE_PLAYERPROFILE:playerprofile_draw();
								 break;
	case TGL_STATE_INTRO:intro_draw();
								break;
	case TGL_STATE_MAINMENU:mainmenu_draw();
									break;
	case TGL_STATE_CONFIGURE:configure_draw();
									break;
	case TGL_STATE_LEVELPACKSCREEN:levelpackscreen_draw();
									break;
	case TGL_STATE_LEVELPACKBROWSER:levelpackbrowser_draw();
									break;
	case TGL_STATE_PREGAME:pregame_draw();
							   break;
	case TGL_STATE_GAME:game_draw();
							   break;
	case TGL_STATE_POSTGAME:postgame_draw();
							   break;
	case TGL_STATE_SAVEREPLAY: savereplay_draw();
							   break;
	case TGL_STATE_REPLAYBROWSER: replaybrowser_draw();
							   break;
	case TGL_STATE_HIGHSCORES: highscores_draw();
							   break;
	case TGL_STATE_HIGHSCORES_TIMES: highscores_times_draw();
							   break;
	case TGL_STATE_EDITOR: editor_draw();
							   break;
	case TGL_STATE_MAPEDITOR: mapeditor_draw();
							   break;
	case TGL_STATE_LOADLEVELPACK: loadlevelpack_draw();
							   break;
	case TGL_STATE_LOADMAP: loadmap_draw();
							   break;
	case TGL_STATE_SAVEMAP: savemap_draw();
							   break;
	} // switch 
 	

	if (show_fps) {
		char tmp[80];
        //		sprintf(tmp,"video mem: %.4gmb - fps: %i",float(GLTile::get_memory_used())/float(1024*1024),frames_per_sec);
        sprintf(tmp,"fps: %i",frames_per_sec);
		TGLinterface::print_center(tmp,m_font16,320,460);
	} // if 

	SDL_GL_SwapWindow(appwindow);
} /* TGLapp::draw */ 


void TGLapp::save_playerprofile(void)
{
	FILE *fp;
	char tmp[256];

	if (m_player_profile!=0) {
		sprintf(tmp,"%splayers/%s.pp",m_player_data_path,m_player_profile->m_name);
		fp=fopen(tmp,"w+");
		if (fp!=0) {
			m_player_profile->save(fp);
			fclose(fp);
		} // if 
	} // if
    
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
    
} /* TGLapp::save_playerprofile */


void TGLapp::load_playerprofile(char *name) 
{
	FILE *fp;
	char tmp[256];

	sprintf(tmp,"%splayers/%s.pp",m_player_data_path,name);
	fp=fopen(tmp,"rb");
	if (fp!=0) {
		m_player_profile=new PlayerProfile(fp);
		fclose(fp);
	} else {
		m_player_profile=new PlayerProfile((char *)"default");
	} // if 
} /* TGLapp::load_configuration */ 


void TGLapp::MouseClick(int mousex,int mousey,int button) 
{
	m_mouse_click_x.push_back(mousex);
	m_mouse_click_y.push_back(mousey);
	m_mouse_click_button.push_back(button);
}


void TGLapp::MouseWheel(int mx, int my)
{
    m_mouse_click_x.push_back(mx);
    m_mouse_click_y.push_back(my);
    m_mouse_click_button.push_back(TGL_SCROLLWHEEL_BUTTON);
}



void TGLapp::string_editor_cycle(char *editing,unsigned int *editing_position,unsigned int max_length,KEYBOARDSTATE *k)
{
    for(SDL_Keysym ke:k->keyevents) {
        if (ke.scancode==SDL_SCANCODE_BACKSPACE) {
            if ((*editing_position)>0) {
                strcpy(editing+(*editing_position)-1,editing+(*editing_position));
                (*editing_position)--;
            } // if
        } // if
        
        if (ke.scancode==SDL_SCANCODE_DELETE) {
            if ((*editing_position)<strlen(editing)) {
                strcpy(editing+(*editing_position),editing+(*editing_position)+1);
            } // if
        } // if
        
        if (ke.scancode==SDL_SCANCODE_LEFT) {
            if ((*editing_position)>0) {
                (*editing_position)--;
            } // if
        } // if
        if (ke.scancode==SDL_SCANCODE_RIGHT) {
            if ((*editing_position)<strlen(editing)) {
                (*editing_position)++;
            } // if
        } // if
        
        if (ke.scancode==SDL_SCANCODE_HOME) (*editing_position)=0;
        if (ke.scancode==SDL_SCANCODE_END) (*editing_position)=(unsigned int)strlen(editing);
    } // while
    k->keyevents.clear();
    
    for(SDL_TextInputEvent ti:k->textevents) {
        for(int i = 0;ti.text[i]!=0;i++) {
            if (strlen(editing)<max_length) {
                if ((*editing_position)<strlen(editing)) {
                    for(unsigned int j= (unsigned)strlen(editing)+1;j>(*editing_position);j--) editing[j]=editing[j-1];
                    editing[(*editing_position)]=ti.text[i];
                    (*editing_position)++;
                } else {
                    editing[(*editing_position)]=ti.text[i];
                    (*editing_position)++;
                    editing[(*editing_position)]=0;
                } // if
            }
        }
    }
    k->textevents.clear();
} /* TGLapp::string_editor_cycle */


void TGLapp::clearOpenGLState()
{
    m_GLTM->clearOpenGLState();
    TGLinterface::clear_print_cache();
//    if (m_game!=0) m_game->clearOpenGLState();
}

void TGLapp::resetOpenGLState()
{
    programID = LoadShaders();
    
    glUseProgram(programID);
    PVMatrixID = glGetUniformLocation(programID, "PV");
    MMatrixID = glGetUniformLocation(programID, "M");
    inColorID = glGetUniformLocation(programID, "inColor");
    useTextureID = glGetUniformLocation(programID, "useTexture");
    output_debug_message("PVMatrixID: %i\n",PVMatrixID);
    output_debug_message("MMatrixID: %i\n",MMatrixID);
    output_debug_message("inColorID: %i\n",inColorID);
    output_debug_message("useTextureID: %i\n",useTextureID);
    
//    TGLinterface::createOpenGLBuffers();
    
    GLenum err = glGetError();
    if (err!=GL_NO_ERROR) output_debug_message("glError() = %i\n", err);
    output_debug_message("resetOpenGLState complete\n");
}
