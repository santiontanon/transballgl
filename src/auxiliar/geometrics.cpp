#include "stdio.h"
#include "math.h"
#include "string.h"

#include "Vector.h"

#include "geometrics.h"

void Normal (double vector1[3],double vector2[3],double resultado[3])
{
	double norma;
	resultado[0]=vector1[1]*vector2[2]-vector2[1]*vector1[2];
    resultado[1]=vector1[2]*vector2[0]-vector2[2]*vector1[0];
	resultado[2]=vector1[0]*vector2[1]-vector2[0]*vector1[1];
	norma=sqrt(resultado[0]*resultado[0]+
			   resultado[1]*resultado[1]+
			   resultado[2]*resultado[2]);
	resultado[0]/=norma;
	resultado[1]/=norma;
	resultado[2]/=norma;
} /* Normal */ 


void Normal_f(float vector1[3],float vector2[3],float resultado[3])
{
	float norma;
	resultado[0]=vector1[1]*vector2[2]-vector2[1]*vector1[2];
    resultado[1]=vector1[2]*vector2[0]-vector2[2]*vector1[0];
	resultado[2]=vector1[0]*vector2[1]-vector2[0]*vector1[1];
	norma=float(sqrt(resultado[0]*resultado[0]+
					 resultado[1]*resultado[1]+
					 resultado[2]*resultado[2]));
	resultado[0]/=norma;
	resultado[1]/=norma;
	resultado[2]/=norma;
} /* Normal_f */ 


float ObtainYAngle(Vector *v)
{
	double x=v->x,z=v->z;
	double h=sqrt(x*x+z*z);
	double cy,sy;

	if (h==0) return 0;

	cy=x/h;
	sy=z/h;
//	printf("%f %f %f\n",cy,sy,h);
	return float((atan2(cy,sy)*180))/3.141592F;
} /* ObtainYAngle */ 


void ApplyMatrix(float v[4],float matrix[16],float out[4])
{
	out[0]=v[0]*matrix[0]+v[1]*matrix[4]+v[2]*matrix[8]+v[3]*matrix[12];
	out[1]=v[0]*matrix[1]+v[1]*matrix[5]+v[2]*matrix[9]+v[3]*matrix[13];
	out[2]=v[0]*matrix[2]+v[1]*matrix[6]+v[2]*matrix[10]+v[3]*matrix[14];
	out[3]=v[0]*matrix[3]+v[1]*matrix[7]+v[2]*matrix[11]+v[3]*matrix[15];

} /* ApplyMatrix */ 


void ApplyMatrix2(float v[4],float matrix[16],float out[4])
{
	out[0]=v[0]*matrix[0]+v[1]*matrix[1]+v[2]*matrix[2]+v[3]*matrix[3];
	out[1]=v[0]*matrix[4]+v[1]*matrix[5]+v[2]*matrix[6]+v[3]*matrix[7];
	out[2]=v[0]*matrix[8]+v[1]*matrix[9]+v[2]*matrix[10]+v[3]*matrix[11];
	out[3]=v[0]*matrix[12]+v[1]*matrix[13]+v[2]*matrix[14]+v[3]*matrix[15];

} /* ApplyMatrix2 */ 


void MulMatrix(float m2[16],float m1[16],float out[16])
{
	out[0]=m1[0]*m2[0]+m1[1]*m2[4]+m1[2]*m2[8]+m1[3]*m2[12];
	out[1]=m1[0]*m2[1]+m1[1]*m2[5]+m1[2]*m2[9]+m1[3]*m2[13];
	out[2]=m1[0]*m2[2]+m1[1]*m2[6]+m1[2]*m2[10]+m1[3]*m2[14];
	out[3]=m1[0]*m2[3]+m1[1]*m2[7]+m1[2]*m2[11]+m1[3]*m2[15];

	out[4]=m1[4]*m2[0]+m1[5]*m2[4]+m1[6]*m2[8]+m1[7]*m2[12];
	out[5]=m1[4]*m2[1]+m1[5]*m2[5]+m1[6]*m2[9]+m1[7]*m2[13];
	out[6]=m1[4]*m2[2]+m1[5]*m2[6]+m1[6]*m2[10]+m1[7]*m2[14];
	out[7]=m1[4]*m2[3]+m1[5]*m2[7]+m1[6]*m2[11]+m1[7]*m2[15];

	out[8]=m1[8]*m2[0]+m1[9]*m2[4]+m1[10]*m2[8]+m1[11]*m2[12];
	out[9]=m1[8]*m2[1]+m1[9]*m2[5]+m1[10]*m2[9]+m1[11]*m2[13];
	out[10]=m1[8]*m2[2]+m1[9]*m2[6]+m1[10]*m2[10]+m1[11]*m2[14];
	out[11]=m1[8]*m2[3]+m1[9]*m2[7]+m1[10]*m2[11]+m1[11]*m2[15];

	out[12]=m1[12]*m2[0]+m1[13]*m2[4]+m1[14]*m2[8]+m1[15]*m2[12];
	out[13]=m1[12]*m2[1]+m1[13]*m2[5]+m1[14]*m2[9]+m1[15]*m2[13];
	out[14]=m1[12]*m2[2]+m1[13]*m2[6]+m1[14]*m2[10]+m1[15]*m2[14];
	out[15]=m1[12]*m2[3]+m1[13]*m2[7]+m1[14]*m2[11]+m1[15]*m2[15];

} /* MulMatrix */ 


