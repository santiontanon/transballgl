#ifdef _WIN32
#include "windows.h"
#endif

#include "math.h"

#include "stdio.h"
#include "stdlib.h"
#include "Vector.h"
#include "geometrics.h"
#include "BB2D.h"


BB2D::BB2D()
{
	reset();
} /* BB2D::BB2D */ 


BB2D::BB2D(float *p,int np)
{
	set(p,np);
} /* BB2D::BB2D */ 


BB2D::BB2D(float *px,float *py,int np)
{
	set(px,py,np);
} /* BB2D::BB2D */ 


void BB2D::reset(void)
{
	x[0]=x[1]=0;
	y[0]=y[1]=0;
	radius=0;
	sq_radius=0;
	quick=false;
	empty=true;
} /* BB2D::reset */ 

void BB2D::set(float *p,int np)
{
	bool first=true;
	int i;

	for(i=0;i<np;i++) {
		if (first) {
			x[0]=x[1]=p[i*2];
			y[0]=y[1]=p[i*2+1];
			first=false;
		} else {
			if (p[i*2]<x[0]) x[0]=p[i*2];
			if (p[i*2]>x[1]) x[1]=p[i*2];
			if (p[i*2+1]<y[0]) y[0]=p[i*2+1];
			if (p[i*2+1]>y[1]) y[1]=p[i*2+1];
		} /* if */ 
	} /* for */ 
	if (x[0]!=x[1] && y[0]!=y[1]) empty=false;
							 else empty=true;

	float tx,ty;
	tx=float(fmax(fabs(x[0]),fabs(x[1])));
	ty=float(fmax(fabs(y[0]),fabs(y[1])));
	sq_radius=tx*tx+ty*ty;
	radius=float(sqrt(sq_radius));
} /* BB2D::set */ 


void BB2D::set(float *px,float *py,int np)
{
	bool first=true;
	int i;

	for(i=0;i<np;i++) {
		if (first) {
			x[0]=x[1]=px[i];
			y[0]=y[1]=py[i];
			first=false;
		} else {
			if (px[i]<x[0]) x[0]=px[i];
			if (px[i]>x[1]) x[1]=px[i];
			if (py[i]<y[0]) y[0]=py[i];
			if (py[i]>y[1]) y[1]=py[i];
		} /* if */ 
	} /* for */ 
	if (x[0]!=x[1] && y[0]!=y[1]) empty=false;
							 else empty=true;
	float tx,ty;
	tx=float(fmax(fabs(x[0]),fabs(x[1])));
	ty=float(fmax(fabs(y[0]),fabs(y[1])));
	sq_radius=tx*tx+ty*ty;
	radius=float(sqrt(sq_radius));
} /* BB2D::set */ 


float BB2D::get_radius(void)
{
	return radius;
} /* BB2D::get_radius */ 


float BB2D::get_sq_radius(void)
{
	return sq_radius;
} /* BB2D::get_sq_radius */ 


bool BB2D::collision(BB2D *o2)
{
	if (empty || o2->empty) return false;
	if (x[1]>o2->x[0] && x[0]<o2->x[1] &&
		y[1]>o2->y[0] && y[0]<o2->y[1]) return true;

	return false;
} /* BB2D::collision */ 


/* Uses a circle to test collisions: */ 
bool BB2D::collision_simple(float x1,float y1,BB2D *o2,float x2,float y2)
{
	float d;

	if (empty || o2->empty) return false;

	d=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
	if ((radius+o2->radius)*(radius+o2->radius)>d) return true;

	return false;
} /* BB2D::collision_simple */ 


/* Uses a circle to test collisions: */ 
bool BB2D::collision_simple(float x1,float y1,float x2,float y2)
{
	float d;

	if (empty) return false;

	d=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
	if (sq_radius>=d) return true;

	return false;
} /* BB2D::collision_simple */ 


