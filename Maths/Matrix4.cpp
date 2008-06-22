/**
* File : matrix4.cpp
* Description : 3D Math library : 4x4 matrix routines
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

#include "Matrix4.h"
#include "math3D.h"

Mat4x4::Mat4x4(void) {
	identity();
}

Mat4x4::Mat4x4(float e11, float e12, float e13, float e14,
			   float e21, float e22, float e23, float e24,
			   float e31, float e32, float e33, float e34,
			   float e41, float e42, float e43, float e44) {
	_11 = e11; _12 = e12; _13 = e13; _14 = e14;
	_21 = e21; _22 = e22; _23 = e23; _24 = e24;
	_31 = e31; _32 = e32; _33 = e33; _34 = e34;
	_41 = e41; _42 = e42; _43 = e43; _44 = e44;
}

Mat4x4::Mat4x4(const Mat4x4& mat) {
	_11 = mat._11; _12 = mat._12; _13 = mat._13; _14 = mat._14;
	_21 = mat._21; _22 = mat._22; _23 = mat._23; _24 = mat._24;
	_31 = mat._31; _32 = mat._32; _33 = mat._33; _34 = mat._34;
	_41 = mat._41; _42 = mat._42; _43 = mat._43; _44 = mat._44;		
}

void Mat4x4::set(float e11, float e12, float e13, float e14,
		 		 float e21, float e22, float e23, float e24,
				 float e31, float e32, float e33, float e34,
				 float e41, float e42, float e43, float e44) {
	_11 = e11; _12 = e12; _13 = e13; _14 = e14;
	_21 = e21; _22 = e22; _23 = e23; _24 = e24;
	_31 = e31; _32 = e32; _33 = e33; _34 = e34;
	_41 = e41; _42 = e42; _43 = e43; _44 = e44;
}
	
void Mat4x4::zero(void) {
	_11 = 0; _12 = 0; _13 = 0; _14 = 0;
	_21 = 0; _22 = 0; _23 = 0; _24 = 0;
	_31 = 0; _32 = 0; _33 = 0; _34 = 0;
	_41 = 0; _42 = 0; _43 = 0; _44 = 0;
}

void Mat4x4::identity(void) {
	_11 = 1; _12 = 0; _13 = 0; _14 = 0;
	_21 = 0; _22 = 1; _23 = 0; _24 = 0;
	_31 = 0; _32 = 0; _33 = 1; _34 = 0;
	_41 = 0; _42 = 0; _43 = 0; _44 = 1;
}

void Mat4x4::scale(float sx, float sy, float sz) {
	Mat4x4 temp = *this, matScale;
	matScale._11 = sx;
	matScale._22 = sy;
	matScale._33 = sz;
	*this = matScale * temp;
}
void Mat4x4::translate(float tx, float ty, float tz) {
	Mat4x4 temp = *this, matTrans;
	matTrans._41 = tx;
	matTrans._42 = ty;
	matTrans._43 = tz;
	*this = matTrans * temp;
}

void Mat4x4::rotate(float rx, float ry, float rz) {
	Mat4x4 temp(*this), rotX, rotY, rotZ;
	
	long x = (long)rx;	
	long y = (long)ry;
	long z = (long)rz;
	
	x = x % 360;
	y = y % 360;
	z = z % 360;

	if(x < 0) x += 360;
	if(y < 0) y += 360;
	if(z < 0) z += 360;

	rotX._22 = COS(x);
	rotX._23 = SIN(x);
	rotX._32 = -SIN(x);
	rotX._33 = COS(x);

	rotY._11 = COS(y);
	rotY._13 = -SIN(y);
	rotY._31 = SIN(y);
	rotY._33 = COS(y);

	rotZ._11 = COS(z);
	rotZ._12 = SIN(z);
	rotZ._21 = -SIN(z);
	rotZ._22 = COS(z);

	*this = rotX * temp;
	temp = rotY * *this;
	*this = rotZ * temp;
}

Mat4x4& Mat4x4::operator+=(const Mat4x4& mat) {
/*	int i, j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			m[i][j] += mat.m[i][j];
		}
	}*/
	
    _11 += mat._11;_12 += mat._12;_13 += mat._13;_14 += mat._14;
    _21 += mat._21;_22 += mat._22;_23 += mat._23;_24 += mat._24;
    _31 += mat._31;_32 += mat._32;_33 += mat._33;_34 += mat._34;
    _41 += mat._41;_42 += mat._42;_43 += mat._43;_44 += mat._44;	
	
	return *this;
}

