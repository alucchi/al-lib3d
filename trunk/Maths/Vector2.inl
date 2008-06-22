/**
* File : vector2.inl
* Description : 3D Math library : 2D Vector routines
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include "Vector2.h"
#include <math.h>

//------------------------------------------------------------------- FUNCTIONS

template <class T>
inline CVector2<T>::CVector2(T X, T Y) :
x(X),
y(Y)
{

}

template <class T>
inline void CVector2<T>::Set(T X, T Y)
{
    x = X;
    y = Y;
}

template <class T>
inline T CVector2<T>::Length() const
{
    return sqrt(LengthSq()); // sqrt (x²+y²)
}

template <class T>
inline T CVector2<T>::LengthSq() const
{
    return x * x + y * y;
}

template <class T>
inline void CVector2<T>::Normalize()
{
    T Norm = Length();

    if (std::abs(Norm) > std::numeric_limits<T>::epsilon())
    {
        x /= Norm;
        y /= Norm;
    }
}

template <class T>
inline CVector2<T> CVector2<T>::operator +() const
{
    return this;
}

template <class T>
inline CVector2<T> CVector2<T>::operator -() const
{
    return CVector2<T>(-x, -y);
}

template <class T>
inline CVector2<T> CVector2<T>::operator +(const CVector2<T>& v) const
{
    return CVector2<T>(x + v.x, y + v.y);
}

template <class T>
inline CVector2<T> CVector2<T>::operator -(const CVector2<T>& v) const
{
    return CVector2<T>(x - v.x, y - v.y);
}

template <class T>
inline const CVector2<T>& CVector2<T>::operator +=(const CVector2<T>& v)
{
    x += v.x;
    y += v.y;

    return *this;
}

template <class T>
inline const CVector2<T>& CVector2<T>::operator -=(const CVector2<T>& v)
{
    x -= v.x;
    y -= v.y;

    return *this;
}

template <class T>
inline CVector2<T> CVector2<T>::operator *(T t) const
{
    return CVector2<T>(x * t, y * t);
}

template <class T>
inline CVector2<T> CVector2<T>::operator /(T t) const
{
    return CVector2<T>(x / t, y / t);
}

template <class T>
inline const CVector2<T>& CVector2<T>::operator *=(T t)
{
    x *= t;
    y *= t;

    return *this;
}

template <class T>
inline const CVector2<T>& CVector2<T>::operator /=(T t)
{
    x /= t;
    y /= t;

    return *this;
}

template <class T>
inline bool CVector2<T>::operator ==(const CVector2<T>& v) const
{
    return ((std::abs(x - v.x) <= std::numeric_limits<T>::epsilon()) &&
            (std::abs(y - v.y) <= std::numeric_limits<T>::epsilon()));
}

template <class T>
inline bool CVector2<T>::operator !=(const CVector2<T>& v) const
{
    return !(*this == v);
}

template <class T>
inline CVector2<T>::operator T*()
{
    return &x;
}

template <class T>
inline CVector2<T> operator *(const CVector2<T>& v, T t)
{
    return CVector2<T>(v.x * t, v.y * t);
}

template <class T>
inline CVector2<T> operator /(const CVector2<T>& v, T t)
{
    return CVector2<T>(v.x / t, v.y / t);
}

template <class T>
inline CVector2<T> operator *(T t, const CVector2<T>& v)
{
    return v * t;
}

template <class T>
inline T Dot(const CVector2<T>& v1, const CVector2<T>& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

template <class T>
inline CVector2<T> Cross(const CVector2<T>& v1, const CVector2<T>& v2)
{
    return CVector2<T>(/* ??? */);
}

template <class T>
inline std::istream& operator >>(std::istream& Stream, CVector2<T>& Vector)
{
    return Stream >> Vector.x >> Vector.y;
}

template <class T>
inline std::ostream& operator <<(std::ostream& Stream, const CVector2<T>& Vector)
{
    return Stream << Vector.x << " " << Vector.y;
}
