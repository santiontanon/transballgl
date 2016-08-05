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


extern int SCREEN_X,SCREEN_Y;

TGLTileBrowser::TGLTileBrowser(float x,float y,float dx,float dy,int ID) : TGLInterfaceElement()
{
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

} /* TGLTileBrowser::TGLTileBrowser */ 


TGLTileBrowser::~TGLTileBrowser()
{
	m_entries.clear();
} /* TGLTileBrowser::~TGLTileBrowser */ 


void TGLTileBrowser::draw(void)
{
	draw(1);
} /* TGLTileBrowser::draw */ 


void TGLTileBrowser::draw(float alpha)
{
	// Compute the height:
	int height = 0;
    for(GLTile *t:m_entries) height+=t->get_dy()*scale(t)+4;

    drawQuad(m_x,m_y, m_dx,m_dy, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x,m_y, m_dx,4, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x+m_dx-4,m_y, 4,m_dy, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x,m_y, 4,m_dy, 0, 0, 0, alpha*0.5f);
    drawQuad(m_x,m_y+m_dy-4, m_dx,4, 0, 0, 0, alpha*0.5f);

	// Draw the slider:
	{
		if (m_entries.size()>0) {
			m_slider_height = (m_dy-8)/height;
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

		y = int(int(m_y) - (m_slider_pos/(m_dy-8))*height);

        for(GLTile *entry:m_entries) {
            if (y>=m_y+m_dy) break;
            
			float s = scale(entry);
			
            if (m_mouse_over == i) drawQuad(m_x+6,float(y+4), m_dx, entry->get_dy()*s, 0.5f,1,0.5f,alpha*0.5f);

			if (m_selected==i) {
                drawLine(m_x+6,float(y+3),
                         m_x+6+float(entry->get_dx()*s+4), float(y+3), 1,1,1,1);
                drawLine(m_x+6+float(entry->get_dx()*s+4), float(y+3),
                         m_x+6+float(entry->get_dx()*s+4), float(y+3+entry->get_dy()*s+2), 1,1,1,1);
                drawLine(m_x+6+float(entry->get_dx()*s+4), float(y+3+entry->get_dy()*s+2),
                         m_x+6,float(y+3+entry->get_dx()*s+2), 1,1,1,1);
                drawLine(m_x+6,float(y+3+entry->get_dy()*s+2),
                         m_x+6,float(y+3), 1,1,1,1);
			} // if

			entry->draw(m_x+8+entry->get_hot_x()*s,float(y+4)+entry->get_hot_y()*s,0,0,s);
			y+=entry->get_dy()*s+4;;
			i++;
		} // while

        glDisable(GL_SCISSOR_TEST);
		glScissor(bb[0],bb[1],bb[2],bb[3]);
	}
} /* TGLTileBrowser::draw */ 


bool TGLTileBrowser::mouse_over(int mousex,int mousey)
{
	if (mousex>=m_x && mousex<m_x+m_dx &&
		mousey>=m_y && mousey<m_y+m_dy) return true;
	return false;
}


bool TGLTileBrowser::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	int height = 0;
    for(GLTile *t:m_entries) height+=t->get_dy()*scale(t)+4;

	if (mousex>m_x && mousex<m_x+(m_dx-20) && mousey>m_y && mousey<m_y+m_dy) {
		int s=-1;
		{
			int i = 0;
			int y = int(int(m_y) - (m_slider_pos/(m_dy-8))*height);
			int new_y;
            for(GLTile *t:m_entries) {
				new_y=y+t->get_dy()+4;
				if (mousey>=y && mousey<new_y) s = i;
				i++;
				y=new_y;
			} // while
		}

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

		return true;
	} // if 

	if (button!=0 && mousex>=m_x && mousey>=m_y && mousex<=m_x+m_dx && mousey<=m_y+m_dy) return true;

	return false;
} /* TGLTileBrowser::check_status */ 


void TGLTileBrowser::clear(void)
{
    for(GLTile *t:m_entries) delete t;
	m_entries.clear();
} /* TGLTileBrowser::clear */ 


void TGLTileBrowser::addEntry(GLTile  *t)
{
	m_entries.push_back(t);
} /* TGLTileBrowser::addEntry */  


GLTile *TGLTileBrowser::getEntry(int i)
{
	return getAt<GLTile *>(m_entries, i);
} /* TGLTileBrowser::addEntry */  


void TGLTileBrowser::deleteEntry(int i)
{
    GLTile *t = getAt<GLTile *>(m_entries, i);
	if (t!=0) m_entries.remove(t);
} /* TGLTileBrowser::addEntry */  



void TGLTileBrowser::setSelected(int i)
{
	if (i<-1) i=-1;
	if (i>=m_entries.size()) i = int(m_entries.size()-1);
	m_selected = i;
	
	int height = 0;
    for(GLTile *t:m_entries) height+=t->get_dy()*scale(t)+4;
	int first_y = int(-(m_slider_pos/(m_dy-8))*height);

	int current = 0;
	int selected_y = 0;
	int selected_height = 0;
    for(GLTile *t:m_entries) {
		if (current == m_selected) {
			selected_height = t->get_dy()+4;

			break;
		}
		current++;
		selected_y=selected_y+t->get_dy()+4;
	} // while

	if (selected_y+first_y < 0) {
		m_slider_pos = (float(selected_y)*(m_dy-8))/height;
	} else if (selected_y+first_y+selected_height > m_dy) {
		m_slider_pos = (float(selected_y+selected_height - (m_dy-8))*(m_dy-8))/height;
	}		
} /* TGLTileBrowser::setSelected */ 


int TGLTileBrowser::getSelected(void)
{
	return m_selected;
} /* TGLTileBrowser::getSelected */ 


int TGLTileBrowser::getNEntries() 
{
	return (int)m_entries.size();
}


float TGLTileBrowser::scale(GLTile *t)
{
	float scale = 1;
	int dx = m_dx - 28;
	if (t->get_dx()>dx) scale = float(dx)/float(t->get_dx());
	return scale;
}

