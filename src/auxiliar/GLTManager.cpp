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

#include "SDL.h"
#ifdef __EMSCRIPTEN__
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#include <glm.hpp>
#include <ext.hpp>
#else
#include "SDL_image.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#endif
#include "auxiliar.h"
#include "SDL_glutaux.h"


#include <vector>
#include "sound.h"
#include "keyboardstate.h"
#include "BB2D.h"
#include "Symbol.h"
#include "GLTile.h"
#include "GLTManager.h"

GLTManagerNode::GLTManagerNode(GLTile *a_tile, Symbol *a_name, GLTManagerImageNode *a_source, int a_x, int a_y, int a_dx, int a_dy) {
	m_tile = a_tile;
	m_name = a_name;
	m_sourceImage = a_source;
	m_x = a_x;
	m_y = a_y;
	m_dx = a_dx;
	m_dy = a_dy;
} // GLTManagerNode::GLTManagerNode


GLTManagerNode::~GLTManagerNode()
{
	if (m_tile!=0) delete m_tile;
	m_tile=0;
} // GLTManagerNode::~GLTManagerNode 


GLTManagerImageNode::GLTManagerImageNode(SDL_Surface *a_surface, Symbol *a_fileName)
{
	m_surface = a_surface;
	m_fileName = a_fileName;
    
    m_tex = createTexture(a_surface,&m_tx,&m_ty);
    
} // GLTManagerImageNode::GLTManagerImageNode


GLTManagerImageNode::~GLTManagerImageNode() 
{
    glDeleteTextures(1,&m_tex);
	SDL_FreeSurface(m_surface);
	m_surface = 0;
	delete m_fileName;
	m_fileName = 0;
} //GLTManagerImageNode::~GLTManagerImageNode


GLTManager::GLTManager()
{
	m_hash=new std::vector<GLTManagerNode *>[SYMBOL_HASH_SIZE];
	m_image_hash=new std::vector<GLTManagerImageNode *>[SYMBOL_HASH_SIZE];

	output_debug_message("GLTManager created.\n");

} /* GLTManager::GLTManager */ 


GLTManager::~GLTManager()
{
	clear();
	delete []m_hash;
	m_hash=0;
	delete []m_image_hash;
	m_image_hash=0;
} /* GLTManager::~GLTManager */ 


void GLTManager::clear(void)
{
	int h;

	output_debug_message("GLTManager: Clearing tile cache...\n");
	for(h=0;h<SYMBOL_HASH_SIZE;h++) {
		for(GLTManagerNode *n:m_hash[h]) {
			delete n;
		}
		m_hash[h].clear();
		for(GLTManagerImageNode *n:m_image_hash[h]) {
			delete n;
		}
		m_image_hash[h].clear();
	}
	output_debug_message("GLTManager: Clearing tile cache... OK\n");

} /* GLTManager::clear */ 


void GLTManager::clearOpenGLState()
{
//	int h;

	output_debug_message("GLTManager: clearOpenGLState...\n");
/*
    for(h=0;h<SYMBOL_HASH_SIZE;h++) {
		for(GLTManagerNode *n:m_hash[h]) {
			delete n;
		}
		m_hash[h].clear();
	}
 */
    clear();
	output_debug_message("GLTManager: clearOpenGLState... OK\n");

}


GLTile *GLTManager::get(const char *name)
{
//	output_debug_message("GLTManager: get(char *): %s\n", name);
	Symbol *s=new Symbol(name);
	GLTile *t=get(s);
	delete s;
	return t;
} /* GLTManager::get */ 

GLTile *GLTManager::get(Symbol *name) 
{
	return get(name,0,0,-1,-1);
}


GLTile *GLTManager::get(const char *name, int x, int y, int dx, int dy)
{
	Symbol *s=new Symbol(name);
	GLTile *t=get(s,x,y,dx,dy);
	delete s;
	return t;
}


