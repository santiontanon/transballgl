#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#else
#include <sys/time.h>
#include <time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "ctype.h"
#include "math.h"
#include "string.h"
#include "assert.h"
#include <vector>
#include <algorithm>

#include "SDL.h"
#include "SDL_rotozoom.h"


#include "auxiliar.h"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#endif

#include "Symbol.h"
#include "GLtile.h"

#include "TGLobject.h"
#include "collision.h"

#include "debug.h"

// GLTile *last_collision=0;

#define COLLISION_TOLERANCE	2



bool collision_with_point(TGLobject *o1,SDL_Surface *s1,float x1,float y1,int hot_x1,int hot_y1,float a1,float scale1,float x2,float y2)
{
	// Radius check:
//	int collision_pixels=0;
	float radius1;
	int sq_radius1;
	float sq_distance;
	int dx1=s1->w - hot_x1;
	int dy1=s1->h - hot_y1;
	if (hot_x1>dx1) dx1=hot_x1;
	if (hot_y1>dy1) dy1=hot_y1;
	bool free_rotozoom = false;

	sq_radius1=dx1*dx1+dy1*dy1;
	radius1=float(sqrt((double)sq_radius1))*scale1;
	sq_distance=(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);

	if (sq_distance>(radius1)*(radius1)) return false;


	// Create a temprorary surface where to perform the collision check:
	{
		SDL_Surface *obj1_sfc;
		int minx=0,maxx=0;
		int miny=0,maxy=0;
		int new_hot_x1,new_hot_y1;
		SDL_Rect r1;

		// Draw the first object:
		if (a1==0 && scale1==1) {
			obj1_sfc=s1;
		} else {
			if (o1!=0) {
				obj1_sfc = o1->get_collision_cache(a1,scale1,s1);
				if (obj1_sfc==0) {
					obj1_sfc=rotozoomSurface(s1,-a1,scale1,0);
					o1->set_collision_cache(obj1_sfc,a1,scale1,s1);
				}
			} else {
				obj1_sfc=rotozoomSurface(s1,-a1,scale1,0);
				free_rotozoom = true;
			}
		}
		{
			float rad_ang1=float((float(-a1)*M_PI))/180;
			float tmpx1=hot_x1-float(s1->w)/2;
			float tmpy1=hot_y1-float(s1->h)/2;
			float cx1=float(obj1_sfc->w)/2;
			float cy1=float(obj1_sfc->h)/2;
			new_hot_x1=int(cx1+((cos(rad_ang1)*tmpx1+sin(rad_ang1)*tmpy1))*scale1);
			new_hot_y1=int(cy1+((-sin(rad_ang1)*tmpx1+cos(rad_ang1)*tmpy1))*scale1);

			r1.x=int(x1-new_hot_x1);
			r1.y=int(y1-new_hot_y1);
			r1.w=obj1_sfc->w;
			r1.h=obj1_sfc->h;

            minx=std::min(r1.x,int(x2+0));
			miny=std::min(r1.y,int(y2+0));
			maxx=std::max(r1.x+r1.w,int(x2+1));
			maxy=std::max(r1.y+r1.h,int(y2+1));

			r1.x-=minx;
			x2-=minx;
			r1.y-=miny;
			y2-=miny;

			{
				int i,j,offs1;
				int common_x1,common_x2; 
				int common_y1,common_y2;

				common_x1=std::max(r1.x,int(x2+0));
				common_x2=std::min(r1.x+r1.w,int(x2+1));

				common_y1=std::max(r1.y,int(y2+0));
				common_y2=std::min(r1.y+r1.h,int(y2+1));

				if (common_x1>=common_x2 || common_y1>=common_y2) {
					if (free_rotozoom) SDL_FreeSurface(obj1_sfc);
					return false;
				} // if 

				for(i=common_y1;i<common_y2;i++) {
					offs1=(common_x1-r1.x)*4+(i-r1.y)*obj1_sfc->pitch;					
					for(j=common_x1;j<common_x2;j++,offs1+=4) {
						if ((*((Uint32 *)((char *)(obj1_sfc->pixels)+offs1))&AMASK)==AMASK) {
							if (free_rotozoom) SDL_FreeSurface(obj1_sfc);
							return true;
						}
					} // for
				} // for
			}
			if (free_rotozoom) SDL_FreeSurface(obj1_sfc);
		}
	}
	return false;
} /* collision_with_point */ 



