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


float color1_r=1,color1_g=1,color1_b=1;					// TEXT
float color2_r=0.15f,color2_g=0.15f,color2_b=0.15f;		// FRAME_BG
float color3_r=1,color3_g=1,color3_b=1;					// FRAME_FG

/*
 float color4_r=0.25f,color4_g=0.25f,color4_b=0.25f;		// BUTTON STD
 float color5_r=0.5f,color5_g=0.5f,color5_b=0.5f;		// BUTTON MOUSE
 float color6_r=0.75f,color6_g=0.75f,color6_b=0.75f;		// BUTTON PRESSED
 */


float color4_r=0.16f,color4_g=0.16f,color4_b=0.33f;		// BUTTON STD
float color5_r=0.33f,color5_g=0.33f,color5_b=0.66f;		// BUTTON MOUSE
float color6_r=0.66f,color6_g=0.66f,color6_b=0.85f;		// BUTTON PRESSED


TGLInterfaceElement::TGLInterfaceElement()
{
    m_ID=-1;
    m_enabled=false;
    m_active=false;
    m_modal=false;
    m_to_be_deleted=false;
    m_x=m_y=m_dx=m_dy=0;
    
    createOpenGLBuffers();
} /* TGLInterfaceElement::TGLInterfaceElement */


void TGLInterfaceElement::createOpenGLBuffers(void)
{
    vPositionLocation = UVLocation = -1;
}


TGLInterfaceElement::~TGLInterfaceElement()
{
} /* TGLInterfaceElement::~TGLInterfaceElement */


bool TGLInterfaceElement::mouse_over(int mousex,int mousey)
{
    return false;
}


bool TGLInterfaceElement::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
    return false;
} /* TGLInterfaceElement::check_status */


void TGLInterfaceElement::draw(void)
{
} /* TGLInterfaceElement::draw */


void TGLInterfaceElement::draw(float alpha)
{
} /* TGLInterfaceElement::draw */


std::list<TGLInterfaceElement *> TGLInterfaceElement::getChildren(void)
{
    std::list<TGLInterfaceElement *> tmp;
    return tmp;
}


TGLText::TGLText(const char *text,TTF_Font *font,float x,float y,bool centered)
{
    m_centered=centered;
    m_text=new char[strlen(text)+1];
    strcpy(m_text,text);
    m_font=font;
    m_x=x;
    m_y=y;
    m_enabled=true;
    m_active=false;
} /* TGLText::TGLText */


TGLText::TGLText(const char *text,TTF_Font *font,float x,float y,bool centered,int ID) : TGLInterfaceElement()
{
    m_ID = ID;
    m_centered=centered;
    m_text=new char[strlen(text)+1];
    strcpy(m_text,text);
    m_font=font;
    m_x=x;
    m_y=y;
    m_enabled=true;
    m_active=false;
} /* TGLText::TGLText */


TGLText::~TGLText()
{
    delete []m_text;
    m_text=0;
    
} /* TGLText::~TGLText */


void TGLText::draw(void)
{
    if (m_centered) {
        if (m_enabled) TGLinterface::print_center(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2,color1_r,color1_g,color1_b,1);
        else TGLinterface::print_center(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2,0.33f*color1_r,0.33f*color1_g,0.33f*color1_b,1);
    } else {
        if (m_enabled) TGLinterface::print_left(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2);
        else TGLinterface::print_left(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2,0.33f,0.33f,0.33f,1);
    } // if
    
} /* TGLText::draw */


void TGLText::draw(float alpha)
{
    if (m_centered) {
        if (m_enabled) TGLinterface::print_center(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2,color1_r,color1_g,color1_b,alpha);
        else TGLinterface::print_center(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2,0.33f*color1_r,0.33f*color1_g,0.33f*color1_b,alpha);
    } else {
        if (m_enabled) TGLinterface::print_left(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2,color1_r,color1_g,color1_b,alpha);
        else TGLinterface::print_left(m_text,m_font,m_x,m_y+TTF_FontHeight(m_font)/2,0.33f*color1_r,0.33f*color1_g,0.33f*color1_b,alpha);
    } // if
    
} /* TGLText::draw */


void TGLText::set_text(char *text)
{
    if (m_text!=0) 	delete []m_text;
    m_text=new char[strlen(text)+1];
    strcpy(m_text,text);
} /* TGLText::set_text */


