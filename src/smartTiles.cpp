#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"
#include <list>

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif

#include "SDL_rotozoom.h"


#include "Symbol.h"
#include "BB2D.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "BB2D.h"
#include "sound.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "VirtualController.h"

#include "GLTManager.h"
#include "SFXManager.h"
#include "TGLobject.h"
#include "TGLmap.h"

#include "SmartTiles.h"


#include "debug.h"

// results cache:
std::list<SmartTileCacheEntry *> smartTileCache[1024];


SmartTileCacheEntry *findSmarTileInCache(GLTile *t1, GLTile *t2, int direction, int windowSize) 
{
	std::list<SmartTileCacheEntry *> l;
	SmartTileCacheEntry tmp;
	tmp.t1 = t1;
	tmp.t2 = t2;
	tmp.direction = direction;
	tmp.windowsize = windowSize;
	int h = tmp.hash();
	
    for(SmartTileCacheEntry *tmp2:smartTileCache[h]) {
		if (tmp2->t1 == tmp.t1 &&
			tmp2->t2 == tmp.t2 &&
			tmp2->direction == tmp.direction &&
			tmp2->windowsize == tmp.windowsize) return tmp2;
	}
	
	return 0;
}


/*
 this method assumes that t1 and t2 are squares of the same size = 32
 
 direction: 0 = left, 1 = up, 2 = right, 3 = down
 
 windowSize: between 1 to 32: 1 checks pixels 1 by one, 2 checks windows of 2x2 pixels, etc. 32, just checks for the average color of the tiles
 */
float smartTileScore(GLTile *t1, GLTile *t2, int direction, int windowSize)
{
	SmartTileCacheEntry *tmp = findSmarTileInCache(t1,t2,direction,windowSize);
	if (tmp!=0) return tmp->result;
	
	
	Uint32 pixel;
	Uint8 r,g,b,a;
	Uint32 pixel2;;
	Uint8 r2,g2,b2,a2;
	float ar,ag,ab;
	float ar2,ag2,ab2;

	float score = 0;
	for(int i = 0;i<32-(windowSize-1);i++) {		
		ar = ag = ab = 0;
		ar2 = ag2 = ab2 = 0;
		for(int x = 0;x<windowSize;x++) {
			for(int y = 0;y<windowSize;y++) {
				pixel = 0;
				if (t1!=0) {
					switch(direction) {
						case 0:	pixel = t1->get_pixel(x,i+y); break;
						case 1:	pixel = t1->get_pixel(i+x,y); break;
						case 2:	pixel = t1->get_pixel((t1->get_dx()-1)-x,i+y); break;
						case 3:	pixel = t1->get_pixel(i+x,(t1->get_dy()-1)-y); break;
					}				
					SDL_GetRGBA(pixel, t1->getSurface()->format, &r, &g, &b, &a);
					ar += r*float(a)/255.0f;
					ag += g*float(a)/255.0f;
					ab += b*float(a)/255.0f;
				}					
				if (t2!=0) {
					switch(direction) {
						case 0:	pixel2 = t2->get_pixel((t2->get_dx()-1)-x,i+y); break;
						case 1:	pixel2 = t2->get_pixel(i+x,(t2->get_dy()-1)-y); break;
						case 2:	pixel2 = t2->get_pixel(x,i+y); break;
						default:	pixel2 = t2->get_pixel(i+x,y); break;
					}
					SDL_GetRGBA(pixel2, t2->getSurface()->format, &r2, &g2, &b2, &a2);
					ar2 += r2*float(a2)/255.0f;
					ag2 += g2*float(a2)/255.0f;
					ab2 += b2*float(a2)/255.0f;
				}
			}
		}
		ar/=windowSize*windowSize;
		ag/=windowSize*windowSize;
		ab/=windowSize*windowSize;
		ar2/=windowSize*windowSize;
		ag2/=windowSize*windowSize;
		ab2/=windowSize*windowSize;
		
		score += (float(ar-ar2)*float(ar-ar2) + float(ag-ag2)*float(ag-ag2) + float(ab-ab2)*float(ab-ab2));
	}
	
	tmp = new SmartTileCacheEntry();
	tmp->t1 = t1;
	tmp->t2 = t2;
	tmp->direction = direction;
	tmp->windowsize = windowSize;
	tmp->result = score;
	smartTileCache[tmp->hash()].push_back(tmp);
	
	return score;
}


