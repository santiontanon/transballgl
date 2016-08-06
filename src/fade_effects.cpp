#ifdef _WIN32
#include <windows.h>
#endif

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


void TGLapp::fade_in_alpha(float f)
{
    drawQuad(0, 0, SCREEN_X, SCREEN_Y, 0,0,0,f);
} /* TGLapp::fade_in_alpha */


void TGLapp::fade_in_squares(float f,float size)
{
	int i,j;
	float s=size*f*1.2f;

	for(i=0;i<int(SCREEN_X+size*2);i+=int(size*2)) {
		for(j=0;j<int(SCREEN_Y+size*2);j+=int(size*2)) {
            drawQuad(i-s,j-s, s*2, s*2, 0,0,0,1);
            drawQuad(i+size-s,j+size-s, s*2, s*2, 0,0,0,1);
		} // for
	} // for
} /* TGLapp::fade_in_squares */ 


void TGLapp::fade_in_triangles(float f,float size)
{
	int i,j;
	float s=size*f*1.5f;

	for(i=0;i<SCREEN_X+size*2;i+=int(size*2)) {
		for(j=0;j<SCREEN_Y+size*2;j+=int(size*2)) {
            drawTriangle(float(i),j-s,
                         i+s,j+s,
                         i-s,j+s,
                         0,0,0,1);
            
            drawTriangle(i+size,j+size+s,
                         i+size-s,j+size-s,
                         i+size+s,j+size-s,
                         0,0,0,1);
		} // for
	} // for
} /* TGLapp::fade_in_triangles */ 