Mat4x4& Mat4x4::operator-=(const Mat4x4& mat) {
/*	int i, j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			m[i][j] -= mat.m[i][j];
		}
	}
*/	

    _11 -= mat._11;_12 -= mat._12;_13 -= mat._13;_14 -= mat._14;
    _21 -= mat._21;_22 -= mat._22;_23 -= mat._23;_24 -= mat._24;
    _31 -= mat._31;_32 -= mat._32;_33 -= mat._33;_34 -= mat._34;
    _41 -= mat._41;_42 -= mat._42;_43 -= mat._43;_44 -= mat._44;
    
    return *this;
}

Mat4x4& Mat4x4::operator*=(const Mat4x4& mat) {
	float a, b, c, d;
/*	int i;
	for(i = 0; i < 4;i++) {
		a = m[i][0];
		b = m[i][1];
		c = m[i][2];
		d = m[i][3];

		m[i][0] = a * mat.m[0][0] + b * mat.m[1][0] + c * mat.m[2][0] + d * mat.m[3][0];
		m[i][1] = a * mat.m[0][1] + b * mat.m[1][1] + c * mat.m[2][1] + d * mat.m[3][1];
		m[i][2] = a * mat.m[0][2] + b * mat.m[1][2] + c * mat.m[2][2] + d * mat.m[3][2];
		m[i][3] = a * mat.m[0][3] + b * mat.m[1][3] + c * mat.m[2][3] + d * mat.m[3][3];
	}
*/	
    
    a=_11,b=_12,c=_13,d=_14;
    _11 = a * mat._11 + b * mat._21 + c * mat._31 + d * mat._41;
    _12 = a * mat._12 + b * mat._22 + c * mat._32 + d * mat._42;
    _13 = a * mat._13 + b * mat._23 + c * mat._33 + d * mat._43;
    _14 = a * mat._14 + b * mat._24 + c * mat._34 + d * mat._44;
    
    a=_21,b=_22,c=_23,d=_24;
    _21 = a * mat._11 + b * mat._21 + c * mat._31 + d * mat._41;
    _22 = a * mat._12 + b * mat._22 + c * mat._32 + d * mat._42;
    _23 = a * mat._13 + b * mat._23 + c * mat._33 + d * mat._43;
    _24 = a * mat._14 + b * mat._24 + c * mat._34 + d * mat._44;
    
    a=_31,b=_32,c=_33,d=_34;
    _31 = a * mat._11 + b * mat._21 + c * mat._31 + d * mat._41;
    _32 = a * mat._12 + b * mat._22 + c * mat._32 + d * mat._42;
    _33 = a * mat._13 + b * mat._23 + c * mat._33 + d * mat._43;
    _34 = a * mat._14 + b * mat._24 + c * mat._34 + d * mat._44;

    a=_41,b=_42,c=_43,d=_44;
    _41 = a * mat._11 + b * mat._21 + c * mat._31 + d * mat._41;
    _42 = a * mat._12 + b * mat._22 + c * mat._32 + d * mat._42;
    _43 = a * mat._13 + b * mat._23 + c * mat._33 + d * mat._43;
    _44 = a * mat._14 + b * mat._24 + c * mat._34 + d * mat._44;

    return *this;
}

Mat4x4& Mat4x4::operator*=(float s) {
/*	int i, j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			m[i][j] *= s;
		}
	}
*/

    _11 *= s; _12 *= s; _13 *= s; _14 *= s;
    _21 *= s; _22 *= s; _23 *= s; _24 *= s;
    _31 *= s; _32 *= s; _33 *= s; _34 *= s;
    _41 *= s; _42 *= s; _43 *= s; _44 *= s;

	return *this;
}

Mat4x4 operator+(const Mat4x4& m1, const Mat4x4& m2) {
	Mat4x4 mat;
/*	int i, j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			mat.m[i][j] = m1.m[i][j] + m2.m[i][j];		
		}
	}
*/

    mat._11 = m1._11 + m2._11;mat._12 = m1._11 + m2._12;mat._13 = m1._11 + m2._13;mat._14 = m1._11 + m2._14;
    mat._21 = m1._11 + m2._21;mat._22 = m1._11 + m2._22;mat._23 = m1._11 + m2._23;mat._24 = m1._11 + m2._24;
    mat._31 = m1._11 + m2._31;mat._32 = m1._11 + m2._32;mat._33 = m1._11 + m2._33;mat._34 = m1._11 + m2._34;
    mat._41 = m1._11 + m2._41;mat._42 = m1._11 + m2._42;mat._43 = m1._11 + m2._43;mat._44 = m1._11 + m2._44;

	return mat;	
}

