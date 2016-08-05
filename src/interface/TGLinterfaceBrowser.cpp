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
#include "SDL_glutaux.h"
#include "GLTile.h"
#include "keyboardstate.h"

#include "TGLapp.h"

#include "TGLinterface.h"


#define ROW_HEIGHT	20

extern int SCREEN_X,SCREEN_Y;

TGLBrowser::TGLBrowser(TTF_Font *font,float x,float y,float dx,float dy,int ID) : TGLInterfaceElement()
{
	m_font = font;

	m_x=x;
	m_y=y;
	m_dx=dx;
	m_dy=dy;
	m_enabled=true;
	m_active=false;
	m_ID = ID;

	m_mouse_over = -1;
	m_selected = -1;

	m_slider_pos = 0.0f;
	m_slider_height = 0.0f;

	m_old_mouse_x = 0;
	m_old_mouse_y = 0;
} /* TGLBrowser::TGLBrowser */ 


TGLBrowser::~TGLBrowser()
{
    clear();
} /* TGLBrowser::~TGLBrowser */ 


void TGLBrowser::draw(void)
{
	draw(1);
} /* TGLBrowser::draw */ 


void TGLBrowser::draw(float alpha)
{
    drawQuad(m_x,m_y, m_dx,m_dy, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x,m_y, m_dx,4, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x+m_dx-4,m_y, 4,m_dy, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x,m_y, 4,m_dy, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x,m_y+m_dy-4, m_dx,4, 0, 0, 0, alpha*0.5f);

	// Draw the slider:
	{
		if (m_entries.size()>0) {
			m_slider_height = (m_dy-8)/(m_entries.size()*ROW_HEIGHT);
			if (m_slider_height>=1) {
				m_slider_pos = 0;
				m_slider_height = -1;
			} else {
				m_slider_height = m_slider_height*(m_dy-8);
			    if (m_slider_pos + m_slider_height>m_dy-8) {
					m_slider_pos = m_dy-8 - m_slider_height;
				} // if
			} // if 

		} else {
			m_slider_pos = 0;
			m_slider_height = -1;
		} // if 

		if (m_slider_height>0) drawQuad(m_x+m_dx-20,m_y+4+m_slider_pos, 14,m_slider_height, 0.5f,1,0.5f,alpha*0.5f);
	}

	// Draw the entries:
	{
        GLint bb[4];
        glGetIntegerv(GL_SCISSOR_BOX, bb);
		glScissor(int(m_x+4),int(SCREEN_Y-(m_y+4+(m_dy-8))),int(m_dx-(8 + 20)),int(m_dy-8));
        glEnable(GL_SCISSOR_TEST);

		int i=0;
		int y;

		y = int(int(m_y) - (m_slider_pos/(m_dy-8))*(m_entries.size()*ROW_HEIGHT));

        for(char *entry:m_entries) {
            if (y>=m_y+m_dy) break;

			if (m_mouse_over == i) drawQuad(m_x+4,float(y+4), m_dx-8,ROW_HEIGHT, 0.5f,1,0.5f,alpha*0.5f);
			
			if (m_selected == i) TGLinterface::print_left(entry,m_font,m_x+8, float(y+ROW_HEIGHT+4),0,1,0,1);
							else TGLinterface::print_left(entry,m_font,m_x+8, float(y+ROW_HEIGHT+4));
			y+=ROW_HEIGHT;
			i++;
		} // while

        glDisable(GL_SCISSOR_TEST);
		glScissor(bb[0],bb[1],bb[2],bb[3]);
	}
} /* TGLBrowser::draw */ 


bool TGLBrowser::mouse_over(int mousex,int mousey)
{
	if (mousex>=m_x && mousex<m_x+m_dx &&
		mousey>=m_y && mousey<m_y+m_dy) return true;
	return false;
}


bool TGLBrowser::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	if (mousex>m_x && mousex<m_x+(m_dx-20) && mousey>m_y && mousey<m_y+m_dy) {
		int starty = int(int(m_y) - (m_slider_pos/(m_dy-8))*(m_entries.size()*ROW_HEIGHT));
		int s = int((mousey-starty)/ROW_HEIGHT);

		if (s<m_entries.size()) {
			m_mouse_over = s;
		} else {
			m_mouse_over = -1;
		} // if 
	} else {
		m_mouse_over = -1;
	} // if 

	if (button_status==1 && mousex>m_x+(m_dx-20) && mousex<m_x+m_dx && mousey>m_y && mousey<m_y+m_dy) {
		float p = (mousey-m_y-(m_slider_height/2))/(m_dy-8-m_slider_height);
		if (p<0) p=0;
		if (p>1) p=1;
		m_slider_pos = p*(m_dy-8-m_slider_height);
	} // if

	if (m_mouse_over != -1 && button==1) {
		m_selected = m_mouse_over;
		
		m_old_mouse_x = mousex;
		m_old_mouse_y = mousey;

		return true;
	} // if 

	m_old_mouse_x = mousex;
	m_old_mouse_y = mousey;

	return false;
} /* TGLBrowser::check_status */ 


void TGLBrowser::clear(void)
{
    for(char *tmp:m_entries) delete []tmp;
	m_entries.clear();
} /* TGLBrowser::clear */ 


void TGLBrowser::addEntry(char *e)
{
	char *tmp = new char[strlen(e)+1];
	strcpy(tmp,e);
	m_entries.push_back(tmp);
} /* TGLBrowser::addEntry */  


char *TGLBrowser::getEntry(int i)
{
	return getAt<char *>(m_entries, i);
} /* TGLBrowser::addEntry */  


void TGLBrowser::deleteEntry(int i)
{
	char *tmp = getAt<char *>(m_entries, i);
	m_entries.remove(tmp);
	delete []tmp;
} /* TGLBrowser::addEntry */  



void TGLBrowser::setSelected(int i)
{
	if (i<-1) i=-1;
	if (i>=m_entries.size()) i = (int)m_entries.size()-1;
	m_selected = i;
} /* TGLBrowser::setSelected */ 


int TGLBrowser::getSelected(void)
{
	return m_selected;
} /* TGLBrowser::getSelected */ 


int TGLBrowser::getNEntries() 
{
	return (int)m_entries.size();
}

