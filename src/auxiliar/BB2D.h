#ifndef __BRAIN_BB2D
#define __BRAIN_BB2D

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

class BB2D {
public:
	BB2D();
	BB2D(float *p,int np);
	BB2D(float *x,float *y,int np);

	void reset(void);

	void set(float *p,int np);
	void set(float *x,float *y,int np);
//	void expand(BB2D *o2,float *m);

	float get_radius(void);
	float get_sq_radius(void);

	bool collision(BB2D *o2);
	bool collision(float x1,float y1,float a1,BB2D *o2,float x2,float y2,float a2);
	bool collision(float x1,float y1,float a1,float x2,float y2);
	bool collision_simple(float x1,float y1,BB2D *o2,float x2,float y2);
	bool collision_simple(float x1,float y1,float x2,float y2);
	bool collision_point(float x1,float y1,float a1,BB2D *o2,float x2,float y2,float a2,float *cx,float *cy,float precision=1.0F);
	void point_normal(float px,float py,float x,float y,float a,float *nx,float *ny);

	float x[2],y[2];
	float radius,sq_radius;
	bool empty;

	/* Quick values to compute bounding boxes: */
	bool quick;
	float q_px1[4],q_py1[4];
	float q_a[4],q_b[4],q_c[4],q_v[4];
	float q_x1,q_y1,q_a1;
};

#endif
