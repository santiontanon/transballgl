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
#include "SDL/SDL_ttf.h"
#else
#include "SDL_ttf.h"
#endif
#include "SDL_rotozoom.h"


#include "Symbol.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "keyboardstate.h"

#include "TGLapp.h"

#include "TGLinterface.h"


extern SDL_Window *appwindow;

std::list<TGLInterfaceElement *> TGLinterface::m_elements;


void TGLinterface::add_element(TGLInterfaceElement *e)
{
	m_elements.push_back(e);
} /* TGLinterface::add_element */ 


void TGLinterface::remove_element(TGLInterfaceElement *e)
{
	m_elements.remove(e);
} /* TGLinterface::remove_element */ 


void TGLinterface::remove_element(int ID)
{
    for(TGLInterfaceElement *e:m_elements) {
		if (e->m_ID==ID) {
			m_elements.remove(e);
			delete e;
			return;
		} // if
	} // while 
} /* TGLinterface::remove_element */ 



void TGLinterface::substitute_element(TGLInterfaceElement *old,TGLInterfaceElement *e)
{
    for(std::list<TGLInterfaceElement *>::iterator it = m_elements.begin();
        it!=m_elements.end();it++) {
        if (*it == old) {
            *it = e;
            return;
        }
    }
} /* TGLinterface::substitute_element */ 


void TGLinterface::reset(void)
{
    for(TGLInterfaceElement *e:m_elements) delete e;
	m_elements.clear();
} /* TGLinterface::reset */ 


bool TGLinterface::mouse_over_element(int mousex, int mousey)
{
    for(TGLInterfaceElement *e:m_elements) {
		if (e->mouse_over(mousex,mousey)) return true;
	} // while 
	return false;
}


int TGLinterface::update_state(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	int ret_val=-1;
	TGLInterfaceElement *modal=0;
    std::list<TGLInterfaceElement *> to_delete;

	if (k->key_press(SDL_SCANCODE_TAB)) {
		TGLInterfaceElement *found=0;
		bool any_active=false;
		int pos=0;

		// Find if the mouse is currently over one of the active components:
        for(TGLInterfaceElement *e:m_elements) {
            if (found!=0) break;
			if (e->m_active) {
				if (mousex>=e->m_x && mousex<e->m_x+e->m_dx &&
					mousey>=e->m_y && mousey<e->m_y+e->m_dy) {
					found=e;
				} // if 
				any_active=true;
			} // if 
		} // while 

		if (any_active) {
			if (found==0) pos=-1;
					 else pos=indexOf<TGLInterfaceElement *>(m_elements, found);
            TGLInterfaceElement *e = 0;
            do {
                pos++;
                if (pos>=m_elements.size()) pos=0;
                e = getAt<TGLInterfaceElement *>(m_elements, pos);
			} while(!e->m_active);

			SDL_WarpMouseInWindow(appwindow,int(e->m_x+e->m_dx/2),int(e->m_y+e->m_dy/2));
		} // if 
	} // if 

    for(TGLInterfaceElement *e:m_elements) {
        if (modal!=0) break;
		if (e->m_modal==true) modal=e;
	} // while 

	if (modal!=0) {
		if (modal->check_status(mousex,mousey,button,button_status,k)) ret_val=modal->m_ID;
		if (modal->m_to_be_deleted) to_delete.push_back(modal);
	} else {
        for(TGLInterfaceElement *e:m_elements) {
			if (e->check_status(mousex,mousey,button,button_status,k)) ret_val=e->m_ID;
			if (e->m_to_be_deleted) to_delete.push_back(e);
		} // while 
	} // if

    for(TGLInterfaceElement *e:to_delete) {
        m_elements.remove(e);
        delete e;
    }
    to_delete.clear();

	return ret_val;
} /* TGLinterface::update_state */ 


void TGLinterface::draw(void)
{
    for(TGLInterfaceElement *e:m_elements) {
		e->draw();
	} // while 

} /* TGLinterface::draw */ 


void TGLinterface::draw(float alpha)
{
    for(TGLInterfaceElement *e:m_elements) {
		e->draw(alpha);
	} // while 

} /* TGLinterface::draw */ 


void TGLinterface::enable(int ID)
{
    for(TGLInterfaceElement *e:m_elements) {
		if (e->m_ID==ID) e->m_enabled=true;
	} // while 
} /* TGLinterface::enable */  


void TGLinterface::disable(int ID)
{
    for(TGLInterfaceElement *e:m_elements) {
		if (e->m_ID==ID) e->m_enabled=false;
	} // while 
} /* TGLinterface::disable */ 


bool TGLinterface::is_enabled(int ID)
{
    for(TGLInterfaceElement *e:m_elements) {
		if (e->m_ID==ID) return e->m_enabled;
	} // while 

	return false;
} /* TGLinterface::is_enabled */ 


TGLInterfaceElement *TGLinterface::get(int ID)
{
    for(TGLInterfaceElement *e:m_elements) {
		if (e->m_ID==ID) return e;
	} // while 
	return 0;
} /* TGLinterface::get */ 



