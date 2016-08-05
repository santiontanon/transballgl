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
#include <algorithm>
#include <list>

#ifdef __EMSCRIPTEN__
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_image.h"
#include "SDL_ttf.h"
#endif

#include "auxiliar.h"


/*#ifndef _WIN32
#ifndef HAVE_STRLWR
judging by the ifndef, we don't need it on windows
we also don't need it on OSX...

char *strlwr (char *str)
{
	char *ptr;
	
	ptr = str;
	while (*ptr)
	{
	  	*ptr = tolower (*ptr);
		ptr++;
	}
	return str;
}


#endif
#endif
*/

bool substr(char *str,char *substr)
{
	char *ptr,*ptr2;

	while((*str)!=0) {
		ptr=substr;
		ptr2=str;

		while((*ptr)!=0 &&
			  (*ptr2)!=0 &&
			*ptr==*ptr2) {
			ptr++;
			ptr2++;
		} /* while */ 
		if ((*ptr)==0) return true;	

		str++;
	} /* while */ 

	return false;
} /* substr */ 


char *replaceString(const char *orig, const char *rep, const char *with)
{
    char *result; // the return string
    const char *ins;    // the next insert point
    char *tmp;    // varies
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements
    int len_rep = (int)strlen(rep);
    int len_with = (int)strlen(with);
    
    if (!orig) return 0;
    
    ins = orig;
    for (count = 0; (tmp = (char *)strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }
    
    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = new char[(strlen(orig) + (len_with - len_rep) * count + 1)];
    
    if (!result)
        return NULL;
    
    while (count--) {
        ins = strstr(orig, rep);
        len_front = (int)(ins - orig);
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}


void pause(unsigned int time)
{
	unsigned int initt=SDL_GetTicks();

	while((SDL_GetTicks()-initt)<time);
} /* pause */ 


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	SDL_Rect clip;
    int bpp = surface->format->BytesPerPixel;

	SDL_GetClipRect(surface,&clip);

	if (x<clip.x || x>=clip.x+clip.w ||
		y<clip.y || y>=clip.y+clip.h) return;

	if (x<0 || x>=surface->w ||
		y<0 || y>=surface->h) return;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = (Uint8)pixel;
        break;

    case 2:
        *(Uint16 *)p = (Uint16)pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = Uint8( (pixel >> 16) & 0xff );
            p[1] = Uint8( (pixel >> 8) & 0xff );
            p[2] = Uint8( pixel & 0xff );
        } else {
            p[0] = Uint8( pixel & 0xff );
            p[1] = Uint8( (pixel >> 8) & 0xff );
            p[2] = Uint8( (pixel >> 16) & 0xff );
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }

}


void maximumpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	SDL_Rect clip;
    int bpp = surface->format->BytesPerPixel;
	Uint8 r,g,b,r2,g2,b2;
	Uint8 *p;

	SDL_GetClipRect(surface,&clip);

	if (x<clip.x || x>=clip.x+clip.w ||
		y<clip.y || y>=clip.y+clip.h) return;

	if (x<0 || x>=surface->w ||
		y<0 || y>=surface->h || bpp!=4) return;

	p=(Uint8 *)&pixel;

	r2=p[ROFFSET];
	g2=p[GOFFSET];
	b2=p[BOFFSET];

	p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	r=p[ROFFSET];
	g=p[GOFFSET];
	b=p[BOFFSET];
	
    *(Uint32 *)p = SDL_MapRGB(surface->format,std::max(r,r2),std::max(g,g2),std::max(b,b2));
}


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;

	if (x<0 || x>=surface->w ||
		y<0 || y>=surface->h) return 0;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;
    }
}


void draw_rectangle(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel)
{
	int i;

	SDL_LockSurface(surface);

	for(i=0;i<w;i++) {
		if (x+i>=0 && x+i<surface->w && y>=0 && y<surface->h) putpixel(surface,x+i,y,pixel);
		if (x+i>=0 && x+i<surface->w && y+h-1>=0 && y+h<surface->h) putpixel(surface,x+i,y+h-1,pixel);
	} // for 
	for(i=0;i<h;i++) {
		if (x>=0 && x<surface->w && y+i>=0 && y+i<surface->h) putpixel(surface,x,y+i,pixel);
		if (x+w-1>=0 && x+w<surface->w && y+i>=0 && y+i<surface->h) putpixel(surface,x+w-1,y+i,pixel);
	} // for 

	SDL_UnlockSurface(surface);

} /* rectangle */ 



