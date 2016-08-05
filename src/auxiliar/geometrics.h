#ifndef __BRAIN_GEOMETRICS
#define __BRAIN_GEOMETRICS


void Normal(double vector1[3],double vector2[3],double resultado[3]);
void Normal_f(float vector1[3],float vector2[3],float resultado[3]);

float ObtainYAngle(Vector *v);

void ApplyMatrix(float v[4],float matrix[16],float out[4]);
void ApplyMatrix2(float v[4],float matrix[16],float out[4]);
void MulMatrix(float m2[16],float m1[16],float out[16]);

bool PlaneLineCollision(float plane[4],float p[3],float v[3],float crossp[3]);

bool LineLineCollision(float p0[3],float v0[3],float p1[3],float v1[3],float *l1,float *l2);

void DistributeVector(Vector v,Vector v2,Vector *r1,Vector *r2);

double determinante_d(double m[9]);
float determinante_f(float m[9]);

bool kramer_d(double m[9],double c[3],double sol[3]);
bool kramer_f(float m[9],float c[3],float sol[3]);

bool gauss_solve_d(double *m,double *ctnt,double *s,int neq);

double det_d(double *m,int size);

double distancia_a_recta(Vector p,Vector pr,Vector vr);

#endif
