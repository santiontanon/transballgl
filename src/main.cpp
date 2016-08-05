#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"
#include "time.h"

#include "SDL.h"
#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#endif

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
#include "PlayerProfile.h"

#include "debug.h"

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#ifdef __EMSCRIPTEN__
const char *persistent_data_folder="transballgl_persistent_data";
#endif


/*						GLOBAL VARIABLES INITIALIZATION:							*/ 

const char *application_name="Transball GL";
const char *application_version="V1.0";
int SCREEN_X = DEFAULT_SCREEN_X;
int SCREEN_Y = DEFAULT_SCREEN_Y;
int N_SFX_CHANNELS=16;
int COLOUR_DEPTH=32;
#ifdef __EMSCRIPTEN__
bool sound=false;
#else
bool sound=true;
#endif
bool current_fullscreen=false;
SDL_Surface *screen_sfc=0;

TRanrotBGenerator *rg=0;

/* Redrawing constant: */ 
const int REDRAWING_PERIOD=18;
//const int REDRAWING_PERIOD=10;

/* Frames per second counter: */ 
int frames_per_sec=0;
int frames_per_sec_tmp=0;
int init_time=0;
bool show_fps=false;
int current_cycle=0;

SDL_Window *appwindow = 0;
SDL_GLContext glcontext = 0;

// these have to be globals, because of a limitation of emscripten not allowing infinite loops...
bool quit = false;
int game_time,act_time;
KEYBOARDSTATE *k;

TGLapp *app = 0;

void one_iter(void);


/*						AUXILIAR FUNCTION DEFINITION:							*/ 


void toogle_video_mode()
{
    SDL_SetWindowFullscreen(appwindow, (current_fullscreen ? SDL_WINDOW_FULLSCREEN : 0));

    /*
//    // note: this is supposed to work in SDL2, but it does not work on Mac, so...
//    SDL_SetWindowFullscreen(appwindow, (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
    
//    if (app!=0) app->clearOpenGLState();
    
    SDL_DestroyWindow(appwindow);
    appwindow = SDL_CreateWindow(application_name,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SCREEN_X, SCREEN_Y,
                                 (current_fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_OPENGL);
    if (appwindow==0) {
        output_debug_message("Video mode set failed: %s\n",SDL_GetError());
        return;
    } // if
    SDL_GL_MakeCurrent(appwindow, glcontext);
    
    if (app!=0) app->resetOpenGLState();
    
    SDL_ShowCursor(SDL_DISABLE);
    
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    
#ifndef __EMSCRIPTEN__
    float ddpi = 0,hdpi = 0,vdpi = 0;
    if (0!=SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi)) output_debug_message("SDL_GetDisplayDPI failed: %s\n",SDL_GetError());
    output_debug_message("Video mode changed. Desired was %i,%i, actual is %i,%i (DPIs are: %g, %g, %g)\n",SCREEN_X, SCREEN_Y, mode.w, mode.h, ddpi, hdpi, vdpi);
#endif
     */
}


void init_video_mode()
{
    output_debug_message("Initializing video mode\n");
    
    appwindow = SDL_CreateWindow(application_name,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SCREEN_X, SCREEN_Y,
                                 (current_fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_OPENGL);
    if (appwindow==0) {
        output_debug_message("Video mode set failed: %s\n",SDL_GetError());
        return;
    } // if
    glcontext = SDL_GL_CreateContext(appwindow);
    
    LoadShaders();
    
    SDL_ShowCursor(SDL_DISABLE);
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    
    
#ifndef __EMSCRIPTEN__
    float ddpi = 0,hdpi = 0,vdpi = 0;
    if (0!=SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi)) output_debug_message("SDL_GetDisplayDPI failed: %s\n",SDL_GetError());
    output_debug_message("Video mode changed. Desired was %i,%i, actual is %i,%i (DPIs are: %g, %g, %g)\n",SCREEN_X, SCREEN_Y, mode.w, mode.h, ddpi, hdpi, vdpi);
#endif
}


