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
#include <algorithm>

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
//#include "SDL_rotozoom.h"

#include <vector>
#include "sound.h"
#include "keyboardstate.h"
#include "BB2D.h"

#include "GLTile.h"
#include "GLTManager.h"

#define COLLISION_TOLERANCE 0


GLTile::GLTile()
{
    identity = glm::mat4(1.0f);
    hot_x=0;
    hot_y=0;
    
    tile=0;
    dx=0;
    dy=0;
    tex=0;
    
    bb=0;
    
    textureSamplerLocation = vPositionLocation = UVLocation = -1;
} // GLTile::GLTile

GLTile::GLTile(const char *fname)
{
    identity = glm::mat4(1.0f);
    hot_x=0;
    hot_y=0;
    
    tile=0;
    dx=0;
    dy=0;
    tex=0;
    
    bb=0;
    
    textureSamplerLocation = vPositionLocation = UVLocation = -1;
    
    set(fname);
} // GLTile::GLTile

GLTile::GLTile(SDL_Surface *sfc)
{
    identity = glm::mat4(1.0f);
    hot_x=0;
    hot_y=0;
    
    tile=0;
    dx=0;
    dy=0;
    tex=0;
    
    bb=0;
    
    textureSamplerLocation = vPositionLocation = UVLocation = -1;
    
    set(sfc);
} // GLTile::GLTile


GLTile::GLTile(char *fname,int ax,int ay,int adx,int ady)
{
    identity = glm::mat4(1.0f);
    hot_x=0;
    hot_y=0;
    
    tile=0;
    dx=0;
    dy=0;
    tex=0;
    
    bb=0;
    
    textureSamplerLocation = vPositionLocation = UVLocation = -1;
    
    {
        SDL_Surface *sfc=IMG_Load(fname);
        assert(sfc!=0);
        
        if (sfc!=0) {
            SDL_Surface *sfc2=SDL_CreateRGBSurface(SDL_SWSURFACE,adx,ady,32,RMASK,GMASK,BMASK,AMASK);
            SDL_Rect r;
            
            r.x=ax;
            r.y=ay;
            r.w=adx;
            r.h=ady;
            //			SDL_SetSurfaceAlphaMod(sfc, 0);
            SDL_BlitSurface(sfc,&r,sfc2,0);
            set(sfc2);
            SDL_FreeSurface(sfc);
        } // if
    }
} // GLTile::GLTile


GLTile::GLTile(SDL_Surface *sfc,int ax,int ay,int adx,int ady)
{
    identity = glm::mat4(1.0f);
    hot_x=0;
    hot_y=0;
    
    tile=0;
    dx=0;
    dy=0;
    tex=0;
    
    bb=0;
    
    textureSamplerLocation = vPositionLocation = UVLocation = -1;
    
    if (sfc!=0) {
        SDL_Surface *sfc2=SDL_CreateRGBSurface(SDL_SWSURFACE,adx,ady,32,RMASK,GMASK,BMASK,AMASK);
        SDL_Rect r;
        
        r.x=ax;
        r.y=ay;
        r.w=adx;
        r.h=ady;
        //		output_debug_message("GLTile::GLTile: %i,%i - %i,%i...\n", r.x,r.y,r.w,r.h);
        //		SDL_SetSurfaceAlphaMod(sfc, 0);
        SDL_BlitSurface(sfc,&r,sfc2,0);
        set(sfc2);
    } // if
} // GLTile::GLTile


GLTile::GLTile(GLTManagerImageNode *a_tex, int a_x, int a_y, int a_dx, int a_dy)
{
    identity = glm::mat4(1.0f);
    hot_x=0;
    hot_y=0;
    
    tile=0;
    bb = 0;
    textureSamplerLocation = vPositionLocation = UVLocation = -1;
    
    set(a_tex, a_x, a_y, a_dx, a_dy);
}


GLTile::~GLTile()
{
    free();
}  // GLTile::~GLTile


void GLTile::free(void)
{
    if (tile!=0) {
        SDL_FreeSurface(tile);
        tile=0;
        if (free_texture) glDeleteTextures(1,&tex);
        glDeleteBuffers(1,&vertexbuffer);
#ifdef __APPLE__
        glDeleteVertexArraysAPPLE(1,&VertexArrayID);
#else
        glDeleteVertexArrays(1,&VertexArrayID);
#endif
        
    } // if
    
    if (bb!=0) delete bb;
    bb=0;
} // GLTile::free


void GLTile::set(const char *fname)
{
    SDL_Surface *img=IMG_Load(fname);
    set(img);
} // GLTile::set


