#include "math.h"
#include "stdio.h"
#include "Vector.h"


Vector::Vector() 
{
	x=0;
	y=0;
	z=0;
} /* Vector */ 


Vector::Vector(double nx,double ny,double nz) 
{
	x=nx;
	y=ny;
	z=nz;
} /* Vector */ 


Vector::Vector(const Vector &v)
{
	x=v.x;
	y=v.y;
	z=v.z;
} /* Vector */ 


Vector Vector::operator+(const Vector &v)
{
	Vector tmp;

	tmp.x=x+v.x;
	tmp.y=y+v.y;
	tmp.z=z+v.z;

	return tmp;
} /* operator+ */ 


Vector Vector::operator-(const Vector &v)
{
	Vector tmp;

	tmp.x=x-v.x;
	tmp.y=y-v.y;
	tmp.z=z-v.z;

	return tmp;
} /* operator- */ 


Vector Vector::operator-(void)
{
	Vector tmp;

	tmp.x=-x;
	tmp.y=-y;
	tmp.z=-z;

	return tmp;
} /* operator- */ 


Vector Vector::operator^(const Vector &v) 
{
	Vector res;

	res.x=y*v.z-v.y*z;
	res.y=z*v.x-v.z*x;
	res.z=x*v.y-v.x*y;
	return res;
} /* operator* */ 


double Vector::operator*(const Vector &v) 
{
	return x*v.x+y*v.y+z*v.z;
} /* operator* */ 


Vector Vector::operator*(double ctnt)
{
	Vector res=*this;

	res.x*=ctnt;
	res.y*=ctnt;
	res.z*=ctnt;

	return res;
} /* operator* */ 


Vector Vector::operator/(double ctnt)
{
	Vector res=*this;

	res.x/=ctnt;
	res.y/=ctnt;
	res.z/=ctnt;

	return res;
} /* operator/ */ 


bool Vector::operator==(const Vector &v)
{
	if (x!=v.x ||
		y!=v.y ||
		z!=v.z) return false;
	return true;
} /* operator== */ 


bool Vector::operator!=(const Vector &v)
{
	if (x!=v.x ||
		y!=v.y ||
		z!=v.z) return true;
	return false;
} /* operator!= */ 


double Vector::norma(void)
{
	return sqrt(x*x+y*y+z*z);
} /* norma */ 


double Vector::normalize(void) 
{
	double n=norma();

	if (n==0) return 0;

	x/=n;
	y/=n;
	z/=n;

	return n;
} /* normalize */ 


bool Vector::zero()
{
	return (x==0 && y==0 && z==0);
} /* zero */ 


bool Vector::load(FILE *fp) 
{
	float t1,t2,t3;

	if (3!=fscanf(fp,"%f %f %f",&t1,&t2,&t3)) return false;
	x=t1;
	y=t2;
	z=t3;

	return true;
} /* load */ 


bool Vector::save(FILE *fp) 
{
	fprintf(fp,"%f %f %f\n",float(x),float(y),float(z));

	return true;
} /* save */ 