float smartTileNumberOfNonZeroPizels(GLTile *t)
{
	Uint32 pixel;
	Uint8 r,g,b,a;
	int res = 0;
	
	if (t==0) return 0;
	
	for(int i = 0;i<32;i++) {		
		for(int j = 0;j<32;j++) {			
			pixel = t->get_pixel(i,j);
			SDL_GetRGBA(pixel, t->getSurface()->format, &r, &g, &b, &a);
			if (a!=0 && (r!=0 || g!=0 || b!=0)) res++;
		}
	}
	return res;
}


GLTile *smartTile(TGLmap *map, int x, int y, float wleft, float wup, float wright, float wdown, std::vector<GLTile *> *tiles)
{
	GLTile *best = 0;
	float best_score = 0;
	GLTile *tile_left = 0;
	GLTile *tile_up = 0;
	GLTile *tile_right = 0;
	GLTile *tile_down = 0;
//	Uint32 pixel;
//	Uint8 r,g,b,a;
//	Uint32 pixel2;;
//	Uint8 r2,g2,b2,a2;
	int windowSize = 4;
	
	if (x>0) tile_left = map->getFGTile((x-1)+y*map->get_fg_dx()); else wleft = 0;
	if (y>0) tile_up = map->getFGTile((x)+(y-1)*map->get_fg_dx()); else wup = 0;
	if (x<map->get_fg_dx()-1) tile_right = map->getFGTile((x+1)+y*map->get_fg_dx()); else wright = 0;
	if (y<map->get_fg_dy()-1) tile_down = map->getFGTile((x)+(y+1)*map->get_fg_dx()); else wdown = 0;
		
	/*
	 #ifdef __DEBUG_MESSAGES
	 output_debug_message("smartTile %i,%i with (%i,%i,%i,%i)\n",x,y,l.PositionRef(tile_left), l.PositionRef(tile_up), l.PositionRef(tile_right), l.PositionRef(tile_down));
	 #endif	
	 */
	
	// score of the empty tile:
	{
		float score_left = 0;
		float score_up = 0;
		float score_right = 0;
		float score_down = 0;
//		float score = 0;
		if (wleft>0) score_left = smartTileScore(0,tile_left,0,windowSize);
		if (wup>0) score_up = smartTileScore(0,tile_up,1,windowSize);
		if (wright>0) score_right = smartTileScore(0,tile_right,2,windowSize);
		if (wdown>0) score_down = smartTileScore(0,tile_down,3,windowSize);
		best = 0;
		best_score += wleft*score_left + wup*score_up + wright*score_right + wdown*score_down;
	}
	
    for(GLTile *tile:*tiles) {
		float score_left = 0;
		float score_up = 0;
		float score_right = 0;
		float score_down = 0;
		float score = 0;
		if (wleft>0) score_left = smartTileScore(tile,tile_left,0,windowSize);
		if (wup>0) score_up = smartTileScore(tile,tile_up,1,windowSize);
		if (wright>0) score_right = smartTileScore(tile,tile_right,2,windowSize);
		if (wdown>0) score_down = smartTileScore(tile,tile_down,3,windowSize);

		score += wleft*score_left + wup*score_up + wright*score_right + wdown*score_down;
		
		if (score<best_score) {
			best = tile;
			best_score = score;
		} else if (score==best_score) {
			// select the tile with least number of non-zero pixels:
			if (smartTileNumberOfNonZeroPizels(best)>smartTileNumberOfNonZeroPizels(tile)) {
				best = tile;
			}
		}
	}
	
	return best;
}