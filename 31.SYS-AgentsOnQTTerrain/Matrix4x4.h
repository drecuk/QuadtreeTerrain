//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Matrix Class
//	##########################################################

#ifndef MATRIX4x4_H
#define MATRIX4x4_H

#include "math.h"
#include "Vector3f.h"
#include <iostream>
using namespace std;

// glLoadMatrixf(const GLfloat *matrix);
// glMultMatrixf(const GLfloat *matrix);

class Matrix4x4
{
private:
	// so that the global member &operator* can access this class members
	friend Vector3f *operator*(const Vector3f v, const Matrix4x4 M)
	{
	    Vector3f *nV = new Vector3f(
			v.x*M.m11 + v.y*M.m21 + v.z*M.m31 + M.tx,
			v.x*M.m12 + v.y*M.m22 + v.z*M.m32 + M.ty,
			v.x*M.m13 + v.y*M.m23 + v.z*M.m33 + M.tz
			);

	    return nV;
	}
	friend ostream &operator<<(ostream& output, const Matrix4x4 mat) {
		//output << "(" <<  p.x << ", " << p.y <<")";

		cout<<mat.m11<<" "<<mat.m12<<" "<<mat.m13<<" "<<mat.m14<<endl;
		cout<<mat.m21<<" "<<mat.m22<<" "<<mat.m23<<" "<<mat.m24<<endl;
		cout<<mat.m31<<" "<<mat.m32<<" "<<mat.m33<<" "<<mat.m34<<endl;
		cout<<mat.tx<<" "<<mat.ty<<" "<<mat.tz<<" "<<mat.m44<<endl;

		return output;  // for multiple << operators.
	}
public:
	/******************* variables *******************/
	float matrix[16];	// a pointer to a matrix float (first line address)
	Vector3f translation; // 3, 7, 11 in matrix is translation
	Vector3f rotation;

	// matrix elements
	float m11,		m12,		m13,		m14;		// first row
	float m21,		m22,		m23,		m24;		// second row
	float m31,		m32,		m33,		m34;	// third row
	float tx,			ty,			tz,			m44;	// fourth row


	/******************* constructors *******************/
	Matrix4x4()
	{
		// initialise a default identity matrix
		m11=1;		m12=0;		m13=0;		m14=0;
		m21=0;		m22=1;		m23=0;		m24=0;
		m31=0;		m32=0;		m33=1;		m34=0;
		tx=0;		ty=0;		tz=0;		m44=1;

		/*
		float newMatrix[16] = {
								m11,		m12,		m13,		m14,
								m21,		m22,		m23,		m24,
								m31,		m32,		m33,		m34,
								tx,			ty,			tz,			m44
							};
		*/

		matrix[0] =	m11;		matrix[1] =	m12;		matrix[2] =	m13;		matrix[3] =	m14;
		matrix[4] =	m21;		matrix[5] =	m22;		matrix[6] =	m23;		matrix[7] =	m24;
		matrix[8] =	m31;		matrix[9] =	m32;		matrix[10] = m33;		matrix[11] = m34;
		matrix[12] = tx;		matrix[13] = ty;		matrix[14] = tz;		matrix[15] = m44;

		//matrix = newMatrix;			// public matrix
	}

	Matrix4x4(float newM[16])
	{
		for(int i=0; i<16; i++)
			matrix[i] = newM[i];	// public
	}

	/******************* functions *******************/
	void translate(float x, float y, float z)
	{
		//identity();	// make matrix identity;

		tx = x;
		ty = y;
		tz = z;

		fillMatrix();
	}

	void rotateX(float angle)
	{
		//identity();	// make matrix identity;
		angle = angle*PI/180;

		m11=1;		m12=0;						m13=0;						m14=0;
		m21=0;		m22=cos(angle);		m23=-sin(angle);	m24=0;
		m31=0;		m32=sin(angle);		m33=cos(angle);		m34=0;
		//tx=0;			ty=0;							tz=0;
		m44=1;

		fillMatrix();
	}

	void rotateY(float angle)
	{
		//identity();	// make matrix identity;
		angle = angle*PI/180;

		m11=cos(angle);		m12=0;				m13=sin(angle);		m14=0;
		m21=0;						m22=1;				m23=0;						m24=0;
		m31=-sin(angle);	m32=0;				m33=cos(angle);		m34=0;
		//tx=0;							ty=0;					tz=0;
		m44=1;

		/*
		matrix[0] =	cos(angle);		matrix[1] =	0;		matrix[2] =	sin(angle);		matrix[3] =	0;
		matrix[4] =	0;						matrix[5] =	1;		matrix[6] =	0;						matrix[7] =	0;
		matrix[8] =	-sin(angle);	matrix[9] =	0;		matrix[10] = cos(angle);	matrix[11] =	0;
		matrix[12] =	0;					matrix[13] =	0;	matrix[14] = 0;						matrix[15] =	1;
		*/

		fillMatrix();
	}