bool BB2D::collision(float x1,float y1,float a1,BB2D *o2,float x2,float y2,float a2)
{
	float tx[4],ty[4];
	int i;

	float px2[4],py2[4];
	float s1,c1,s2,c2;
	float rad_a1;
	float rad_a2;
	float cx1,cy1,cx2,cy2;

	float q_a2[4],q_b2[4],q_c2[4],q_v2[4];

	if (!collision_simple(x1,y1,o2,x2,y2)) return false;

	a1=-a1;
	a2=-a2;

	/* Transform the CMCs: */ 
	if (quick &&
		q_x1==x1 && 
		q_y1==y1 && 
		q_a1==a1) {
		/* quick values already computed! */ 
		cx1=(q_px1[0]+q_px1[1]+q_px1[2]+q_px1[3])/4;
		cy1=(q_py1[0]+q_py1[1]+q_py1[2]+q_py1[3])/4;
	} else {

		quick=true;
		q_x1=x1;
		q_y1=y1;
		q_a1=a1;

		rad_a1=(q_a1*M_PI)/180.0F;
		
		s1=float(sin(rad_a1));
		c1=float(cos(rad_a1));

		tx[0]=x[0];		ty[0]=y[0];
		tx[1]=x[1];		ty[1]=y[0];
		tx[2]=x[1];		ty[2]=y[1];
		tx[3]=x[0];		ty[3]=y[1];
		for(i=0;i<4;i++) {
			q_px1[i]=(tx[i]*c1+ty[i]*s1)+x1;
			q_py1[i]=((-tx[i]*s1)+ty[i]*c1)+y1;
		} /* for */ 

		cx1=(q_px1[0]+q_px1[1]+q_px1[2]+q_px1[3])/4;
		cy1=(q_py1[0]+q_py1[1]+q_py1[2]+q_py1[3])/4;

		for(i=0;i<4;i++) {
			if (i==3) {
				q_a[i] = q_py1[0]-q_py1[i];
				q_b[i] = - (q_px1[0]-q_px1[i]);
				q_c[i] = q_py1[i] * (q_px1[0]-q_px1[i]) - q_px1[i] * (q_py1[0]-q_py1[i]);
			} else {
				q_a[i] = q_py1[i+1]-q_py1[i];
				q_b[i] = -(q_px1[i+1]-q_px1[i]);
				q_c[i] = q_py1[i] * (q_px1[i+1]-q_px1[i]) - q_px1[i] * (q_py1[i+1]-q_py1[i]);
			} /* if */ 
			q_v[i] = q_a[i]*cx1+q_b[i]*cy1+q_c[i];
		} /* for */ 
	} /* if */ 

	if (o2->quick &&
		o2->q_x1==x2 && 
		o2->q_y1==y2 && 
		o2->q_a1==a2) {
		/* quick values already computed! */ 
		for(i=0;i<4;i++) {
			px2[i]=o2->q_px1[i];
			py2[i]=o2->q_py1[i];
			q_a2[i]=o2->q_a[i];
			q_b2[i]=o2->q_b[i];
			q_c2[i]=o2->q_c[i];
			q_v2[i]=o2->q_v[i];
		} /* for */ 
		cx2=(px2[0]+px2[1]+px2[2]+px2[3])/4;
		cy2=(py2[0]+py2[1]+py2[2]+py2[3])/4;
	} else {	
//		o2->quick=true;
//		o2->q_x1=x2;
//		o2->q_y1=y2;
//		o2->q_a1=a2;

		rad_a2=(a2*M_PI)/180.0F;
		s2=float(sin(rad_a2));
		c2=float(cos(rad_a2));
		tx[0]=o2->x[0];		ty[0]=o2->y[0];
		tx[1]=o2->x[1];		ty[1]=o2->y[0];
		tx[2]=o2->x[1];		ty[2]=o2->y[1];
		tx[3]=o2->x[0];		ty[3]=o2->y[1];
		for(i=0;i<4;i++) {
			px2[i]=(tx[i]*c2+ty[i]*s2)+x2;
			py2[i]=((-tx[i]*s2)+ty[i]*c2)+y2;
		} /* for */ 

		cx2=(px2[0]+px2[1]+px2[2]+px2[3])/4;
		cy2=(py2[0]+py2[1]+py2[2]+py2[3])/4;

		for(i=0;i<4;i++) {
			if (i==3) {
				q_a2[i] = py2[0]-py2[i];
				q_b2[i] = - (px2[0]-px2[i]);
				q_c2[i] = py2[i] * (px2[0]-px2[i]) - px2[i] * (py2[0]-py2[i]);
			} else {
				q_a2[i] = py2[i+1]-py2[i];
				q_b2[i] = -(px2[i+1]-px2[i]);
				q_c2[i] = py2[i] * (px2[i+1]-px2[i]) - px2[i] * (py2[i+1]-py2[i]);
			} /* if */ 
			q_v2[i] = q_a2[i]*cx2+q_b2[i]*cy2+q_c2[i];
		} /* for */ 

		if (o2!=this) {
			for(i=0;i<4;i++) {
				o2->q_px1[i]=px2[i];
				o2->q_py1[i]=py2[i];
				o2->q_a[i]=q_a2[i];
				o2->q_b[i]=q_b2[i];
				o2->q_c[i]=q_c2[i];
				o2->q_v[i]=q_v2[i];
			} /* for */ 
		} // if 
	}

	/* Test whether at least one of the points of CMC1 is inside CMC2: */ 
	{
		bool inside=true;
		int j;

		for(i=0;i<4;i++) {
			inside=true;
			for(j=0;j<4 && inside;j++) {
				if (q_v2[j]*(q_a2[j]*q_px1[i]+q_b2[j]*q_py1[i]+q_c2[j])<0) inside=false;
			} /* if */ 
			if (inside) return true;
		} /* for */ 

		for(i=0;i<4;i++) {
			inside=true;
			for(j=0;j<4 && inside;j++) {
				if (q_v[j]*(q_a[j]*px2[i]+q_b[j]*py2[i]+q_c[j])<0) inside=false;
			} /* if */ 
			if (inside) return true;
		} /* for */ 

		/* Test if the center point of CMC2 is insice CMC1: */ 
		inside=true;
		for(j=0;j<4 && inside;j++) {
			if (q_v[j]*(q_a[j]*cx2+q_b[j]*cy2+q_c[j])<0) inside=false;
		} /* if */ 
		if (inside) return true;

		/* Test if the center point of CMC1 is insice CMC2: */ 
		inside=true;
		for(j=0;j<4 && inside;j++) {
			if (q_v2[j]*(q_a2[j]*cx1+q_b2[j]*cy1+q_c2[j])<0) inside=false;
		} /* if */ 
		if (inside) return true;

	}

	return false;
} /* BB2D::collision */ 