bool collision(TGLobject *o1,SDL_Surface *s1,float x1,float y1,int hot_x1,int hot_y1,float a1,float scale1,TGLobject *o2,SDL_Surface *s2,float x2,float y2,int hot_x2,int hot_y2,float a2,float scale2)
{
	// Radius check:
	int collision_pixels=0;
	float radius1,radius2;
	int sq_radius1;
	int sq_radius2;
	float sq_distance;
	int dx1=s1->w - hot_x1;
	int dy1=s1->h - hot_y1;
	int dx2=s2->w - hot_x2;
	int dy2=s2->h - hot_y2;
	if (hot_x1>dx1) dx1=hot_x1;
	if (hot_y1>dy1) dy1=hot_y1;
	if (hot_x2>dx2) dx2=hot_x2;
	if (hot_y2>dy2) dy2=hot_y2;

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
			if (o1!=0) {
				obj1_sfc = o1->get_collision_cache(a1,scale1,s1);
				if (obj1_sfc==0) {
					obj1_sfc=rotozoomSurface(s1,-a1,scale1,0);
					o1->set_collision_cache(obj1_sfc,a1,scale1,s1);
				}
			} else {
				obj1_sfc=rotozoomSurface(s1,-a1,scale1,0);
				free_rotozoom1 = true;
			}
		}
		if (a2==0 && scale2==1) {
			obj2_sfc=s2;
		} else {
			if (o2!=0) {
				obj2_sfc = o2->get_collision_cache(a2,scale2,s2);
				if (obj2_sfc==0) {
					obj2_sfc=rotozoomSurface(s2,-a2,scale2,0);
					o2->set_collision_cache(obj2_sfc,a2,scale2,s2);
				}
			} else {
				obj2_sfc=rotozoomSurface(s2,-a2,scale2,0);
				free_rotozoom2 = true;
			}
		}
		{
			float rad_ang1=float((float(-a1)*M_PI))/180;
			float tmpx1=hot_x1-float(s1->w)/2;
			float tmpy1=hot_y1-float(s1->h)/2;
			float cx1=float(obj1_sfc->w)/2;
			float cy1=float(obj1_sfc->h)/2;
			new_hot_x1=int(cx1+((cos(rad_ang1)*tmpx1+sin(rad_ang1)*tmpy1))*scale1);
			new_hot_y1=int(cy1+((-sin(rad_ang1)*tmpx1+cos(rad_ang1)*tmpy1))*scale1);
			float rad_ang2=float(float(-a2)*M_PI)/180;
			float tmpx2=hot_x2-float(s2->w)/2;
			float tmpy2=hot_y2-float(s2->h)/2;
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
} /* collision */ 