void GLTile::set(SDL_Surface *sfc)
{
    if (sfc!=0) {
        tile=sfc;
        
        dx=tile->w;
        dy=tile->h;
        tex_coord_x0 = tex_coord_y0 = 0;
        tex=createTexture(tile,&tex_coord_x1,&tex_coord_y1);
        
        glBindVertexArray(0);
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        GLfloat g_vertex_buffer_data[] = {
            0.0f, 0.0f, 0.0f,					tex_coord_x0, tex_coord_y0,
            (GLfloat)dx, 0, 0.0f,				tex_coord_x1, tex_coord_y0,
            (GLfloat)dx, (GLfloat)dy, 0.0f,		tex_coord_x1, tex_coord_y1,
            0.0f,  (GLfloat)dy, 0.0f,			tex_coord_x0, tex_coord_y1,
        };
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
        free_texture = true;
    } // if
    
    compute_bb();
} // GLTile::set


void GLTile::set(GLTManagerImageNode *a_tex, int x, int y, int a_dx, int a_dy)
{
    {
        tile=SDL_CreateRGBSurface(SDL_SWSURFACE,a_dx,a_dy,32,RMASK,GMASK,BMASK,AMASK);
        SDL_Rect r;
        r.x=x;
        r.y=y;
        r.w=a_dx;
        r.h=a_dy;
        //        SDL_SetSurfaceAlphaMod(a_tex->m_surface, 0);
        SDL_BlitSurface(a_tex->m_surface,&r,tile,0);
    }
    
    dx=a_dx;
    dy=a_dy;
    tex = a_tex->m_tex;
    tex_coord_x0 = (x*a_tex->m_tx)/a_tex->m_surface->w;
    tex_coord_y0 = (y*a_tex->m_ty)/a_tex->m_surface->h;
    tex_coord_x1 = ((x+a_dx)*a_tex->m_tx)/a_tex->m_surface->w;
    tex_coord_y1 = ((y+a_dy)*a_tex->m_ty)/a_tex->m_surface->h;
    
    glBindVertexArray(0);
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat g_vertex_buffer_data[] = {
        0.0f, 0.0f, 0.0f,					tex_coord_x0, tex_coord_y0,
        (GLfloat)dx, 0, 0.0f,				tex_coord_x1, tex_coord_y0,
        (GLfloat)dx, (GLfloat)dy, 0.0f,		tex_coord_x1, tex_coord_y1,
        0.0f,  (GLfloat)dy, 0.0f,			tex_coord_x0, tex_coord_y1,
    };
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    free_texture = false;
    
    compute_bb();
}


void GLTile::set_hotspot(int hx,int hy)
{
    hot_x=hx;
    hot_y=hy;
    
    compute_bb();
} // GLTile::set_hotspot


void GLTile::set_hotspot_nobb(int hx,int hy)
{
    hot_x=hx;
    hot_y=hy;
} // GLTile::set_hotspot


void GLTile::set_bb(int x1,int y1,int x2,int y2)
{
    float x[2];
    float y[2];
    
    x[0]=float(x1);
    y[0]=float(y1);
    x[1]=float(x2);
    y[1]=float(y2);
    
    x[0]-=get_hot_x();
    x[1]-=get_hot_x();
    y[0]-=get_hot_y();
    y[1]-=get_hot_y();
    
    bb->set(x,y,2);
} // GLTile::set_bb


void GLTile::compute_bb(void)
{
    float x[2]={0,0},y[2]={0,0};
    bool first=true;
    int i,j;
    Uint32 c;
    
    if (bb!=0) delete bb;
    
    if (tile==0) {
        x[1] = dx;
        y[1] = dy;
        bb=new BB2D(x,y,2);
        return;
    }
    
    for(i=0;i<get_dy();i++) {
        for(j=0;j<get_dx();j++) {
            c=get_pixel(j,i);
            //			if ((c&AMASK)==AMASK) {
            if ((c&AMASK)!=0) {
                if (first) {
                    first=false;
                    x[0]=x[1]=float(j);
                    y[0]=y[1]=float(i);
                } else {
                    if (j<x[0]) x[0]=float(j);
                    if (j>x[1]) x[1]=float(j);
                    if (i<y[0]) y[0]=float(i);
                    if (i>y[1]) y[1]=float(i);
                } // if
            } // if
            
        } // for
    } // for
    
    x[0]-=get_hot_x();
    x[1]-=get_hot_x();
    y[0]-=get_hot_y();
    y[1]-=get_hot_y();
    
    bb=new BB2D(x,y,2);
} // GLTile::compute_bb


void GLTile::draw()
{
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &identity[0][0]);
    drawInternal();
} // GLTile::draw


void GLTile::draw(glm::mat4 transform)
{
    glm::mat4 tmp = glm::translate(transform, glm::vec3(-hot_x,-hot_y,0));
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    drawInternal();
}