TGLbutton::TGLbutton(const char *text,TTF_Font *font,float x,float y,float dx,float dy,int ID)  : TGLInterfaceElement()
{
    m_text=new char[strlen(text)+1];
    strcpy(m_text,text);
    m_font=font;
    m_x=x;
    m_y=y;
    m_dx=dx;
    m_dy=dy;
    m_ID=ID;
    m_status=0;
    m_enabled=true;
    m_tile=0;
    m_active=true;
    
    createOpenGLBuffers();
} /* TGLbutton::TGLbutton */


TGLbutton::TGLbutton(GLTile *icon,float x,float y,float dx,float dy,int ID)  : TGLInterfaceElement()
{
    m_text=0;
    m_font=0;
    m_x=x;
    m_y=y;
    m_dx=dx;
    m_dy=dy;
    m_ID=ID;
    m_status=0;
    m_enabled=true;
    m_tile=icon;
    m_active=true;
    
    createOpenGLBuffers();
} /* TGLbutton::TGLbutton */


void TGLbutton::createOpenGLBuffers(void)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat *g_vertex_buffer_data = new GLfloat[36]{
        m_x+3,m_y,0,
        m_x+m_dx-3,m_y,0,
        m_x+m_dx-1,m_y+1,0,
        m_x+m_dx,m_y+3,0,
        m_x+m_dx,m_y+m_dy-3,0,
        m_x+m_dx-1,m_y+m_dy-1,0,
        m_x+m_dx-3,m_y+m_dy,0,
        m_x+3,m_y+m_dy,0,
        m_x+1,m_y+m_dy-1,0,
        m_x,m_y+m_dy-3,0,
        m_x,m_y+3,0,
        m_x+1,m_y+1,0
    };
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*36, g_vertex_buffer_data, GL_STATIC_DRAW);
    //output_debug_message("Button created: '%s' %i %i [%lu]\n",m_text, VertexArrayID, vertexbuffer, sizeof(GLfloat)*36);
    delete []g_vertex_buffer_data;
}


TGLbutton::~TGLbutton()
{
    // Only delete the tile if it has been created here:
    if (m_text!=0) {
        if (m_tile!=0) delete m_tile;
        m_tile=0;
    } // if
    
    if (m_text!=0) delete []m_text;
    m_text=0;
    
   	glDeleteBuffers(1,&vertexbuffer);
#ifdef __APPLE__
   	glDeleteVertexArraysAPPLE(1,&VertexArrayID);
#else
   	glDeleteVertexArrays(1,&VertexArrayID);
#endif
    
} /* TGLbutton::~TGLbutton */


void TGLbutton::draw(void)
{
    draw(1);
} /* TGLbutton::draw */


void TGLbutton::draw(float alpha)
{
    if (!m_enabled) alpha/=3;
    
    switch(m_status) {
        case 1: glUniform4f(inColorID, color5_r,color5_g,color5_b,alpha);
            break;
        case 2: glUniform4f(inColorID, color6_r,color6_g,color6_b,alpha);
            break;
        default:glUniform4f(inColorID, color4_r,color4_g,color4_b,alpha);
    } // switch
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    if (vPositionLocation==-1) {
        vPositionLocation = glGetAttribLocation(programID, "vPosition");
        UVLocation = glGetAttribLocation(programID, "UV");
    }
    glUniform1i(useTextureID, 0);
    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(vPositionLocation);
    glEnableVertexAttribArray(UVLocation);
    glVertexAttribPointer(vPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    glDisableVertexAttribArray(vPositionLocation);
    glDisableVertexAttribArray(UVLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    if (m_tile==0) {
        TGLinterface::print_center(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2+(TTF_FontHeight(m_font))/2,color1_r,color1_g,color1_b,alpha);
    } else {
        m_tile->draw(color1_r,color1_g,color1_b,alpha,m_x+m_dx/2,m_y+m_dy/2,0,0,1);
    } // if
} /* TGLbutton::draw */


bool TGLbutton::mouse_over(int mousex,int mousey)
{
    if (mousex>=m_x && mousex<m_x+m_dx &&
        mousey>=m_y && mousey<m_y+m_dy) return true;
    return false;
}


bool TGLbutton::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
    if (!m_enabled) {
        m_status=0;
        return false;
    } // if
    
    if (mousex>=m_x && mousex<m_x+m_dx &&
        mousey>=m_y && mousey<m_y+m_dy) {
        if (button==1) {
            m_status=2;
            return true;
        } else {
            m_status=1;
        } // if
    } else {
        m_status=0;
    } // if
    
    return false;
} /* TGLbutton::check_status */



TGLbuttonTransparent::TGLbuttonTransparent(const char *text,TTF_Font *font,float x,float y,float dx,float dy,int ID) : TGLbutton(text,font,x,y,dx,dy,ID)
{
} /* TGLbuttonTransparent::TGLbuttonTransparent */



void TGLbuttonTransparent::draw(float alpha)
{
    if (!m_enabled) alpha/=3;
    switch(m_status) {
        case 1: TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,0,0,0,alpha*0.5f,0,1.25f);
            TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,1,1,1,alpha*0.5f,0,1.1f);
            TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,1,1,1,alpha,0,1.05f);
            TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,0.75f,0.75f,0.75f,alpha,0,1);
            break;
        case 2: TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,0,0,0,alpha*0.5f,0,1.25f);
            TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,1,1,1,alpha*0.5f,0,1.1f);
            TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,1,1,1,alpha,0,1.05f);
            TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,1,1,1,alpha,0,1);
            break;
        default:TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,0,0,0,alpha*0.5f,0,1.15f);
            TGLinterface::print_centered(m_text,m_font,m_x+m_dx/2,m_y+m_dy/2,0.75f,0.75f,0.75f,alpha,0,1);
            
    } // switch
    
} /* TGLbuttonTransparent::draw */


