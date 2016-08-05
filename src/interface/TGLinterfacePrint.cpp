#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"
#include <list>

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
#include "SDL_rotozoom.h"


#include "Symbol.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "keyboardstate.h"

#include "debug.h"

#include "TGLapp.h"

#include "TGLinterface.h"


#define TEXT_TILE_BUFFER_SIZE	100

class PrintBuffer {
public:
	~PrintBuffer() {
		delete []text;
		delete tile;
	} //

	TTF_Font *font;
	char *text;
	GLTile *tile;
};

// This stores the last text messages we printed so that we are not generating new tiles all the time (and save CPU time):
std::list<PrintBuffer *> text_tile_buffer;

void TGLinterface::clear_print_cache(void)
{
#ifdef __DEBUG_MESSAGES
		output_debug_message("TGLinterface: Clearing print cache... showing textures used:\n");
		{
            for(PrintBuffer *pb:text_tile_buffer) {
				output_debug_message("%i -> '%s'\n",int(pb->tile->get_texture()),pb->text);
			} // while 
		}
#endif

    for(PrintBuffer *pb:text_tile_buffer) delete pb;
	text_tile_buffer.clear();
} /* TGLinterface::clear_print_cache */ 



GLTile *TGLinterface::get_text_tile(const char *text,TTF_Font *font)
{
	PrintBuffer *pb;

    for(PrintBuffer *pb:text_tile_buffer) {
		if (pb->font==font && strcmp(pb->text,text)==0) {
			// Move it the text to the top of the list to indicate that it has been used:
			text_tile_buffer.remove(pb);
			text_tile_buffer.push_front(pb);
			return pb->tile;
		} // if 
	} // if 

//#ifdef __DEBUG_MESSAGES
//		output_debug_message("New text message '%s', current number ot text tiles: %i\n",text,text_tile_buffer.size());
//#endif

	while(text_tile_buffer.size()>=TEXT_TILE_BUFFER_SIZE) {
		pb = text_tile_buffer.back();
		delete pb;
        text_tile_buffer.pop_back();
	} // while 

	{
		GLTile *tile;
		SDL_Surface *sfc;
		SDL_Color c;
		c.r=255;
		c.g=255;
		c.b=255;

		sfc=TTF_RenderText_Blended(font,text,c);
        if (sfc==0) return 0;
		tile=new GLTile(sfc);

		pb=new PrintBuffer();
		pb->font=font;
		pb->text=new char[strlen(text)+1];
		strcpy(pb->text,text);
		pb->tile=tile;
		text_tile_buffer.push_front(pb);

		return tile;
	}

} /* get_text_tile */ 


void TGLinterface::print_left(const char *text,TTF_Font *font,float x,float y)
{
	GLTile *tile;

    if (text[0]==0) return;
    tile=get_text_tile(text,font);
	tile->set_hotspot(0,tile->get_dy());
	tile->draw(x,y,0,0,1);
} /* TGLinterface::print_left */ 


void TGLinterface::print_center(const char *text,TTF_Font *font,float x,float y)
{
	GLTile *tile;

    if (text[0]==0) return;
    tile=get_text_tile(text,font);
	tile->set_hotspot(tile->get_dx()/2,tile->get_dy());
	tile->draw(x,y,0,0,1);
} /* TGLinterface::print_center */ 


void TGLinterface::print_left(const char *text,TTF_Font *font,float x,float y,float r,float g,float b,float a)
{
	GLTile *tile;

    if (text[0]==0) return;
	tile=get_text_tile(text,font);
	tile->set_hotspot(0,tile->get_dy());
	tile->draw(r,g,b,a,x,y,0,0,1);
} /* TGLinterface::print_left */ 


void TGLinterface::print_center(const char *text,TTF_Font *font,float x,float y,float r,float g,float b,float a)
{
	GLTile *tile;

    if (text[0]==0) return;
    tile=get_text_tile(text,font);
	tile->set_hotspot(tile->get_dx()/2,tile->get_dy());
	tile->draw(r,g,b,a,x,y,0,0,1);
} /* TGLinterface::print_center */ 


void TGLinterface::print_centered(const char *text,TTF_Font *font,float x,float y,float r,float g,float b,float a,float angle,float scale)
{
	GLTile *tile;

    if (text[0]==0) return;
    tile=get_text_tile(text,font);
	tile->set_hotspot(tile->get_dx()/2,tile->get_dy()/2);
	tile->draw(r,g,b,a,x,y,0,angle,scale);
} /* TGLinterface::print_center */ 