GLTile *GLTManager::get(Symbol *name, int x, int y, int dx, int dy)
{
	char tmp[256];
    if (dx==-1 || dy==-1) {
        sprintf(tmp,"%s",name->get());
    } else {
        sprintf(tmp,"%s-%i-%i-%i-%i",name->get(),x,y,dx,dy);
    }
	Symbol *tileName = new Symbol(tmp);

	int h1=tileName->hash_function();
	for(GLTManagerNode *n:m_hash[h1]) {
		if (n->m_name->cmp(tileName)) {
            delete tileName;
			return n->m_tile;
		}
	} // while 

	int h2=name->hash_function();
	GLTManagerImageNode *image = 0;
	for(GLTManagerImageNode *n:m_image_hash[h2]) {
		if (n->m_fileName->cmp(name)) {
			image = n;
			break;
		}
	} // while 

	// load the tile:
	if (image==0) {
//		output_debug_message("GLTManager: loading tile %s...\n", filename);
		SDL_Surface *img=IMG_Load(name->get());
		image = new GLTManagerImageNode(img, new Symbol(name));
		m_image_hash[h2].push_back(image);
	}

	if (dx<=0) dx = image->m_surface->w;
	if (dy<=0) dy = image->m_surface->h;

	GLTile *t;
//	output_debug_message("GLTManager: creating tile %i,%i - %i,%i...\n", x,y,dx,dy);
//	t=new GLTile(image->m_surface,x,y,dx,dy);
    t=new GLTile(image,x,y,dx,dy);
	m_hash[h1].push_back(new GLTManagerNode(t, tileName, image, x, y, dx, dy));

    char filename[512];
	float fx=1.0f,fy=1.0f;
    strcpy(filename,name->get());
    filename[strlen(filename)-4] = '.';
    filename[strlen(filename)-3] = 'n';
    filename[strlen(filename)-2] = 'f';
    filename[strlen(filename)-1] = 'o';
    {
		FILE *fp;
		fp=fopen(filename,"rb");
		if (fp!=0) {
			char tmp[80],tmp2[80];
			while(!feof(fp)) {
				fgets(tmp,80,fp);
				if (1==sscanf(tmp,"%s",tmp2)) {
					if (strcmp(tmp2,"hs")==0 ||
						strcmp(tmp2,"HS")==0) {
						int hx,hy;
						sscanf(tmp,"%s %i %i",tmp2,&hx,&hy);
						t->set_hotspot((int)((hx/fx)+0.5F),(int)((hy/fy)+0.5F));
					} // if 
					if (strcmp(tmp2,"bb")==0 ||
						strcmp(tmp2,"BB")==0) {
						int x1,y1,x2,y2;
						sscanf(tmp,"%s %i %i %i %i",tmp2,&x1,&y1,&x2,&y2);
						t->set_bb((int)((x1/fx)+0.5F),(int)((y1/fy)+0.5F),(int)((x2/fx)+0.5F),(int)((y2/fy)+0.5F));
					} // if 
				} // if 
			} // while 
			fclose(fp);
		} // if 
	}
	output_debug_message("GLTManager: done %p\n",t);
	return t;
} // GLTManager::get 


SDL_Surface *GLTManager::getImage(const char *name)
{
	Symbol *s=new Symbol(name);
	SDL_Surface *t=getImage(s);
	delete s;
	return t;	
}


SDL_Surface *GLTManager::getImage(Symbol *name)
{
	int h2=name->hash_function();

//	output_debug_message("GLTManager::getImage %s -> %i\n",name->get(),h2);

	GLTManagerImageNode *image = 0;
	for(GLTManagerImageNode *n:m_image_hash[h2]) {
		if (n->m_fileName->cmp(name)) {
			return n->m_surface;
			break;
		}
	} // while 	

//	output_debug_message("GLTManager::getImage loading...\n");

	char filename[256];
	sprintf(filename,"%s",name->get());
	SDL_Surface *img=IMG_Load(filename);
	image = new GLTManagerImageNode(img, new Symbol(name));
	m_image_hash[h2].push_back(image);

	output_debug_message("GLTManager::getImage OK: %p - %p\n",img, image);

	return img;
}



char *GLTManager::get_name(GLTile *tile)
{
	int i;

	for(i=0;i<SYMBOL_HASH_SIZE;i++) {
		for(GLTManagerNode *n:m_hash[i]) {
			if (n->m_tile==tile) return n->m_name->get();
		} // while
	} // for

	return 0;
} // GLTManager::get_name 

