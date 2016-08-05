#ifndef __BRAIN_GL_TILE
#define __BRAIN_GL_TILE

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


/* OpenGL HW accelerated 2D tiles */

class GLTile {
public:
    GLTile();
    GLTile(const char *fname);
    GLTile(SDL_Surface *sfc);
    GLTile(char *fname,int x,int y,int dx,int dy);
    GLTile(SDL_Surface *sfc,int x,int y,int dx,int dy);
    GLTile(class GLTManagerImageNode *a_tex, int x, int y, int dx, int dy);
    ~GLTile();
    void free(void);
    
    void set(const char *fname);
    void set(SDL_Surface *sfc);
    void set(GLTManagerImageNode *a_tex, int x, int y, int dx, int dy);
    
    void draw();
    void draw(glm::mat4 transform);
    void draw(float r, float g, float b, float a, glm::mat4 transform);
    void draw(float x,float y,float z,float angle,float scale);
    void draw(float r, float g, float b, float a, float x,float y,float z,float angle,float scale);
    void drawInternal();
    
    int get_dx(void) {return dx;};
    int get_dy(void) {return dy;};
    int get_hot_x(void) {return hot_x;};
    int get_hot_y(void) {return hot_y;};
    
    void set_hotspot(int hx,int hy);
    void set_hotspot_nobb(int hx,int hy);
    void set_bb(int x1,int y1,int x2,int y2);
    
    void compute_bb(void);
    Uint32 get_pixel(int x,int y);
    class BB2D *get_bb(void) {return bb;};
    GLuint get_texture(void) {return tex;};
    SDL_Surface *getSurface() {return tile;};
    
    // collision detection routines, at the level of bounding boxes, and at the level of pixels:
    bool collision_bb(float x1,float y1,float a1, GLTile *t2,float x2,float y2,float a2);
    bool collision_bb(float x1,float y1,GLTile *t2,float x2,float y2);
//    bool collision_pixel(float x1,float y1,float a1,float scale1, GLTile *t2,float x2,float y2,float a2, float scale2);
//    bool collision_pixel(float x1,float y1,GLTile *t2,float x2,float y2);
    
private:
    glm::mat4 identity; // the identity matrix
    GLuint VertexArrayID;
    GLuint vertexbuffer;
    
    int hot_x,hot_y;
    
    SDL_Surface *tile;
    int dx,dy;
    float tex_coord_x0,tex_coord_y0;
    float tex_coord_x1,tex_coord_y1;
    
    bool free_texture;
    GLuint tex;
    int textureSamplerLocation, vPositionLocation, UVLocation;
    
    class BB2D *bb;
};

#endif