	void rotateZ(float angle)
	{
		//identity();	// make matrix identity;
		angle = angle*PI/180;

		m11=cos(angle);		m12=-sin(angle);	m13=0;		m14=0;
		m21=sin(angle);		m22=cos(angle);		m23=0;		m24=0;
		m31=0;						m32=0;						m33=1;		m34=0;
		//tx=0;							ty=0;							tz=0;
		m44=1;

		/*
		matrix[0] =	cos(angle);		matrix[1] =	-sin(angle);	matrix[2] =	0;		matrix[3] =	0;
		matrix[4] =	sin(angle);		matrix[5] =	cos(angle);		matrix[6] =	0;		matrix[7] =	0;
		matrix[8] =	0;						matrix[9] =	0;						matrix[10] = 1;		matrix[11] =	0;
		matrix[12] = 0;						matrix[13] =	0;					matrix[14] = 0;		matrix[15] =	1;
		*/

		fillMatrix();
	}

	Vector3f multiply(Vector3f v)
	{
		return Vector3f(
			v.x*m11 + v.y*m21 + v.z*m31 + tx,
			v.x*m12 + v.y*m22 + v.z*m32 + ty,
			v.x*m13 + v.y*m23 + v.z*m33 + tz
			);
	}

	Matrix4x4 multiply(Matrix4x4 N)
	{
		Matrix4x4 newM;

		// First Row
		newM.m11 = m11*N.m11 + m12*N.m21 + m13*N.m31 + m14*N.tx;
		newM.m12 = m11*N.m12 + m12*N.m22 + m13*N.m32 + m14*N.ty;
		newM.m13 = m11*N.m13 + m12*N.m23 + m13*N.m33 + m14*N.tz;
		newM.m14 = m11*N.m14 + m12*N.m24 + m13*N.m34 + m14*N.m44;

		// Second Row
		newM.m21 = m21*N.m11 + m22*N.m21 + m23*N.m31 + m24*N.tx;
		newM.m22 = m21*N.m12 + m22*N.m22 + m23*N.m32 + m24*N.ty;
		newM.m23 = m21*N.m13 + m22*N.m23 + m23*N.m33 + m24*N.tz;
		newM.m24 = m21*N.m14 + m22*N.m24 + m23*N.m34 + m24*N.m44;

		// Third Row
		newM.m31 = m31*N.m11 + m32*N.m21 + m33*N.m31 + m34*N.tx;
		newM.m32 = m31*N.m12 + m32*N.m22 + m33*N.m32 + m34*N.ty;
		newM.m33 = m31*N.m13 + m32*N.m23 + m33*N.m33 + m34*N.tz;
		newM.m34 = m31*N.m14 + m32*N.m24 + m33*N.m34 + m34*N.m44;

		// Fourth Row
		newM.tx = tx*N.m11 + ty*N.m21 + tz*N.m31 + m44*N.tx;
		newM.ty = tx*N.m12 + ty*N.m22 + tz*N.m32 + m44*N.ty;
		newM.tz = tx*N.m13 + ty*N.m23 + tz*N.m33 + m44*N.tz;
		newM.m44 = tx*N.m14 + ty*N.m24 + tz*N.m34 + m44*N.m44;

		return newM;
	}

	void identity()
	{
		/*
			matrix[0] =	1;		matrix[1] =	0;		matrix[2] =	0;		matrix[3] =	0;
			matrix[4] =	0;		matrix[5] =	1;		matrix[6] =	0;		matrix[7] =	0;
			matrix[8] =	0;		matrix[9] =	0;		matrix[10] = 1;		matrix[11] =	0;
			matrix[12] =	0;	matrix[13] =	0;	matrix[14] = 0;		matrix[15] =	1;
		*/

		m11=1;		m12=0;		m13=0;		m14=0;
		m21=0;		m22=1;		m23=0;		m24=0;
		m31=0;		m32=0;		m33=1;		m34=0;
		tx=0;			ty=0;			tz=0;			m44=1;

		fillMatrix();
	}