bool BB2D::collision(float x1,float y1,float a1,float x2,float y2)
{
	float tx[4],ty[4];
	int i;

	float s1,c1;
	float rad_a1;
	float cx1,cy1;

	a1=-a1;

	if (!collision_simple(x1,y1,x2,y2)) return false;

	/* Transform the CMCs: */ 
	if (quick &&
		q_x1==x1 && 
		q_y1==y1 && 
		q_a1==a1) {
		/* quick values already computed! */ 
//		cx1=(q_px1[0]+q_px1[1]+q_px1[2]+q_px1[3])/4;
//		cy1=(q_py1[0]+q_py1[1]+q_py1[2]+q_py1[3])/4;
	} else {

		quick=true;
		q_x1=x1;
		q_y1=y1;
		q_a1=a1;

		rad_a1=(q_a1*M_PI)/180.0F;
		
		s1=float(sin(rad_a1));
		c1=float(cos(rad_a1));

		tx[0]=x[0];		ty[0]=y[0];
		tx[1]=x[1];		ty[1]=y[0];
		tx[2]=x[1];		ty[2]=y[1];
		tx[3]=x[0];		ty[3]=y[1];
		for(i=0;i<4;i++) {
			q_px1[i]=(tx[i]*c1+ty[i]*s1)+x1;
			q_py1[i]=((-tx[i]*s1)+ty[i]*c1)+y1;
		} /* for */ 

		cx1=(q_px1[0]+q_px1[1]+q_px1[2]+q_px1[3])/4;
		cy1=(q_py1[0]+q_py1[1]+q_py1[2]+q_py1[3])/4;

		for(i=0;i<4;i++) {
			if (i==3) {
				q_a[i] = q_py1[0]-q_py1[i];
				q_b[i] = - (q_px1[0]-q_px1[i]);
				q_c[i] = q_py1[i] * (q_px1[0]-q_px1[i]) - q_px1[i] * (q_py1[0]-q_py1[i]);
			} else {
				q_a[i] = q_py1[i+1]-q_py1[i];
				q_b[i] = -(q_px1[i+1]-q_px1[i]);
				q_c[i] = q_py1[i] * (q_px1[i+1]-q_px1[i]) - q_px1[i] * (q_py1[i+1]-q_py1[i]);
			} /* if */ 
			q_v[i] = q_a[i]*cx1+q_b[i]*cy1+q_c[i];
		} /* for */ 
	} /* if */ 

	/* Test whether the point is inside the CMC: */ 
	{
		bool inside=true;
		int j;

		inside=true;
		for(j=0;j<4 && inside;j++) {
			if (q_v[j]*(q_a[j]*x2+q_b[j]*y2+q_c[j])<0) inside=false;
		} /* if */ 
		if (inside) return true;
	}

	return false;
} /* BB2D::collision */ 