bool PlaneLineCollision(float plane[4],float p[3],float v[3],float crossp[3])
{
	float lambda;
	float tmp1,tmp2;

	tmp1=plane[0]*v[0]+plane[1]*v[1]+plane[2]*v[2];
	if (tmp1==0) return false;

	tmp2=plane[0]*p[0]+plane[1]*p[1]+plane[2]*p[2]+plane[3];
	lambda=-tmp2/tmp1;

	crossp[0]=p[0]+v[0]*lambda;
	crossp[1]=p[1]+v[1]*lambda;
	crossp[2]=p[2]+v[2]*lambda;
	return true;
} /* PlaneLineCollision */ 



void DistributeVector(Vector v,Vector v2,Vector *r1,Vector *r2)
{
	double norma;
	double tmp;

	norma=v.normalize();

	tmp=v.x*v2.x+v.y*v2.y+v.z*v2.z;

	r1->x=tmp*v.x;
	r1->y=tmp*v.y;
	r1->z=tmp*v.z;

	r2->x=v2.x-r1->x;
	r2->y=v2.y-r1->y;
	r2->z=v2.z-r1->z;
} /* DistributeVector */ 


double determinante_d(double m[9])
{
	return m[0]*(m[4]*m[8]-m[7]*m[5]) - 
		   m[1]*(m[3]*m[8]-m[5]*m[6]) + 
		   m[2]*(m[3]*m[7]-m[6]*m[4]);
} /* determinante_d */ 


float determinante_f(float m[9])
{
	return m[0]*(m[4]*m[8]-m[7]*m[5]) - 
		   m[1]*(m[3]*m[8]-m[5]*m[6]) + 
		   m[2]*(m[3]*m[7]-m[6]*m[4]);
} /* determinante_f */ 


bool kramer_d(double m[9],double c[3],double sol[3])
{
	double mt[9];
	double d;

	d=determinante_d(m);
	if (d==0) return false;

	mt[0]=c[0];
	mt[1]=m[1];
	mt[2]=m[2];

	mt[3]=c[1];
	mt[4]=m[4];
	mt[5]=m[5];

	mt[6]=c[2];
	mt[7]=m[7];
	mt[8]=m[8];
	
	sol[0]=determinante_d(mt)/d;

	mt[0]=m[0];
	mt[1]=c[0];
	mt[2]=m[2];

	mt[3]=m[3];
	mt[4]=c[1];
	mt[5]=m[5];

	mt[6]=m[6];
	mt[7]=c[2];
	mt[8]=m[8];
	
	sol[1]=determinante_d(mt)/d;

	mt[0]=m[0];
	mt[1]=m[1];
	mt[2]=c[0];

	mt[3]=m[3];
	mt[4]=m[4];
	mt[5]=c[1];

	mt[6]=m[6];
	mt[7]=m[7];
	mt[8]=c[2];
	
	sol[2]=determinante_d(mt)/d;

	return true;
} /* kramer_d */ 


bool kramer_f(float m[9],float c[3],float sol[3])
{
	float mt[9];
	float d;

	d=determinante_f(m);
	if (d==0) return false;

	mt[0]=c[0];
	mt[1]=m[1];
	mt[2]=m[2];

	mt[3]=c[1];
	mt[4]=m[4];
	mt[5]=m[5];

	mt[6]=c[2];
	mt[7]=m[7];
	mt[8]=m[8];
	
	sol[0]=determinante_f(mt)/d;

	mt[0]=m[0];
	mt[1]=c[0];
	mt[2]=m[2];

	mt[3]=m[3];
	mt[4]=c[1];
	mt[5]=m[5];

	mt[6]=m[6];
	mt[7]=c[2];
	mt[8]=m[8];
	
	sol[1]=determinante_f(mt)/d;

	mt[0]=m[0];
	mt[1]=m[1];
	mt[2]=c[0];

	mt[3]=m[3];
	mt[4]=m[4];
	mt[5]=c[1];

	mt[6]=m[6];
	mt[7]=m[7];
	mt[8]=c[2];
	
	sol[2]=determinante_f(mt)/d;

	return true;
} /* kramer_f */ 


