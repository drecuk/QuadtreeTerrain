//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ OpenGL Object Orientation Application
//	An OpenGL Vector3f Class
//	##########################################################


#ifndef VECTOR3F_H
#define VECTOR3F_H

#include "math.h"
#define PI 3.14159265358979323846264338327950288419716939937510

using namespace std;

class Vector3f
{
private:

public:
	/******************* variables *******************/
	float x;
	float y;
	float z;

	/******************* constructors *******************/
	Vector3f()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3f(float vx, float vy, float vz)
	{
		x = vx;
		y = vy;
		z = vz;
	}

	/******************* functions *******************/
	float dotProduct(Vector3f v2)
	{
		float dotProd = x*v2.x + y*v2.y + z*v2.z;

		return dotProd;
	}

	float angleDotProd(Vector3f v2)
	{
		Vector3f v1(x, y, z);	// this vector
		float angle = acos(v1.dotProduct(v2)/(v1.magnitude() * v2.magnitude()));

		return angle * (180/PI);
	}

	float magnitude()
	{
		float mag = sqrt((x*x) + (y*y) + (z*z));

		return mag;
	}

	static float distance(Vector3f v1, Vector3f v2)
	{
		float dx = v1.x - v2.x;
		float dy = v1.y - v2.y;
		float dz = v1.z - v2.z;

		return sqrt(dx*dx + dy*dy + dz*dz);
	}

	void normalise()
	{
		Vector3f v(x, y, z); // this vector

		float vMag = v.magnitude();

		//cout<<"Magnitude:"<<vMag<<endl;
		//cout<<"v.x/vMag = "<<(float)(v.x/vMag)<<endl;

		x = v.x / vMag;
		y = v.y / vMag;
		z = v.z / vMag;
	}

	Vector3f crossProduct(Vector3f v2)
	{
		Vector3f vC(0, 0, 0);

		vC.x = y*v2.z - z*v2.y;
		vC.y = z*v2.x - x*v2.z;
		vC.z = x*v2.y - y*v2.x;

		return vC;
	}

	void print()
	{
		cout<<x<<" "<<y<<" "<<z<<endl;
	}

	static Vector3f vRotate2D(float angle, Vector3f target, Vector3f pos)
	{
		// Calculate component distance
		float distX = target.x - pos.x;
		float distZ = target.z - pos.z;

		// Define a return array for global to local x and y
		Vector3f v;

		v.x = (distX * cos(angle*PI/180) + distZ * sin(angle*PI/180)); // x
		v.z = (-distX * sin(angle*PI/180) + distZ * cos(angle*PI/180)); // y

		return v;
	}

	// For OpenGL axis
	// if return value is 0, point is on line
	// if value <0, point is above line
	// if value >0 point is below line
	// v1 = {x2-x1, y2-y1}
	// v2 = {x2-xA, y2-yA}
	// xp = v1.x*v2.y - v1.y*v2.x
	static float pointOnLine2D(float x1, float y1, float x2, float y2, float px, float py)
	{
		//cout<<"v1: "<<x2<<"-"<<x1<<" | "<<y2<<"-"<<y1<<endl;
		//cout<<"v2: "<<px<<"-"<<x1<<" | "<<py<<"-"<<y1<<endl;
		Vector3f v1 = Vector3f(x2 - x1, 0, y2 - y1);
		Vector3f v2 = Vector3f(px - x1, 0, py - y1);

		// essentially a cross product between two vectors
		float v = v1.x * v2.z - v1.z * v2.x;

		//cout<<v<<"="<<v1.x<<"*"<<v2.z<<" - "<<v1.z<<"*"<<v2.x<<endl;

		return v;
	}

	// test if a point is above or below a line
	// used for calculating agents on the surfaces (plane) of terrain
	// each plane is made up of two triangles (p0, p1, p2) (p2, p1, p3)
	static bool isAboveLine(Vector3f v1, Vector3f v2, Vector3f point)
	{
		bool _isAboveLine = false;

		float v = pointOnLine2D(v1.x, v1.z, v2.x, v2.z, point.x, point.z);

		// OpenGL axis implemented here. Screen coordinate is in reverse.
		if(v >= 0)
			_isAboveLine = false;
		else if (v < 0)
			_isAboveLine = true;

			return _isAboveLine;
	}

/******************* operator overloading *******************/
	// vector summation
	Vector3f operator+(Vector3f v) const
	{
		Vector3f tempV;

		tempV.x = x + v.x;
		tempV.y = y + v.y;
		tempV.z = z + v.z;

		return tempV;
	}

	// vector subtraction
	Vector3f operator-(Vector3f v) const
	{
		Vector3f tempV;

		tempV.x = x - v.x;
		tempV.y = y - v.y;
		tempV.z = z - v.z;

		return tempV;
	}

	// vector negation
	 const Vector3f operator-() const
	 {
		return Vector3f(-x, -y, -z);
	 }

	 // vector multiplication by scalar
	Vector3f operator*(float s) const
	{
		return Vector3f(x*s, y*s, z*s);
	}

	// vector division by scalar
	Vector3f operator/(float s) const
	{
		// no checks for division by zero
		return Vector3f(x/s, y/s, z/s);
	}



	//inline Vector3f operator*(float k, const Vector3f &v)
	//{
	//	return vector3f(k*v.x, k*v.y, k*v.z);
	//}

	/******************* destructor *******************/
	~Vector3f() {	}

};



#endif
