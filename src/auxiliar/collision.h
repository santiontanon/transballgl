#ifndef TILE_COLLISIONS
#define TILE_COLLISIONS

// these functions admit an optional TGLoobject parameter per surface, to prevent rotozooming the surfaces many times with the same parameters:

bool collision_with_point(TGLobject *o1,SDL_Surface *s1,float x1,float y1,int hot_x1,int hot_y1,float a1,float scale1,float x2,float y2);
bool collision(TGLobject *o1,SDL_Surface *s1,float x1,float y1,int hot_x1,int hot_y1,float a1,float scale1,TGLobject *o2,SDL_Surface *s2,float x2,float y2,int hot_x2,int hot_y2,float a2,float scale2);
bool collision_vector(TGLobject *o1,SDL_Surface *s1,float x1,float y1,int hot_x1,int hot_y1,float a1,float scale1,TGLobject *o2,SDL_Surface *s2,float x2,float y2,int hot_x2,int hot_y2,float a2,float scale2,float *collision_vector_x,float *collision_vector_y);

#endif