bool BB2D::collision_point(float x1,float y1,float a1,BB2D *o2,float x2,float y2,float a2,float *cx,float *cy,float precision)
{
	float area1,area2;
	float tmp;	
	BB2D *bb1=0,*bb2=0;

	/* Select the smallest area bounding box: */ 
	area1=(x[1]-x[0])*(y[1]-y[0]);
	area2=(o2->x[1]-o2->x[0])*(o2->y[1]-o2->y[0]);
	if (area1>area2) {
		bb1=o2;
		bb2=this;
		tmp=x1;
		x1=x2;
		x2=tmp;

		tmp=y1;
		y1=y2;
		y2=tmp;

		tmp=a1;
		a1=a2;
		a2=tmp;
	} else {
		bb1=this;
		bb2=o2;
	} /* if */ 

	/* Iterate through all the points in the small bounding box and test individual collision: */ 
	{
		float accum_x=0;
		float accum_y=0;
		int n_points=0;
		float i,j;
		float x,y;

		for(i=bb1->x[0];i<=bb1->x[1];i+=precision) {
			for(j=bb1->y[0];j<=bb1->y[1];j+=precision) {
				x=float(cos((-a1*M_PI)/180.0F)*i+sin((-a1*M_PI)/180.0F)*j)+x1;
				y=float(-sin((-a1*M_PI)/180.0F)*i+cos((-a1*M_PI)/180.0F)*j)+y1;

				if (bb2->collision(x2,y2,a2,x,y)) {
					accum_x+=x;
					accum_y+=y;
					n_points++;
				} /* if */ 
			} /* for */ 
		} /* for */ 

		if (n_points>0) {
			*cx=accum_x/n_points;
			*cy=accum_y/n_points;			
			return true;
		} else {
			return false;
		} /* if */ 
		
	}

} /* BB2D::collision_point */ 


void BB2D::point_normal(float col_x,float col_y,float bx,float by,float ba,float *nx,float *ny)
{
	float px[4]={0,0,0,0};
	float py[4]={0,0,0,0};
	float c=float(cos(-ba*M_PI/180.0F));
	float s=float(sin(-ba*M_PI/180.0F));
	float closest_point_x,closest_point_y;
	int closest_side,closest_side2;
	float distance,minimum_distance=0;
	float p3[3]={col_x,col_y,0};

	int i,i2;
	float p1[3],p2[3];
	float v1[3],v2[3];
	float l1,l2;

	/* Compute transformed sides: */ 
	px[0]=(x[0]*c+y[0]*s)+bx;
	px[1]=(x[1]*c+y[0]*s)+bx;
	px[2]=(x[1]*c+y[1]*s)+bx;
	px[3]=(x[0]*c+y[1]*s)+bx;

	py[0]=(y[0]*c-x[0]*s)+by;
	py[1]=(y[0]*c-x[1]*s)+by;
	py[2]=(y[1]*c-x[1]*s)+by;
	py[3]=(y[1]*c-x[0]*s)+by;

	/* Compute the closest point to each side: */ 
	closest_side=-1;
	for(i=0;i<4;i++) {

		i2=i+1;
		if (i2>=4) i2-=4;

		p1[0]=px[i];
		p1[1]=py[i];
		p1[2]=0;
		p2[0]=px[i2];
		p2[1]=py[i2];
		p2[2]=0;
		v1[0]=p2[0]-p1[0];
		v1[1]=p2[1]-p1[1];
		v1[2]=0;
		v2[0]=-v1[1];
		v2[1]=v1[0];
		v2[2]=0;

		if (LineLineCollision(p1,v1,p3,v2,&l1,&l2)) {
			if (l1<0) l1=0;
			if (l1>1) l1=1;

			closest_point_x=p1[0]+l1*v1[0];
			closest_point_y=p1[1]+l1*v1[1];
			distance=(closest_point_x-col_x)*(closest_point_x-col_x)+(closest_point_y-col_y)*(closest_point_y-col_y);

			if (closest_side==-1 ||
				distance<minimum_distance) {
				closest_side=i;
				minimum_distance=distance;
			} /* if */ 
			
		} /* if */ 
		
	} /* for */ 

	if (closest_side>=0) {
		float n;

		closest_side2=closest_side+1;
		if (closest_side2>=4) closest_side2-=4;

		p1[0]=px[closest_side];
		p1[1]=py[closest_side];
		p1[2]=0;
		p2[0]=px[closest_side2];
		p2[1]=py[closest_side2];
		p2[2]=0;

		*nx=p2[1]-p1[1];
		*ny=-(p2[0]-p1[0]);
		n=float(sqrt((*nx)*(*nx)+(*ny)*(*ny)));
		if (n!=0) {
			*nx/=n;
			*ny/=n;
		} /* if */ 
	} else {
		*nx=1;
		*ny=0;
	} /* if */ 
} /* BB2D::point_normal */ 