bool gauss_solve_d(double *m,double *ctnt,double *s,int neq)
{
	int i,j,k;
	int *order;
	bool found;
	double c,det;
	double tmp;

//	FILE *fp;

//	fp=fopen("out.txt","w");

	det=det_d(m,neq);
	if (det==0) return false;

//	fprintf(fp,"%f\n",float(det));

	order=new int[neq];
	for(i=0;i<neq;i++) order[i]=-1;

	for(i=0;i<neq;i++) {
//		int a,b;
//		fprintf(fp,"%i\n",i);
//		for(a=0;a<neq;a++) {
//			for(b=0;b<neq;b++) {
//				fprintf(fp,"%.6f  ",float(m[a*neq+b]));
//			} // for 
//			fprintf(fp,"|  %.6f\n",float(ctnt[a]));
//		} // for 

		/* Buscar una ecuación con coeficiente de la variable i diferente de 0: */ 
		for(j=0,found=false,tmp=0;j<neq;j++) {
			if (fabs(m[i+j*neq])>tmp) {
				found=true;
				for(k=0;k<i;k++) {
					if (order[k]==j) found=false;
				} // for 
				if (found) {
					order[i]=j;
					tmp=fabs(m[i+j*neq]);
				} // if 
			} // if 
		} // for 

		if (!found) return false;

		/* Colocar un 1: */ 
		c=m[i+order[i]*neq];
		for(j=0;j<neq;j++) {
			m[order[i]*neq+j]/=c;
		} // for 
		ctnt[order[i]]/=c;

		/* Llenar la columna de ceros: */ 
		for(j=0;j<neq;j++) {
			if (j!=order[i]) {
				c=m[i+j*neq];
				
				for(k=0;k<neq;k++) {
					m[j*neq+k]-=c*m[order[i]*neq+k];
				} // for 
				m[j*neq+i]=0;
				ctnt[j]-=c*ctnt[order[i]];
			} // if 
		} // for 
	} // for 

	for(i=0;i<neq;i++) {
		s[i]=ctnt[order[i]]/m[order[i]*neq+i];
	} // for 

	delete order;
	return true;
} /* gauss_solve_d */ 


double det_d(double *m,int size)
{
	double acum=0;
	double *mt;
	int i,j,k;

	if (size==1) return m[0];

	mt=new double[(size-1)*(size-1)];

	for(i=0;i<size;i++) {
		double val;
		val=m[i];
		if (val!=0) {
			if ((i&1)!=0) val=-val;
			
			for(j=0;j<size-1;j++) {
				for(k=0;k<size-1;k++) {
					if (k<i) {
						mt[j*(size-1)+k]=m[(j+1)*size+k];
					} else {
						mt[j*(size-1)+k]=m[(j+1)*size+k+1];
					} // if 
				} // for 
			} // for 

			acum+=val*det_d(mt,size-1);
		} // if 
	} // for 

	delete mt;

	return acum;
} /* det_d */ 


double distancia_a_recta(Vector p,Vector pr,Vector vr)
{
	/* Buscamos el plano que tiene como vector normal 'vr' y pasa por 'p': */ 
	/* x*vr.x + y*vr.y + z*vr.z - (p*vr) = 0 */ 

	/* Buscamos el punto de colisión entre la recta y el plano anterior: */ 
	Vector col_point;
	double lambda;
	double tmp1,tmp2;

	tmp1=vr*vr;
	tmp2=vr*pr-(p*vr);
	lambda=-tmp2/tmp1;

	col_point=pr+vr*lambda;

	return (p-col_point).norma();
} /* distancia_a_recta */ 


bool LineLineCollision(float p0[3],float v0[3],float p1[3],float v1[3],float *l1,float *l2)
{
	float m[6];
	float det;

	m[0]=v0[0];		m[1]=-v1[0];	m[2]=p1[0]-p0[0];
	m[3]=v0[1];		m[4]=-v1[1];	m[5]=p1[1]-p0[1];
	det=m[0]*m[4]-m[1]*m[3];
	if (det!=0) {
		*l1=(m[2]*m[4]-m[5]*m[1])/det;
		*l2=(m[0]*m[5]-m[3]*m[2])/det;
		return true;
	} // if 

	m[0]=v0[0];		m[1]=-v1[0];	m[2]=p1[0]-p0[0];
	m[3]=v0[2];		m[4]=-v1[2];	m[5]=p1[2]-p0[2];
	det=m[0]*m[4]-m[1]*m[3];
	if (det!=0) {
		*l1=(m[2]*m[4]-m[5]*m[1])/det;
		*l2=(m[0]*m[5]-m[3]*m[2])/det;
		return true;
	} // if 

	m[0]=v0[1];		m[1]=-v1[1];	m[2]=p1[1]-p0[1];
	m[3]=v0[2];		m[4]=-v1[2];	m[5]=p1[2]-p0[2];
	det=m[0]*m[4]-m[1]*m[3];
	if (det!=0) {
		*l1=(m[2]*m[4]-m[5]*m[1])/det;
		*l2=(m[0]*m[5]-m[3]*m[2])/det;
		return true;
	} // if 

	return false;
} /* LineLineCollision */ 

