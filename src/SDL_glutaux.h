#ifndef __BRAIN_SDL_GLUTAUX
#define __BRAIN_SDL_GLUTAUX

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

#ifdef __EMSCRIPTEN__
#include <glm.hpp>
#include <ext.hpp>
#else
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#endif


extern GLuint programID;

extern GLuint PVMatrixID;
extern GLuint MMatrixID;
extern GLuint inColorID;
extern GLuint useTextureID;

GLuint LoadShaders();

void setTexture(GLuint tex);    // This function keeps track of the previous texture being used,

GLuint  createTexture(SDL_Surface *sfc,float *tx,float *ty);
GLuint  createTextureFromScreen(int x,int y,int dx,int dy,float *tx,float *ty);

// Note: these functions are VERY slow, since it creates and destroys a new vertex buffer each time it's called:
void clearDrawPrimitivesData();
void drawQuad(float x, float y, float w, float h, float r, float g, float b, float a);
void drawCircle(float x, float y, float z, float radius, float r, float g, float b, float a);
void drawCircle(glm::mat4 transform, float x, float y, float z, float radius, float r, float g, float b, float a);
void drawPixel(float x, float y, float z, float r, float g, float b, float a);
void drawLine(float x0, float y0, float x1, float y1, float r, float g, float b, float a);
void drawTriangle(float x0, float y0,
                  float x1, float y1,
                  float x2, float y2,
                  float r, float g, float b, float a);

#endif
