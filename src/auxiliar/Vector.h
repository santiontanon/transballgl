#ifndef __AW_VECTOR
#define __AW_VECTOR

class Vector {
public:
	Vector();
	Vector(double nx,double ny,double nz);
	Vector(const Vector &v);
	Vector(FILE *fp) {
		x=y=z=0;
		load(fp);
	} 

	Vector operator+(const Vector &v);
	Vector operator-(const Vector &v);
	Vector operator-(void);

	Vector operator^(const Vector &v);
	double operator*(const Vector &v);
	Vector operator*(double ctnt);

	Vector operator/(double ctnt);

	bool operator==(const Vector &v);
	bool operator!=(const Vector &v);

	bool zero();

	double norma(void);
	double normalize(void);

	bool load(FILE *fp);

	bool save(FILE *fp);

	double x,y,z;
}; /* Vector */ 

#endif