void GLTile::draw(float r, float g, float b, float a, glm::mat4 transform)
{
    glm::mat4 tmp = glm::translate(transform, glm::vec3(-hot_x,-hot_y,0));
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    glUniform4f(inColorID, r, g, b, a);
    drawInternal();
    glUniform4f(inColorID, 1.0,1.0,1.0,1.0);
}


void GLTile::draw(float x,float y,float z,float angle_in_degrees,float scale)
{
    glm::mat4 tmp = glm::translate(identity, glm::vec3(x,y,z));
    if (scale!=1) tmp = glm::scale(tmp, glm::vec3(scale, scale, scale));
    if (angle_in_degrees!=0) tmp = glm::rotate(tmp, -float(angle_in_degrees*M_PI/180), glm::vec3(0,0,-1));
    if (hot_x!=0 || hot_y!=0) tmp = glm::translate(tmp, glm::vec3(-hot_x,-hot_y,0));
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    glUniform4f(inColorID, 1.0,1.0,1.0,1.0);
    drawInternal();
}


void GLTile::draw(float r, float g, float b, float a, float x,float y,float z,float angle_in_degrees,float scale)
{
    glm::mat4 tmp = glm::translate(identity, glm::vec3(x,y,z));
    if (scale!=1) tmp = glm::scale(tmp, glm::vec3(scale, scale, scale));
    if (angle_in_degrees!=0) tmp = glm::rotate(tmp, -float(angle_in_degrees*M_PI/180), glm::vec3(0,0,-1));
    if (hot_x!=0 || hot_y!=0) tmp = glm::translate(tmp, glm::vec3(-hot_x,-hot_y,0));
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    glUniform4f(inColorID, r, g, b, a);
    drawInternal();
    glUniform4f(inColorID, 1.0,1.0,1.0,1.0);
}