bool collision_vector(TGLobject *o1,SDL_Surface *s1,float x1,float y1,int hot_x1,int hot_y1,float a1,float scale1,TGLobject *o2,SDL_Surface *s2,float x2,float y2,int hot_x2,int hot_y2,float a2,float scale2,float *collision_vector_x,float *collision_vector_y)
{
	std::vector<int> collision_x,collision_y;
	int collision_pixels=0;
	float radius1,radius2;
	int sq_radius1;
	int sq_radius2;
	float sq_distance;
	int dx1=s1->w - hot_x1;
	int dy1=s1->h - hot_y1;
	int dx2=s2->w - hot_x2;
	int dy2=s2->h - hot_y2;
	if (hot_x1>dx1) dx1=hot_x1;
	if (hot_y1>dy1) dy1=hot_y1;
	if (hot_x2>dx2) dx2=hot_x2;
	if (hot_y2>dy2) dy2=hot_y2;

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
			if (o1!=0) {
				obj1_sfc = o1->get_collision_cache(a1,scale1,s1);
				if (obj1_sfc==0) {
					obj1_sfc=rotozoomSurface(s1,-a1,scale1,0);
					o1->set_collision_cache(obj1_sfc,a1,scale1,s1);
				}
			} else {
				obj1_sfc=rotozoomSurface(s1,-a1,scale1,0);
				free_rotozoom1 = true;
			}
		}
		if (a2==0 && scale2==1) {
			obj2_sfc=s2;
		} else {
			if (o2!=0) {
				obj2_sfc = o2->get_collision_cache(a2,scale2,s2);
				if (obj2_sfc==0) {
					obj2_sfc=rotozoomSurface(s2,-a2,scale2,0);
					o2->set_collision_cache(obj2_sfc,a2,scale2,s2);
				}
			} else {
				obj2_sfc=rotozoomSurface(s2,-a2,scale2,0);
				free_rotozoom2 = true;
			}
		}
		{
			float rad_ang1=float(float(-a1)*M_PI)/180;
			float tmpx1=hot_x1-float(s1->w)/2;
			float tmpy1=hot_y1-float(s1->h)/2;
			float cx1=float(obj1_sfc->w)/2;
			float cy1=float(obj1_sfc->h)/2;
			new_hot_x1=int(cx1+((cos(rad_ang1)*tmpx1+sin(rad_ang1)*tmpy1))*scale1);
			new_hot_y1=int(cy1+((-sin(rad_ang1)*tmpx1+cos(rad_ang1)*tmpy1))*scale1);
			float rad_ang2=float(float(-a2)*M_PI)/180;
			float tmpx2=hot_x2-float(s2->w)/2;
			float tmpy2=hot_y2-float(s2->h)/2;
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

//			sfc=SDL_CreateRGBSurface(SDL_SWSURFACE,(maxx+1)-minx,(maxy+1)-miny,32,RMASK,GMASK,BMASK,AMASK);
//			SDL_FillRect(sfc,0,0);

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

				for(i=common_y1;i<common_y2;i++) {
					offs1=(common_x1-r1.x)*4+(i-r1.y)*obj1_sfc->pitch;
					offs2=(common_x1-r2.x)*4+(i-r2.y)*obj2_sfc->pitch;
					for(j=common_x1;j<common_x2;j++,offs1+=4,offs2+=4) {
						if ((*((Uint32 *)((char *)(obj1_sfc->pixels)+offs1))&AMASK)==AMASK &&
							(*((Uint32 *)((char *)(obj2_sfc->pixels)+offs2))&AMASK)==AMASK) {
							collision_pixels++;

							collision_x.push_back(((j-r1.x)-new_hot_x1));
							collision_y.push_back(((i-r1.y)-new_hot_y1));
						} // if 
					} // for
				} // for
			}
			if (free_rotozoom1) SDL_FreeSurface(obj1_sfc);
			if (free_rotozoom2) SDL_FreeSurface(obj2_sfc);
		}

//		if (last_collision!=0) delete last_collision;
//		last_collision=new GLTile(sfc);
//		SDL_FreeSurface(sfc);

		if (collision_x.size()>COLLISION_TOLERANCE) {
			// Compute the collision vector:
			float vx=0;
			float vy=0;
			int i;

			for(i=0;i<collision_pixels;i++) {
				vx+=collision_x[i];
				vy+=collision_y[i];
			} // for 

			*collision_vector_x=vx/collision_pixels;
			*collision_vector_y=vy/collision_pixels;
			return true;
		} // if 
	}

	return false;
} /* collision_vector */ 


