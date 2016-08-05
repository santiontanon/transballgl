#ifndef BRAIN_AUXILIAR
#define BRAIN_AUXILIAR

#ifndef M_PI
#define M_PI	3.141592654F
#endif

#include "SDL.h"
#include<list>
#include<vector>

//#ifndef _WIN32
//#define min(a,b) ((a)<(b) ? (a) : (b))
//#define max(a,b) ((a)>(b) ? (a) : (b))
//#endif
//#define abs(a) ((a)<0 ? (-(a)) : (a))


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
// PPC values:
#define RMASK  0xff000000
#define GMASK  0x00ff0000
#define BMASK  0x0000ff00
#define AMASK  0x000000ff
#define AOFFSET 3
#define BOFFSET 2
#define GOFFSET 1
#define ROFFSET 0

#else
// Intel values:
#define RMASK  0x000000ff
#define GMASK  0x0000ff00
#define BMASK  0x00ff0000
#define AMASK  0xff000000 
#define AOFFSET 3
#define BOFFSET 2
#define GOFFSET 1
#define ROFFSET 0

#endif

/*
judging by the ifndef, we don't need it on windows
we also don't need it on OSX...
#ifndef _WIN32
#ifndef HAVE_STRLWR

char *strlwr (char *str);

#endif
#endif
*/

bool substr(char *str,char *substr);
char *replaceString(const char *string, const char *replaceThis, const char *withThis);
void pause(unsigned int time);


template<typename T> T getAt(std::list<T> l, int idx)
{
    int i = 0;
    for(T o:l) {
        if (i==idx) return o;
        i++;
    }
    return (T)0;
}


template<typename T> int indexOf(std::list<T> l, T obj)
{
    int idx = 0;
    for(T obj2:l) {
        if (obj==obj2) return idx;
        idx++;
    }
    return -1;
}


template<typename T> int indexOf(std::vector<T> l, T obj)
{
    int idx = 0;
    for(T obj2:l) {
        if (obj==obj2) return idx;
        idx++;
    }
    return -1;
}


template<typename T> bool contains(std::list<T> l, T obj)
{
    for(T obj2:l) {
        if (obj==obj2) return true;
    }
    return false;
}


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void maximumpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void draw_rectangle(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel);
void draw_rectangle_vp(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel,SDL_Rect *vp);
void draw_line(SDL_Surface *SDL,int x1,int y1,int x2,int y2,Uint32 pixel);
void draw_thick_line(SDL_Surface *SDL,int x1,int y1,int x2,int y2,Uint32 pixel,int thickness);
void area_fill(SDL_Surface *SDL,int x,int y,Uint32 pixel);
void fast_area_fill(SDL_Surface *SDL,int x,int y,Uint32 pixel);

void surface_automatic_alpha(SDL_Surface *sfc);
void surface_bw(SDL_Surface *sfc,int threshold);
void surface_mask_from_bitmap(SDL_Surface *sfc,SDL_Surface *mask,int x,int y);

bool segment_collision(int s1x1,int s1y1,int s1x2,int s1y2,
					   int s2x1,int s2y1,int s2x2,int s2y2,
					   int *cx,int *cy);


/* Blits orig into dest, and sets the alpha channel of all the blitted pixels to the right value: */ 
void surface_blit_alpha(SDL_Surface *orig,SDL_Rect *o_r,SDL_Surface *dest,SDL_Rect *d_r);

/* functions to write/read from files: */ 

bool save_float(FILE *fp,float f);
bool load_float(FILE *fp,float *f);

bool save_int(FILE *fp,int f);
bool load_int(FILE *fp,int *f);

// like standard sin/cos functions, but defined for degrees, rather than radians:
double sin_degree(double degree);
double cos_degree(double degree);

#endif