Mat4x4 operator-(const Mat4x4& m1, const Mat4x4& m2) {
	Mat4x4 mat;
/*	int i, j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			mat.m[i][j] = m1.m[i][j] - m2.m[i][j];		
		}
	}
*/	
    mat._11 = m1._11 - m2._11;mat._12 = m1._11 - m2._12;mat._13 = m1._11 - m2._13;mat._14 = m1._11 - m2._14;
    mat._21 = m1._11 - m2._21;mat._22 = m1._11 - m2._22;mat._23 = m1._11 - m2._23;mat._24 = m1._11 - m2._24;
    mat._31 = m1._11 - m2._31;mat._32 = m1._11 - m2._32;mat._33 = m1._11 - m2._33;mat._34 = m1._11 - m2._34;
    mat._41 = m1._11 - m2._41;mat._42 = m1._11 - m2._42;mat._43 = m1._11 - m2._43;mat._44 = m1._11 - m2._44;
    
    return mat;	

}

Mat4x4 operator*(const Mat4x4& m1, const Mat4x4& m2) {
	Mat4x4 mat;
    float a, b, c, d;
/*	int i;
	for(i = 0; i < 4; i++) {
		a = m1.m[i][0];
		b = m1.m[i][1];
		c = m1.m[i][2];
		d = m1.m[i][3];

		mat.m[i][0] = a * m2.m[0][0] + b * m2.m[1][0] + c * m2.m[2][0] + d * m2.m[3][0];
		mat.m[i][1] = a * m2.m[0][1] + b * m2.m[1][1] + c * m2.m[2][1] + d * m2.m[3][1];
		mat.m[i][2] = a * m2.m[0][2] + b * m2.m[1][2] + c * m2.m[2][2] + d * m2.m[3][2];
		mat.m[i][3] = a * m2.m[0][3] + b * m2.m[1][3] + c * m2.m[2][3] + d * m2.m[3][3];
	}
*/	
    a=m1._11,b=m1._12,c=m1._13,d=m1._14;
    mat._11 = a * m2._11 + b * m2._21 + c * m2._31 + d * m2._41;
    mat._12 = a * m2._12 + b * m2._22 + c * m2._32 + d * m2._42;
    mat._13 = a * m2._13 + b * m2._23 + c * m2._33 + d * m2._43;
    mat._14 = a * m2._14 + b * m2._24 + c * m2._34 + d * m2._44;

    a=m1._21,b=m1._22,c=m1._23,d=m1._24;
    mat._21 = a * m2._11 + b * m2._21 + c * m2._31 + d * m2._41;
    mat._22 = a * m2._12 + b * m2._22 + c * m2._32 + d * m2._42;
    mat._23 = a * m2._13 + b * m2._23 + c * m2._33 + d * m2._43;
    mat._24 = a * m2._14 + b * m2._24 + c * m2._34 + d * m2._44;

    a=m1._31,b=m1._32,c=m1._33,d=m1._34;
    mat._31 = a * m2._11 + b * m2._21 + c * m2._31 + d * m2._41;
    mat._32 = a * m2._12 + b * m2._22 + c * m2._32 + d * m2._42;
    mat._33 = a * m2._13 + b * m2._23 + c * m2._33 + d * m2._43;
    mat._34 = a * m2._14 + b * m2._24 + c * m2._34 + d * m2._44;

    a=m1._41,b=m1._42,c=m1._43,d=m1._44;
    mat._41 = a * m2._11 + b * m2._21 + c * m2._31 + d * m2._41;
    mat._42 = a * m2._12 + b * m2._22 + c * m2._32 + d * m2._42;
    mat._43 = a * m2._13 + b * m2._23 + c * m2._33 + d * m2._43;
    mat._44 = a * m2._14 + b * m2._24 + c * m2._34 + d * m2._44;

    return mat;
}

Mat4x4 operator*(const Mat4x4& mat, float s) {
	Mat4x4 rMat;
/*	int i, j;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			rMat.m[i][j] = mat.m[i][j] * s;
		}	
	}
*/	
    rMat._11 = mat._11 * s; rMat._12 = mat._11 * s; rMat._13 = mat._11 * s; rMat._14 = mat._11 * s;
    rMat._21 = mat._11 * s; rMat._22 = mat._11 * s; rMat._23 = mat._11 * s; rMat._24 = mat._11 * s;
    rMat._31 = mat._11 * s; rMat._32 = mat._11 * s; rMat._33 = mat._11 * s; rMat._34 = mat._11 * s;
    rMat._41 = mat._11 * s; rMat._42 = mat._11 * s; rMat._43 = mat._11 * s; rMat._44 = mat._11 * s;
    
    return rMat;
}

Mat4x4 operator*(float s, const Mat4x4& mat) {
	return (mat * s);
}