/**
* File : matrix4.h
* Description : 3D Math library : 4x4 matrix routines
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

#ifndef MATRIX4_H
#define MATRIX4_H

//--------------------------------------------------------------------- INCLUDE
#include <stdio.h>

class Mat4x4 {

public:

	union {
		struct {
			float _11, _12, _13, _14,
				  _21, _22, _23, _24,
				  _31, _32, _33, _34,
				  _41, _42, _43, _44;
		};
		float m[4][4];					// Or how about making it row-major ?
	};

	Mat4x4(void);
	Mat4x4(float e11, float e12, float e13, float e14,
		   float e21, float e22, float e23, float e24,
		   float e31, float e32, float e33, float e34,
		   float e41, float e42, float e43, float e44);
	Mat4x4(const Mat4x4& mat);

	void set(float e11, float e12, float e13, float e14,
			 float e21, float e22, float e23, float e24,
			 float e31, float e32, float e33, float e34,
			 float e41, float e42, float e43, float e44);
	
	void zero(void);
	void identity(void);

	void scale(float sx, float sy, float sz);
	void translate(float tx, float ty, float tz);
	void rotate(float rx, float ry, float rz);	

	Mat4x4& operator+=(const Mat4x4& mat);
	Mat4x4& operator-=(const Mat4x4& mat);
	Mat4x4& operator*=(const Mat4x4& mat);
	Mat4x4& operator*=(float s);	

	friend Mat4x4 operator+(const Mat4x4& m1, const Mat4x4& m2);
	friend Mat4x4 operator-(const Mat4x4& m1, const Mat4x4& m2);
	friend Mat4x4 operator*(const Mat4x4& m1, const Mat4x4& m2);
	friend Mat4x4 operator*(const Mat4x4& mat, float s);
	friend Mat4x4 operator*(float s, const Mat4x4& mat);

	inline void output(void) {
		printf("%f %f %f %f\n"
			   "%f %f %f %f\n"
				"%f %f %f %f\n"
			   "%f %f %f %f\n",
			   _11, _12, _13, _14,
			   _21, _22, _23, _24,
			   _31, _32, _33, _34,
			   _41, _42, _43, _44);
	}
};

#endif //MATRIX4_H