void GLTile::drawInternal()
{
    if (textureSamplerLocation==-1) {
        textureSamplerLocation = glGetUniformLocation(programID,"textureSampler");
        vPositionLocation = glGetAttribLocation(programID, "vPosition");
        UVLocation = glGetAttribLocation(programID, "UV");
    }
    glUniform1i(useTextureID, 1);
    setTexture(tex);
    glUniform1i(textureSamplerLocation, 0);
    
    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(vPositionLocation);
    glEnableVertexAttribArray(UVLocation);
    glVertexAttribPointer(vPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(vPositionLocation);
    glDisableVertexAttribArray(UVLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUniform1i(useTextureID, 0);
}


Uint32 GLTile::get_pixel(int ax,int ay)
{
    if (ax>=0 && ax<dx &&
        ay>=0 && ay<dy) {
        return getpixel(tile,ax,ay);
    } // if
    
    return 0;
} // GLTile::get_pixel


bool GLTile::collision_bb(float x1,float y1,float a1, GLTile *t2,float x2,float y2,float a2)
{
    if (bb==0) compute_bb();
    if (t2->get_bb()==0) t2->compute_bb();
    return bb->collision(x1, y1, a1, t2->get_bb(), x2, y2, a2);
}


bool GLTile::collision_bb(float x1,float y1,GLTile *t2,float x2,float y2)
{
    return collision_bb(x1, y1, 1, t2, x2, y2, 1);
}

/*
bool GLTile::collision_pixel(float x1,float y1,float a1,float scale1, GLTile *t2,float x2,float y2,float a2, float scale2)
{
    SDL_Surface *s1 = tile;
    SDL_Surface *s2 = t2->tile;
    
    // Radius check:
    int collision_pixels=0;
    float radius1,radius2;
    int sq_radius1;
    int sq_radius2;
    float sq_distance;
    int dx1=s1->w - hot_x;
    int dy1=s1->h - hot_y;
    int dx2=s2->w - t2->hot_x;
    int dy2=s2->h - t2->hot_y;
    if (hot_x>dx1) dx1=hot_x;
    if (hot_y>dy1) dy1=hot_y;
    if (t2->hot_x>dx2) dx2=t2->hot_x;
    if (t2->hot_y>dy2) dy2=t2->hot_y;
    
    sq_radius1=dx1*dx1+dy1*dy1;
    sq_radius2=dx2*dx2+dy2*dy2;
    radius1=float(sqrt((double)sq_radius1))*scale1;
    radius2=float(sqrt((double)sq_radius2))*scale2;
    sq_distance=(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
    
    if (sq_distance>(radius1+radius2)*(radius1+radius2)) return false;
    
    
    // Create a temprorary surface where to perform the collision check:
    {
        SDL_Surface *obj1_sfc,*obj2_sfc;
        int minx=0,maxx=0;
        int miny=0,maxy=0;
        int new_hot_x1,new_hot_y1;
        int new_hot_x2,new_hot_y2;
        SDL_Rect r1;
        SDL_Rect r2;
        bool free_rotozoom1 = false;
        bool free_rotozoom2 = false;
        
        // Draw the first object:
        if (a1==0 && scale1==1) {
            obj1_sfc=s1;
        } else {
            obj1_sfc=rotozoomSurface(s1,-a1,scale1,0);
            free_rotozoom1 = true;
        }
        if (a2==0 && scale2==1) {
            obj2_sfc=s2;
        } else {
            obj2_sfc=rotozoomSurface(s2,-a2,scale2,0);
            free_rotozoom2 = true;
        }
        {
            float rad_ang1=float((float(-a1)*M_PI))/180;
            float tmpx1=hot_x-float(s1->w)/2;
            float tmpy1=hot_y-float(s1->h)/2;
            float cx1=float(obj1_sfc->w)/2;
            float cy1=float(obj1_sfc->h)/2;
            new_hot_x1=int(cx1+((cos(rad_ang1)*tmpx1+sin(rad_ang1)*tmpy1))*scale1);
            new_hot_y1=int(cy1+((-sin(rad_ang1)*tmpx1+cos(rad_ang1)*tmpy1))*scale1);
            float rad_ang2=float(float(-a2)*M_PI)/180;
            float tmpx2=t2->hot_x-float(s2->w)/2;
            float tmpy2=t2->hot_y-float(s2->h)/2;
            float cx2=float(obj2_sfc->w)/2;
            float cy2=float(obj2_sfc->h)/2;
            new_hot_x2=int(cx2+((cos(rad_ang2)*tmpx2+sin(rad_ang2)*tmpy2))*scale2);
            new_hot_y2=int(cy2+((-sin(rad_ang2)*tmpx2+cos(rad_ang2)*tmpy2))*scale2);
            
            r1.x=int(x1-new_hot_x1);
            r1.y=int(y1-new_hot_y1);
            r1.w=obj1_sfc->w;
            r1.h=obj1_sfc->h;
            r2.x=int(x2-new_hot_x2);
            r2.y=int(y2-new_hot_y2);
            r2.w=obj2_sfc->w;
            r2.h=obj2_sfc->h;
            
            minx=std::min(r1.x,r2.x);
            miny=std::min(r1.y,r2.y);
            maxx=std::max(r1.x+r1.w,r2.x+r2.w);
            maxy=std::max(r1.y+r1.h,r2.y+r2.h);
            
            r1.x-=minx;
            r2.x-=minx;
            r1.y-=miny;
            r2.y-=miny;
            
            {
                int i,j,offs1,offs2;
                int common_x1,common_x2;
                int common_y1,common_y2;
                
                common_x1=std::max(r1.x,r2.x);
                common_x2=std::min(r1.x+r1.w,r2.x+r2.w);
                
                common_y1=std::max(r1.y,r2.y);
                common_y2=std::min(r1.y+r1.h,r2.y+r2.h);
                
                if (common_x1>=common_x2 || common_y1>=common_y2) {
                    if (free_rotozoom1) SDL_FreeSurface(obj1_sfc);
                    if (free_rotozoom2) SDL_FreeSurface(obj2_sfc);
                    return false;
                } // if
                
                int area1 = s1->w*s1->h;
                int area2 = s2->w*s2->h;
                int min_area = (area1>area2 ? area2:area1);
                
                for(i=common_y1;i<common_y2;i++) {
                    offs1=(common_x1-r1.x)*4+(i-r1.y)*obj1_sfc->pitch;
                    offs2=(common_x1-r2.x)*4+(i-r2.y)*obj2_sfc->pitch;
                    for(j=common_x1;j<common_x2;j++,offs1+=4,offs2+=4) {
                        if ((*((Uint32 *)((char *)(obj1_sfc->pixels)+offs1))&AMASK)==AMASK &&
                            (*((Uint32 *)((char *)(obj2_sfc->pixels)+offs2))&AMASK)==AMASK)
                            collision_pixels++;
                        if (collision_pixels>COLLISION_TOLERANCE || collision_pixels>=min_area) {
                            if (free_rotozoom1) SDL_FreeSurface(obj1_sfc);
                            if (free_rotozoom2) SDL_FreeSurface(obj2_sfc);
                            return true;
                        } // if 
                    } // for
                } // for
            }
            
            if (free_rotozoom1) SDL_FreeSurface(obj1_sfc);
            if (free_rotozoom2) SDL_FreeSurface(obj2_sfc);
        }
    }
    return false;
}


bool GLTile::collision_pixel(float x1,float y1,GLTile *t2,float x2,float y2)
{
    return collision_pixel(x1, y1, 0, 1, t2, x2, y2, 0, 1);
}
*/