TGLquad::TGLquad(float x,float y,float dx,float dy, float a_r, float a_g, float a_b, float a_a) : TGLInterfaceElement()
{
    r = a_r;
    g = a_g;
    b = a_b;
    a = a_a;
    m_x=x;
    m_y=y;
    m_dx=dx;
    m_dy=dy;
    m_ID=-1;
    m_enabled=true;
    m_active=false;
    
    createOpenGLBuffers();
    //	output_debug_message("BQuad created");
} // TGLquad::TGLquad


void TGLquad::createOpenGLBuffers(void)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat *g_vertex_buffer_data = new GLfloat[3*4]{
        m_x,m_y,0,
        m_x+m_dx,m_y,0,
        m_x+m_dx,m_y+m_dy,0,
        m_x,m_y+m_dy,0,
    };
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*4, g_vertex_buffer_data, GL_STATIC_DRAW);
    delete []g_vertex_buffer_data;
    
    //	output_debug_message("BQuad::createOpenGLBuffers completed (%g,%g,%g,%g)\n",m_x, m_y, m_dx, m_dy);
}


TGLquad::~TGLquad()
{
   	glDeleteBuffers(1,&vertexbuffer);
#ifdef __APPLE__
   	glDeleteVertexArraysAPPLE(1,&VertexArrayID);
#else
   	glDeleteVertexArrays(1,&VertexArrayID);
#endif
} // BQuad::~BQuad


void TGLquad::draw(void)
{
    draw(1);
} // BFrame::draw


