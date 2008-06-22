/**
* File : vector3.h
* Description : 3D Math library : 3D Vector routines
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

#ifndef VECTOR3_H
#define VECTOR3_H

//-------------------------------------------------------------------- INCLUDES
#include <limits>
#include <iostream>
#include "Matrix4.h"

//--------------------------------------------------------------------- CLASSES

template <class T>
class CVector3
{
public :
	/*!
	 *  vector values
	 */
	T x, y, z;

    CVector3(T X = 0, T Y = 0, T Z = 0);
	CVector3(const CVector3& v);

    void Set(T X, T Y, T Z);
    T Length() const;
    T LengthSq() const;
    void Normalize();

    CVector3<T> operator +() const;
    CVector3<T> operator -() const;

    CVector3<T> operator +(const CVector3<T>& v) const;
    CVector3<T> operator -(const CVector3<T>& v) const;
	CVector3<T> operator *(const CVector3<T>& v) const;

    const CVector3<T>& operator +=(const CVector3<T>& v);
    const CVector3<T>& operator -=(const CVector3<T>& v);

    const CVector3<T>& operator *=(T t);
    const CVector3<T>& operator /=(T t);

    bool operator ==(const CVector3<T>& v) const;
    bool operator !=(const CVector3<T>& v) const;

    operator T*();

	CVector3<T> operator *(const float f) const;
	CVector3<T>& operator*=(const Mat4x4& mat);
};


/*!
 *  Global functions
 */
template <class T> CVector3<T>   operator * (const CVector3<T>& v, T t);
template <class T> CVector3<T>   operator / (const CVector3<T>& v, T t);
template <class T> CVector3<T>   operator * (T t, const CVector3<T>& v);
template <class T> T             Dot  (const CVector3<T>& v1, const CVector3<T>& v2);
template <class T> CVector3<T>   Cross(const CVector3<T>& v1, const CVector3<T>& v2);
template <class T> std::istream& operator >>(std::istream& Stream, CVector3<T>& Vector);
template <class T> std::ostream& operator <<(std::ostream& Stream, const CVector3<T>& Vector);

/*!
 * Vector by matrix product. Returns a vector since:
 * 
 * 1x3 matrix ( the vector ) by 4x4 matrix results in 1x4 matrix. After we strip
 * the last (unused) row element, we get a new vector.
 */
template <class T> CVector3<T> operator*(const CVector3<T>& vec, const Mat4x4& mat);
template <class T> CVector3<T> operator*(const Mat4x4& mat, const CVector3<T>& vec);

//----------------------------------------------------------------------- TYPES
typedef CVector3<int>   Vector3i;
typedef CVector3<float> Vector3;

//---------------------------------------------------------------------- INLINES
#include "Vector3.inl"

#endif // VECTOR3_H