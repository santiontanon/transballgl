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

TGLConfirmation::TGLConfirmation(const char *message,TTF_Font *font,float x,float y,int ID,bool cancelOption) : TGLInterfaceElement()
{
	m_font = font;
	{
		char tmp[256];
		int i,j=0;
		for(i=0;message[i]!=0;i++) {
			if (message[i]=='\n') {
				tmp[j]=0;
				char *tmp2 = new char[strlen(tmp)+1];
				strcpy(tmp2,tmp);
				m_message_lines.push_back(tmp2);
				j = 0;
			} else {
				tmp[j++] = message[i];
			}
		}
		if (j!=0) {
			tmp[j]=0;
			char *tmp2 = new char[strlen(tmp)+1];
			strcpy(tmp2,tmp);
			m_message_lines.push_back(tmp2);
		}
	}
	
	m_x=x;
	m_y=y;
	m_ID=ID;
	m_enabled=true;
	m_active=true;
	m_modal=true;
	m_state=0;
	m_cycle=0;

	if (cancelOption) {
		m_ok_button = new TGLbutton("ok",font,x-100,y+10,80,32,-1);
		m_cancel_button = new TGLbutton("cancel",font,x+20,y+10,80,32,-1);
	} else {
		m_ok_button = new TGLbutton("ok",font,x-40,y+10,80,32,-1);
		m_cancel_button = NULL;
	}
    
    createOpenGLBuffers();
} /* TGLConfirmation::TGLConfirmation */ 


void TGLConfirmation::createOpenGLBuffers(void)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat *g_vertex_buffer_data = new GLfloat[3*4]{
        0,0,0,
        (GLfloat)SCREEN_X,0,0,
        (GLfloat)SCREEN_X,(GLfloat)SCREEN_Y,0,
        0,(GLfloat)SCREEN_Y,0,
    };
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*4, g_vertex_buffer_data, GL_STATIC_DRAW);
    delete []g_vertex_buffer_data;
}


TGLConfirmation::~TGLConfirmation()
{
	delete m_ok_button;
	delete m_cancel_button;

    for(char *tmp:m_message_lines) delete []tmp;
    m_message_lines.clear();
} /* TGLConfirmation::~TGLConfirmation */ 


void TGLConfirmation::draw(void)
{
	draw(1);
} /* TGLConfirmation::draw */ 


void TGLConfirmation::draw(float alpha)
{
	if (!m_enabled) alpha*=0.33f;

	if (m_state==0 && m_cycle<25) alpha*=(m_cycle/25.0f);
	if (m_state==1) {
		if (m_cycle<25) {
			alpha*=((25-m_cycle)/25.0f);
		} else {
			alpha=0;
		} // if 
	} // if

    
    float f = alpha*0.75f;
    drawQuad(0,0,SCREEN_X,SCREEN_Y,0,0,0,f);

	int y = m_y - TTF_FontHeight(m_font)*m_message_lines.size();
    for(char *message:m_message_lines) {
		TGLinterface::print_center(message,m_font,m_x,y);
		y+=TTF_FontHeight(m_font);
	}

	m_ok_button->draw(alpha);
	if (m_cancel_button!=NULL) m_cancel_button->draw(alpha);

} /* TGLConfirmation::draw */ 



bool TGLConfirmation::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	if (!m_enabled) {
		return false;
	} // if

	m_cycle++;

	if (m_ok_button->check_status(mousex,mousey,button,button_status,k)) {
		m_state = 1;
		m_cycle=0;
		return true;
	} // if 
	if (m_cancel_button!=NULL && m_cancel_button->check_status(mousex,mousey,button,button_status,k)) {
		m_state = 1;
		m_cycle=0;
	} // if 

	if (m_state == 1 && m_cycle>25) m_to_be_deleted = true;

	return false;
} /* TGLConfirmation::check_status */ 