void TGLquad::draw(float alpha)
{
    if (!m_enabled) alpha*=0.5;
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    if (vPositionLocation==-1) {
        vPositionLocation = glGetAttribLocation(programID, "vPosition");
        UVLocation = glGetAttribLocation(programID, "UV");
    }
    glUniform1i(useTextureID, 0);
    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(vPositionLocation);
    glEnableVertexAttribArray(UVLocation);
    glVertexAttribPointer(vPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    
    glUniform4f(inColorID, r, g, b, a*alpha);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableVertexAttribArray(vPositionLocation);
    glDisableVertexAttribArray(UVLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
} // TGLquad::draw


TGLframe::TGLframe(float x,float y,float dx,float dy) : TGLInterfaceElement()
{
    m_x=x;
    m_y=y;
    m_dx=dx;
    m_dy=dy;
    m_ID=-1;
    m_enabled=true;
    m_active=false;
    
    createOpenGLBuffers();
} /* TGLframe::TGLframe */


void TGLframe::createOpenGLBuffers(void)
{
    GLfloat bar_height=6;
    GLfloat new_y = m_y + m_dy - bar_height;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat *g_vertex_buffer_data = new GLfloat[3*36]{
        m_x+2,m_y,0,
        m_x+m_dx-2,m_y,0,
        m_x+m_dx-2,m_y+m_dy,0,
        m_x+2,m_y+m_dy,0,
        
        m_x+2,m_y,0,
        m_x+m_dx-1,m_y,0,
        m_x+m_dx,m_y+1,0,
        m_x+m_dx,m_y+bar_height-1,0,
        m_x+m_dx-1,m_y+bar_height,0,
        m_x+2,m_y+bar_height,0,
        m_x,m_y+bar_height-1,0,
        m_x,m_y+1,0,
        
        m_x+3,m_y,0,
        m_x+m_dx-2,m_y,0,
        m_x+m_dx-1,m_y+1,0,
        m_x+m_dx-1,m_y+bar_height-1,0,
        m_x+m_dx-2,m_y+bar_height,0,
        m_x+3,m_y+bar_height,0,
        m_x+1,m_y+bar_height-1,0,
        m_x+1,m_y+1,0,
        
        m_x+2,new_y,0,
        m_x+m_dx-1,new_y,0,
        m_x+m_dx,new_y+1,0,
        m_x+m_dx,new_y+bar_height-1,0,
        m_x+m_dx-1,new_y+bar_height,0,
        m_x+2,new_y+bar_height,0,
        m_x,new_y+bar_height-1,0,
        m_x,new_y+1,0,
        
        m_x+3,new_y,0,
        m_x+m_dx-2,new_y,0,
        m_x+m_dx-1,new_y+1,0,
        m_x+m_dx-1,new_y+bar_height-1,0,
        m_x+m_dx-2,new_y+bar_height,0,
        m_x+3,new_y+bar_height,0,
        m_x+1,new_y+bar_height-1,0,
        m_x+1,new_y+1,0,
    };
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*36, g_vertex_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    delete []g_vertex_buffer_data;
    
    //	output_debug_message("BFrame::createOpenGLBuffers completed (%g,%g,%g,%g)\n",m_x, m_y, m_dx, m_dy);
}


TGLframe::~TGLframe()
{
   	glDeleteBuffers(1,&vertexbuffer);
#ifdef __APPLE__
   	glDeleteVertexArraysAPPLE(1,&VertexArrayID);
#else
   	glDeleteVertexArrays(1,&VertexArrayID);
#endif
} /* TGLframe::~TGLframe */


void TGLframe::draw(void)
{
    if (!m_enabled) draw(0.5f);
    else draw(1);
} /* TGLframe::draw */


void TGLframe::draw(float alpha)
{
    if (!m_enabled) alpha*=0.5;
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    if (vPositionLocation==-1) {
        vPositionLocation = glGetAttribLocation(programID, "vPosition");
        UVLocation = glGetAttribLocation(programID, "UV");
    }
    glUniform1i(useTextureID, 0);
    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(vPositionLocation);
    glEnableVertexAttribArray(UVLocation);
    glVertexAttribPointer(vPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    
    glUniform4f(inColorID, color2_r,color2_g,color2_b,0.8f*alpha);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glUniform4f(inColorID, color3_r*0.5f,color3_g*0.5f,color3_b*0.5f,alpha);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 8);
    
    glUniform4f(inColorID, color3_r,color3_g,color3_b,alpha);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 8);
    
    glUniform4f(inColorID, color3_r*0.5f,color3_g*0.5f,color3_b*0.5f,alpha);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 8);
    
    glUniform4f(inColorID, color3_r,color3_g,color3_b,alpha);
    glDrawArrays(GL_TRIANGLE_FAN, 28, 8);
    
    glDisableVertexAttribArray(vPositionLocation);
    glDisableVertexAttribArray(UVLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
} /* TGLframe::draw */


TGLslider::TGLslider(float x,float y,float dx,float dy,float slider_dx,float slider_dy,int ID) : TGLInterfaceElement()
{
    m_x=x;
    m_y=y;
    m_dx=dx;
    m_dy=dy;
    m_slider_dx=slider_dx;
    m_slider_dy=slider_dy;
    m_ID=ID;
    m_value=0;
    m_active=true;
    m_enabled=true;
    
    createOpenGLBuffers();
    
} /* TGLslider::TGLslider */


void TGLslider::createOpenGLBuffers(void)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat *g_vertex_buffer_data = new GLfloat[36]{
        m_x+3,m_y+(m_dy/2)-2,0,
        m_x+m_dx-3,m_y+(m_dy/2)-2,0,
        m_x+m_dx-1,m_y+(m_dy/2)-2+1,0,
        m_x+m_dx,m_y+(m_dy/2)-2+3,0,
        m_x+m_dx,m_y+(m_dy/2)-2+4-3,0,
        m_x+m_dx-1,m_y+(m_dy/2)-2+4-1,0,
        m_x+m_dx-3,m_y+(m_dy/2)-2+4,0,
        m_x+3,m_y+(m_dy/2)-2+4,0,
        m_x+1,m_y+(m_dy/2)-2+4-1,0,
        m_x,m_y+(m_dy/2)-2+4-3,0,
        m_x,m_y+(m_dy/2)-2+3,0,
        m_x+1,m_y+(m_dy/2)-2+1,0
    };
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*36, g_vertex_buffer_data, GL_STATIC_DRAW);
    delete []g_vertex_buffer_data;

    glGenVertexArrays(1, &VertexArrayID2);
    glBindVertexArray(VertexArrayID2);
    GLfloat *g_vertex_buffer_data2 = new GLfloat[36]{
        3,0,0,
        m_slider_dx-3,0,0,
        m_slider_dx-1,1,0,
        m_slider_dx,3,0,
        m_slider_dx,m_slider_dy-3,0,
        m_slider_dx-1,m_slider_dy-1,0,
        m_slider_dx-3,m_slider_dy,0,
        3,m_slider_dy,0,
        1,m_slider_dy-1,0,
        0,m_slider_dy-3,0,
        0,3,0,
        1,1,0
    };
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*36, g_vertex_buffer_data2, GL_STATIC_DRAW);
    delete []g_vertex_buffer_data2;

}