void draw_rectangle_vp(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel,SDL_Rect *vp)
{
	int i=0;
	int x0,y0,x1,y1;

	SDL_LockSurface(surface);

	if (vp->x>0) x0=vp->x;
			else x0=0;
	if (vp->x+vp->w<surface->w) x1=vp->x+vp->w;
						   else x1=surface->w;
	if (vp->y>0) y0=vp->y;
			else y0=0;
	if (vp->y+vp->h<surface->h) y1=vp->y+vp->h;
						   else y1=surface->h;

	if (y>=y0 && y<y1) for(i=0;i<w;i++) if (x+i>=x0 && x+i<x1) putpixel(surface,x+i,y,pixel);
	if (y+h-1>=y0 && y+h<y1) for(i=0;i<w;i++) if (x+i>=x0 && x+i<x1) putpixel(surface,x+i,y+h-1,pixel);

	if (y+i>=y0 && y+i<y1) for(i=0;i<h;i++) if (x>=x0 && x<x1) putpixel(surface,x,y+i,pixel);
	if (y+i>=y0 && y+i<y1) for(i=0;i<h;i++) if (x+w-1>=x0 && x+w<x1) putpixel(surface,x+w-1,y+i,pixel);

	SDL_UnlockSurface(surface);

} /* rectangle */ 



void draw_line(SDL_Surface *sfc,int x1,int y1,int x2,int y2,Uint32 pixel)
{
	long incy,rincy,incx,errterm,a;
	long d_x,d_y;
	int act_x,act_y;

	act_x=x1;
	act_y=y1;
	errterm=0;
	d_x=x2-x1;
	d_y=y2-y1;

	if (d_y<0) {
		incy=-1;
		rincy=-1;
		d_y=-d_y;
	} else {
		incy=1;
		rincy=1;
	} // if 
	if (d_x<0) {
		incx=-1;
		d_x=-d_x;
	} else {
		incx=1;
	} // if 

	SDL_LockSurface(sfc);

	if (d_x>d_y) {
		/* xline */ 
		for(a=0;a<=d_x;a++) {
			putpixel(sfc,act_x,act_y,pixel);
			errterm+=d_y;
			if (errterm>=d_x) {
				errterm-=d_x;
				act_y+=incy;
			} // if 
			act_x+=incx;
		} // for 
	} else {
		/* yline */ 
		for(a=0;a<=d_y;a++) {
			putpixel(sfc,act_x,act_y,pixel);
			errterm+=d_x;
			if (errterm>=d_y) {
				errterm-=d_y;
				act_x+=incx;
			} // if 
			act_y+=incy;
		} // for 
	} // if 

	SDL_UnlockSurface(sfc);
} /* draw_line */ 


void draw_thick_line(SDL_Surface *sfc,int x1,int y1,int x2,int y2,Uint32 pixel,int thickness)
{
	long incy,rincy,incx,errterm,a;
	long d_x,d_y;
	int act_x,act_y;

	int dot_x1,dot_x2,i,j;

	dot_x1=-(thickness/2);
	dot_x2=dot_x1+thickness;

	act_x=x1;
	act_y=y1;
	errterm=0;
	d_x=x2-x1;
	d_y=y2-y1;

	if (d_y<0) {
		incy=-1;
		rincy=-1;
		d_y=-d_y;
	} else {
		incy=1;
		rincy=1;
	} // if 
	if (d_x<0) {
		incx=-1;
		d_x=-d_x;
	} else {
		incx=1;
	} // if 

	SDL_LockSurface(sfc);

	if (d_x>d_y) {
		/* xline */ 
		for(a=0;a<=d_x;a++) {
			for(i=dot_x1;i<dot_x2;i++)
				for(j=dot_x1;j<dot_x2;j++)
					putpixel(sfc,act_x+i,act_y+j,pixel);
			errterm+=d_y;
			if (errterm>=d_x) {
				errterm-=d_x;
				act_y+=incy;
			} // if 
			act_x+=incx;
		} // for 
	} else {
		/* yline */ 
		for(a=0;a<=d_y;a++) {
			for(i=dot_x1;i<dot_x2;i++)
				for(j=dot_x1;j<dot_x2;j++)
					putpixel(sfc,act_x+i,act_y+j,pixel);
			errterm+=d_x;
			if (errterm>=d_y) {
				errterm-=d_y;
				act_x+=incx;
			} // if 
			act_y+=incy;
		} // for 
	} // if 

	SDL_UnlockSurface(sfc);
} /* draw_thick_line */ 