void initialization(int flags)
{
    output_debug_message("Initializing SDL\n");
    
#ifdef __EMSCRIPTEN__
    if(SDL_Init(SDL_INIT_VIDEO)<0) {
#else
        if(SDL_Init(SDL_INIT_VIDEO|(sound ? SDL_INIT_AUDIO:0)|SDL_INIT_JOYSTICK)<0) {
#endif
            output_debug_message("SDL initialization failed: %s\n",SDL_GetError());
            return;
        } /* if */
        
        output_debug_message("SDL initialized\n");
        init_video_mode();
        //    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
        
#ifndef __APPLE__
#ifndef __EMSCRIPTEN__
        glewExperimental = GL_TRUE;
        if (glewInit()!=GLEW_OK) {
            output_debug_message("glewInit failed!\n");
            return;
        }
        if (!GLEW_VERSION_2_1) {
            output_debug_message("glew does not support 2.1 extensions!\n");
            exit(1);
        }
        output_debug_message("glew initialized\n");
#endif
#endif
        
        if (sound) {
            output_debug_message("Initializing Audio\n");
            
            N_SFX_CHANNELS=Sound_initialization(N_SFX_CHANNELS,0);
            
            output_debug_message("Audio initialized\n");
        } /* if */

	TTF_Init();
} /* initialization */


void finalization()
{
	output_debug_message("Finalizing SDL\n");

	delete rg;
	rg=0;

	if (sound) Sound_release();
	TTF_Quit();
	SDL_Quit();

	output_debug_message("SDL finalized\n");

} /* finalization */ 

#ifdef _WIN32
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
#else
int main(int argc, char** argv)
{
#endif

	srand((unsigned int)time(0));
	
	output_debug_message("Application started\n");

#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);
    
    chdir(path);
    output_debug_message("Current path: %s\n",path);
#endif
    
    initialization(current_fullscreen);
    if (appwindow == 0) return 0;
    
#ifdef __EMSCRIPTEN__
    // Emscripten file system stuff:
    {
        char buffer[256];
        sprintf(buffer,"Module.persistent_storage_path='/%s'",persistent_data_folder);
        emscripten_run_script(buffer);
    }
    EM_ASM(
           //create your directory where we keep our persistent data
           FS.mkdir(Module.persistent_storage_path);
           
           //mount persistent directory as IDBFS
           FS.mount(IDBFS,{},Module.persistent_storage_path);
           
           Module.print("start file sync... (" + Module.persistent_storage_path + ")");
           //flag to check when data are synchronized
           Module.syncdone = 0;
           
           //populate persistent_data directory with existing persistent source data
           //stored with Indexed Db
           //first parameter = "true" mean synchronize from Indexed Db to
           //Emscripten file system,
           // "false" mean synchronize from Emscripten file system to Indexed Db
           //second parameter = function called when data are synchronized
           FS.syncfs(true, function(err) {
                assert(!err);
                Module.print("end file sync.");
                Module.syncdone = 1;
            });
           );
    // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
    emscripten_set_main_loop(one_iter, 0, 1);
#else
    while (!quit) {
        one_iter();
        SDL_Delay(1);
    } // while
#endif
    
    return 0;
} // main
    
    
void one_iter(void) {
        
#ifdef __EMSCRIPTEN__
    // if file system is not synced. just wait!
    if (emscripten_run_script_int("Module.syncdone") == 0) return;
#endif
    if (app==0) {
        k=new KEYBOARDSTATE(-1);    // do not allow keyboard repeat
#ifdef __EMSCRIPTEN__
        app = new TGLapp();
#else
        bool fs=current_fullscreen;
        app=new TGLapp();
        if (fs!=current_fullscreen) toogle_video_mode();
#endif
        game_time=init_time=SDL_GetTicks();
    }
    
    bool need_to_redraw	= false;
    SDL_Event event;

    while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
            /* Keyboard event */
            
            /* NICETOHAVE: a keyboard event handler that deals with key combos
               and sets certain status flags to deal with quit/fullscreen nicely */
            case SDL_KEYDOWN:
#ifdef __APPLE__
                // different quit shortcut on OSX: apple+Q
                if (event.key.keysym.scancode==SDL_SCANCODE_Q) {
                    SDL_Keymod modifiers;
                    modifiers = SDL_GetModState();
                    if ((modifiers&KMOD_GUI)!=0) {
                        quit = true;
                    }
                }
#endif
#ifdef _WIN32
                // different quit shortcut on WIN32: ALT+F4
                if (event.key.keysym.scancode==SDL_SCANCODE_F4) {
                    SDL_Keymod modifiers;
                    modifiers=SDL_GetModState();
                    if ((modifiers&KMOD_ALT)!=0) {
                        quit = true;
                    }
                }
#endif
                // default quit: F12
                if (event.key.keysym.scancode==SDL_SCANCODE_F12) {
                    quit = true;
                } /* if */

                
#ifdef __APPLE__
                if (event.key.keysym.scancode==SDL_SCANCODE_F) {
                    SDL_Keymod modifiers;
                    modifiers = SDL_GetModState();
                    if ((modifiers&KMOD_GUI)!=0) {
                        current_fullscreen=(current_fullscreen ? false : true);
                        toogle_video_mode();
                        app->get_player_profile()->m_fullscreen=current_fullscreen;
                        app->save_playerprofile();
                    } // if 
                } // if 
#else 
                if (event.key.keysym.sym==SDL_SCANCODE_RETURN) {
                    SDL_Keymod modifiers;
                    modifiers = SDL_GetModState();
                    if ((modifiers&KMOD_ALT)!=0) {
                        current_fullscreen=(current_fullscreen ? false : true);
                        toogle_video_mode();
                        app->get_player_profile()->m_fullscreen=current_fullscreen;
                        app->save_playerprofile();
                    } // if
                } // if 
#endif
                if (event.key.keysym.scancode==SDL_SCANCODE_F) {
                    SDL_Keymod modifiers;

                    modifiers=SDL_GetModState();

                    if ((modifiers&KMOD_ALT)!=0) {
                        /* Toogle FPS mode: */ 
                        if (show_fps) show_fps=false;
                                 else show_fps=true;
                    } // if 
                } // if 

                /* Keyboard event */ 
                k->keyevents.push_back(event.key.keysym);

                break;
                
            case SDL_TEXTINPUT:
                k->textevents.push_back(event.text);
                break;                    

            case SDL_MOUSEBUTTONDOWN:					
                app->MouseClick(event.button.x,event.button.y,event.button.button);
                break;
            
            case SDL_MOUSEWHEEL:
                if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
#ifdef __APPLE__
                    app->MouseWheel(event.wheel.x,-event.wheel.y);
#else
                    app->MouseWheel(-event.wheel.x,event.wheel.y);
#endif
                } else {
#ifdef __APPLE__
                    app->MouseWheel(-event.wheel.x,event.wheel.y);
#else
                    app->MouseWheel(event.wheel.x,-event.wheel.y);
#endif
                }
                break;

            /* SDL_QUIT event (window close) */
            case SDL_QUIT:
                quit = true;
                break;
                
//				default:
//					output_debug_message("Event: event.type: %i\n",event.type );
        } /* switch */ 
    } /* while */ 

    act_time=SDL_GetTicks();
    if (act_time-game_time>=REDRAWING_PERIOD) {
        int max_frame_step=10;
        do {
            bool fs=current_fullscreen;
            int sx = SCREEN_X, sy = SCREEN_Y;
            game_time+=REDRAWING_PERIOD;
            if ((act_time-game_time)>10*REDRAWING_PERIOD) game_time=act_time;
        
            /* cycle */ 
            k->cycle();
            current_cycle++;
            if (!app->cycle(k)) quit=true;
            need_to_redraw=true;

            k->clearEvents();

            if (fs != current_fullscreen ||
                sx != SCREEN_X ||
                sy != SCREEN_Y) {
                toogle_video_mode();
            }

            act_time=SDL_GetTicks();
            max_frame_step--;
        } while(act_time-game_time>=REDRAWING_PERIOD && max_frame_step>0);

    } // if 

    /* Redraw */ 
    if (need_to_redraw) {
        app->draw(SCREEN_X,SCREEN_Y);
        need_to_redraw=false;
        frames_per_sec_tmp+=1;
    } // if 

    if ((act_time-init_time)>=1000) {
        frames_per_sec=frames_per_sec_tmp;
        frames_per_sec_tmp=0;
        init_time=act_time;
    } // if 
    
    if (quit) {
        delete k;
        k=0;

        delete app;
        app=0;

        Symbol::reset();

        Stop_playback();

        finalization();

        output_debug_message("Application finished\n");
        close_debug_messages();
    }
}
