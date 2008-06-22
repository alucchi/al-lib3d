/**
* File : vector2.h
* Description : 3D Math library : 2D Vector routines
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

#ifndef VECTOR2_H
#define VECTOR2_H

//-------------------------------------------------------------------- INCLUDES
#include <limits>
#include <iostream>

//--------------------------------------------------------------------- CLASSES

template <class T>
class CVector2
{
public :
	/*!
	 *  vector values
	 */
	T x, y;

	/*!
	 *  \brief Constructor
	 */
	CVector2(T X = 0, T Y = 0);

    void Set(T X, T Y);
    T Length() const;
    T LengthSq() const;
    void Normalize();


    CVector2<T> operator +() const;
    CVector2<T> operator -() const;

    CVector2<T> operator +(const CVector2<T>& v) const;
    CVector2<T> operator -(const CVector2<T>& v) const;

    const CVector2<T>& operator +=(const CVector2<T>& v);
    const CVector2<T>& operator -=(const CVector2<T>& v);

    CVector2<T> operator *(T t) const;
    CVector2<T> operator /(T t) const;

    const CVector2<T>& operator *=(T t);
    const CVector2<T>& operator /=(T t);

    bool operator ==(const CVector2<T>& v) const;
    bool operator !=(const CVector2<T>& v) const;
    
    operator T*();
};


/*!
 *  Global functions
 */
template <class T> CVector2<T>   operator * (const CVector2<T>& v, T t);
template <class T> CVector2<T>   operator / (const CVector2<T>& v, T t);
template <class T> CVector2<T>   operator * (T t, const CVector2<T>& v);
template <class T> T             Dot  (const CVector2<T>& v1, const CVector2<T>& v2);
template <class T> CVector2<T>   Cross(const CVector2<T>& v1, const CVector2<T>& v2);
template <class T> std::istream& operator >>(std::istream& Stream, CVector2<T>& Vector);
template <class T> std::ostream& operator <<(std::ostream& Stream, const CVector2<T>& Vector);

//----------------------------------------------------------------------- TYPES
typedef CVector2<int>   Vector2;
typedef CVector2<float> Vector2f;

//---------------------------------------------------------------------- INLINES
#include "Vector2.inl"

#endif // VECTOR2_H