void surface_automatic_alpha(SDL_Surface *sfc)
{
	int i,j;
	Uint32 color;
    Uint8 r,g,b,a;

	for(i=0;i<sfc->w;i++) {
		for(j=0;j<sfc->h;j++) {                
			SDL_LockSurface(sfc);
            color=getpixel(sfc,i,j);
			SDL_UnlockSurface(sfc);
			SDL_GetRGBA(color,sfc->format,&r,&g,&b,&a);
			if (r!=0 || g!=0 || b!=0) a=255;
                                             else a=0;
			color=SDL_MapRGBA(sfc->format,r,g,b,a);

			SDL_LockSurface(sfc);
			putpixel(sfc,i,j,color);
			SDL_UnlockSurface(sfc);
		} // for 
	} // for 
} /* surface_automatic_alpha */ 


void surface_bw(SDL_Surface *sfc,int threshold)
{
	int i,j;
	Uint32 color;
    Uint8 r,g,b,a;

	for(i=0;i<sfc->w;i++) {
		for(j=0;j<sfc->h;j++) {                
			SDL_LockSurface(sfc);
            color=getpixel(sfc,i,j);
			SDL_UnlockSurface(sfc);
			SDL_GetRGBA(color,sfc->format,&r,&g,&b,&a);
			if (r>=threshold || g>=threshold || b>=threshold) a=255;
														 else a=0;
			color=SDL_MapRGBA(sfc->format,a,a,a,a);

			SDL_LockSurface(sfc);
			putpixel(sfc,i,j,color);
			SDL_UnlockSurface(sfc);
		} // for 
	} // for 
} /* surface_bw */ 


void surface_mask_from_bitmap(SDL_Surface *sfc,SDL_Surface *mask,int x,int y)
{
	int i,j;
	int mean;
	Uint32 color;
    Uint8 r,g,b,a;

	for(i=0;i<sfc->w;i++) {
		for(j=0;j<sfc->h;j++) {
			SDL_LockSurface(mask);
			color=getpixel(mask,x+i,y+j);
			SDL_UnlockSurface(mask);
			SDL_GetRGBA(color,sfc->format,&r,&g,&b,&a);
                        mean=(r+g+b)/3;
			SDL_LockSurface(sfc);
			color=getpixel(sfc,i,j);
			SDL_UnlockSurface(sfc);
			SDL_GetRGBA(color,sfc->format,&r,&g,&b,&a);
			color=SDL_MapRGBA(sfc->format,r,g,b,mean);
			SDL_LockSurface(sfc);
			putpixel(sfc,i,j,color);
			SDL_UnlockSurface(sfc);
		} // for 
	} // for 
} /* surface_mask_from_bitmap */ 


bool segment_collision(int s1x1,int s1y1,int s1x2,int s1y2,
					   int s2x1,int s2y1,int s2x2,int s2y2,
					   int *cx,int *cy)
{
	float v1x,v1y;
	float v2x,v2y;
	float l1,l2;
	float det;

	v1x=float(s1x2-s1x1);
	v1y=float(s1y2-s1y1);
	v2x=float(s2x2-s2x1);
	v2y=float(s2y2-s2y1);

	det=v1y*v2x-v1x*v2y;

	if (det!=0) {
		l1=( v2x*(s2y1-s1y1) - v2y*(s2x1-s1x1) ) / det;
		l2=( v1x*(s2y1-s1y1) - v1y*(s2x1-s1x1) ) / det;

		if (l1>=0 && l1<=1 &&
			l2>=0 && l2<=1) {
			*cx=int(s1x1+v1x*l1);
			*cy=int(s1y1+v1y*l1);
			return true;
		} // if 
	} else {
		if (s1y1==s2y1) {
			*cx=s1x1;
			*cy=s1y1;
			return true;
		} // if 
	} // if 

	return false;
} /* segment_collision */ 