	void zero()
	{
		/*
			matrix[0] =	0;		matrix[1] =	0;		matrix[2] =	0;		matrix[3] =	0;
			matrix[4] =	0;		matrix[5] =	0;		matrix[6] =	0;		matrix[7] =	0;
			matrix[8] =	0;		matrix[9] =	0;		matrix[10] = 0;		matrix[11] =	0;
			matrix[12] =	0;	matrix[13] =	0;	matrix[14] = 0;		matrix[15] =	0;
		*/

		m11=0;		m12=0;		m13=0;		m14=0;
		m21=0;		m22=0;		m23=0;		m24=0;
		m31=0;		m32=0;		m33=0;		m34=0;
		tx=0;			ty=0;			tz=0;			m44=0;

		fillMatrix();
	}

	void fillMatrix()
	{
		matrix[0] =	m11;		matrix[1] =	m12;		matrix[2] =	m13;		matrix[3] =	m14;
		matrix[4] =	m21;		matrix[5] =	m22;		matrix[6] =	m23;		matrix[7] =	m24;
		matrix[8] =	m31;		matrix[9] =	m32;		matrix[10] = m33;		matrix[11] = m34;
		matrix[12] = tx;		matrix[13] = ty;		matrix[14] = tz;		matrix[15] = m44;

		//return matrix;
	}

	void print()
	{
		cout<<m11<<" "<<m12<<" "<<m13<<" "<<m14<<endl;
		cout<<m21<<" "<<m22<<" "<<m23<<" "<<m24<<endl;
		cout<<m31<<" "<<m32<<" "<<m33<<" "<<m34<<endl;
		cout<<tx<<" "<<ty<<" "<<tz<<" "<<m44<<endl;
	}

	/******************* operator overloading *******************/
	Matrix4x4 operator*(const Matrix4x4 N)
	{
		Matrix4x4 newM;

		// First Row
		newM.m11 = m11*N.m11 + m12*N.m21 + m13*N.m31 + m14*N.tx;
		newM.m12 = m11*N.m12 + m12*N.m22 + m13*N.m32 + m14*N.ty;
		newM.m13 = m11*N.m13 + m12*N.m23 + m13*N.m33 + m14*N.tz;
		newM.m14 = m11*N.m14 + m12*N.m24 + m13*N.m34 + m14*N.m44;

		// Second Row
		newM.m21 = m21*N.m11 + m22*N.m21 + m23*N.m31 + m24*N.tx;
		newM.m22 = m21*N.m12 + m22*N.m22 + m23*N.m32 + m24*N.ty;
		newM.m23 = m21*N.m13 + m22*N.m23 + m23*N.m33 + m24*N.tz;
		newM.m24 = m21*N.m14 + m22*N.m24 + m23*N.m34 + m24*N.m44;

		// Third Row
		newM.m31 = m31*N.m11 + m32*N.m21 + m33*N.m31 + m34*N.tx;
		newM.m32 = m31*N.m12 + m32*N.m22 + m33*N.m32 + m34*N.ty;
		newM.m33 = m31*N.m13 + m32*N.m23 + m33*N.m33 + m34*N.tz;
		newM.m34 = m31*N.m14 + m32*N.m24 + m33*N.m34 + m34*N.m44;

		// Fourth Row
		newM.tx = tx*N.m11 + ty*N.m21 + tz*N.m31 + m44*N.tx;
		newM.ty = tx*N.m12 + ty*N.m22 + tz*N.m32 + m44*N.ty;
		newM.tz = tx*N.m13 + ty*N.m23 + tz*N.m33 + m44*N.tz;
		newM.m44 = tx*N.m14 + ty*N.m24 + tz*N.m34 + m44*N.m44;

		return newM;
	}

	// mat1 = mat2;
	Matrix4x4 &operator=(const Matrix4x4 &N)
	{
		m11 =	N.m11;		m12 = N.m12;		m13 = N.m13;		m14 = N.m14;
		m21 =	N.m21;		m22 = N.m22;		m23 = N.m23;		m24 = N.m24;
		m31 =	N.m31;		m32 = N.m32;		m33 = N.m33;		m34 = N.m34;
		tx = N.tx;			ty = N.ty;			tz = N.tz;			m44 = N.m44;

		return *this;
	}


	/******************* destructor *******************/
	~Matrix4x4()	{	}

};

/******************* global methods *******************/
// original
/*
Vector3f &operator*(const Vector3f v, const Matrix4x4 M)
{
    Vector3f nV(
		v.x*M.m11 + v.y*M.m21 + v.z*M.m31 + M.tx,
		v.x*M.m12 + v.y*M.m22 + v.z*M.m32 + M.ty,
		v.x*M.m13 + v.y*M.m23 + v.z*M.m33 + M.tz
		);

    return nV;
}
*/


#endif