TGLslider::~TGLslider()
{
   	glDeleteBuffers(1,&vertexbuffer);
#ifdef __APPLE__
   	glDeleteVertexArraysAPPLE(1,&VertexArrayID);
#else
   	glDeleteVertexArrays(1,&VertexArrayID);
#endif
} /* TGLslider::~TGLslider */


bool TGLslider::mouse_over(int mousex,int mousey)
{
    if (mousex>=m_x && mousex<=m_x+m_dx &&
        mousey>=m_y && mousey<=m_y+m_dy) return true;
    return false;
}


bool TGLslider::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
    if (!m_enabled) return false;
    
    if (button_status!=0) {
        if (mousex>=m_x && mousex<=m_x+m_dx &&
            mousey>=m_y && mousey<=m_y+m_dy) {
            m_value = float(mousex-m_x)/float(m_dx);
            return true;
        } // if
    } // if
    
    return false;
} /* TGLslider::check_status */


void TGLslider::draw(float alpha)
{
    if (!m_enabled) alpha/=3;
    
    if (m_enabled) {
        glUniform4f(inColorID, color4_r*0.8f,color4_g*0.8f,color4_b*0.8f,alpha);
    } else {
        glUniform4f(inColorID, color4_r*0.5f,color4_g*0.5f,color4_b*0.5f,alpha);
    } // switch
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    if (vPositionLocation==-1) {
        vPositionLocation = glGetAttribLocation(programID, "vPosition");
        UVLocation = glGetAttribLocation(programID, "UV");
    }
    glUniform1i(useTextureID, 0);
    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(vPositionLocation);
    glEnableVertexAttribArray(UVLocation);
    glVertexAttribPointer(vPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    glDisableVertexAttribArray(vPositionLocation);
    glDisableVertexAttribArray(UVLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    float x=m_x+(m_value*m_dx)-m_slider_dx/2;
    float y=m_y+(m_dy/2)-m_slider_dy/2;
    if (m_enabled) {
        glUniform4f(inColorID, color4_r,color4_g,color4_b,alpha);
    } else {
        glUniform4f(inColorID, color4_r*0.5f,color4_g*0.5f,color4_b*0.5f,alpha);
    } // switch
    
    glm::mat4 tmp2 = glm::translate(tmp, glm::vec3(x,y,0));
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp2[0][0]);
    glUniform1i(useTextureID, 0);
    glBindVertexArray(VertexArrayID2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glEnableVertexAttribArray(vPositionLocation);
    glEnableVertexAttribArray(UVLocation);
    glVertexAttribPointer(vPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    glDisableVertexAttribArray(vPositionLocation);
    glDisableVertexAttribArray(UVLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

} /* TGLslider::draw */ 


void TGLslider::draw(void)
{
    draw(1);
} /* TGLslider::draw */ 