void surface_blit_alpha(SDL_Surface *orig,SDL_Rect *o_r,SDL_Surface *dest,SDL_Rect *d_r)
{
	int i,j;
	SDL_Rect r1,r2;
	Uint8 *opixels,*dpixels;
	int a1,a2;
	int ap,app;

	if (orig->format->BytesPerPixel!=4 ||
		dest->format->BytesPerPixel!=4) return;

	if (o_r!=0) {
		r1=*o_r;
	} else {
		r1.x=0;
		r1.y=0;
		r1.w=orig->w;
		r1.h=orig->h;
	} // if 

	if (d_r!=0) {
		r2=*d_r;
	} else {
		r2.x=0;
		r2.y=0;
	} // if 

	if (r1.x<0) {
		r1.w+=r1.x;
		r1.x=0;
	} // if 
	if (r1.x>=orig->w) return;
	if (r1.y<0) {
		r1.h+=r1.y;
		r1.y=0;
	} // if 
	if (r1.y>=orig->h) return;

	if (r2.x<0) {
		r1.w+=r2.x;
		r2.x=0;
	} // if 
	if (r2.x>=dest->w) return;
	if (r2.y<0) {
		r1.h+=r2.y;
		r2.y=0;
	} // if 
	if (r2.y>=dest->h) return;
	if (r1.w<=0) return;
	if (r2.x+r1.w>dest->w) r1.w=dest->w-r2.x;
	if (r1.h<=0) return;
	if (r2.y+r1.h>dest->h) r1.h=dest->h-r2.y;

	SDL_LockSurface(orig);
	SDL_LockSurface(dest);

	for(i=0;i<r1.h;i++) {
		opixels=(Uint8 *)orig->pixels+orig->pitch*(i+r1.y)+r1.x*4;
		dpixels=(Uint8 *)dest->pixels+dest->pitch*(i+r2.y)+r2.x*4;
		for(j=0;j<r1.w;j++,opixels+=4,dpixels+=4) {
			a2=opixels[AOFFSET];//*alpha;
			if (a2!=0) {
				a1=dpixels[AOFFSET];
//				a2/=255;
				ap=a1+a2-(a1*a2)/255;
				if (ap>255) ap=255;
				if (ap<0) ap=0;
				if (a2==0) app=0;
					  else app=(-a2*255)/(((a1*a2)/255)-a1-a2);
				if (app>255) app=255;
				if (app<0) app=0;
				dpixels[ROFFSET]=(opixels[ROFFSET]*app+dpixels[ROFFSET]*(255-app))/255;
				dpixels[GOFFSET]=(opixels[GOFFSET]*app+dpixels[GOFFSET]*(255-app))/255;
				dpixels[BOFFSET]=(opixels[BOFFSET]*app+dpixels[BOFFSET]*(255-app))/255;
				dpixels[AOFFSET]=ap;
			} // if 
		} // for 
	} // for 

	SDL_UnlockSurface(orig);
	SDL_UnlockSurface(dest);
} /* surface_blit_alpha */ 



bool save_float(FILE *fp,float f)
{
	unsigned char *p;
	p=(unsigned char *)&f;
	fputc(p[0],fp);
	fputc(p[1],fp);
	fputc(p[2],fp);
	fputc(p[3],fp);

	return true;
} /* save_float */ 


bool load_float(FILE *fp,float *f)
{
	unsigned char *p;
	p=(unsigned char *)f;
	p[0]=fgetc(fp);
	p[1]=fgetc(fp);
	p[2]=fgetc(fp);
	p[3]=fgetc(fp);

	return true;
} /* load_float */ 


bool save_int(FILE *fp,int f)
{
	unsigned char *p;
	p=(unsigned char *)&f;
	fputc(p[0],fp);
	fputc(p[1],fp);
	fputc(p[2],fp);
	fputc(p[3],fp);

	return true;
} /* save_int */ 


bool load_int(FILE *fp,int *f)
{
	unsigned char *p;
	p=(unsigned char *)f;
	p[0]=fgetc(fp);
	p[1]=fgetc(fp);
	p[2]=fgetc(fp);
	p[3]=fgetc(fp);

	return true;
} /* load_int */ 



double sin_degree(double degree)
{
	return sin(degree*M_PI/180.0);
}


double cos_degree(double degree)
{
	return cos(degree*M_PI/180.0);
}


