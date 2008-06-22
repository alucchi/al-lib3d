/**
* File : vector3.inl
* Description : 3D Math library : 3D Vector routines
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include <math.h>

//------------------------------------------------------------------- FUNCTIONS
template <class T>
inline CVector3<T>::CVector3(T X, T Y, T Z) :
x(X),
y(Y),
z(Z)
{

}

template <class T>
inline CVector3<T>::CVector3(const CVector3& v)
{
	x=v.x;
	y=v.y;
	z=v.z;
}

template <class T>
inline void CVector3<T>::Set(T X, T Y, T Z)
{
    x = X;
    y = Y;
    z = Z;
}

template <class T>
inline T CVector3<T>::Length() const
{
    return sqrtf(LengthSq());
}

template <class T>
inline T CVector3<T>::LengthSq() const
{
    return x * x + y * y + z * z;
}

template <class T>
inline void CVector3<T>::Normalize()
{
    T Norm = Length();

    if (fabs(Norm) > std::numeric_limits<T>::epsilon())
    {
        x /= Norm;
        y /= Norm;
        z /= Norm;
    }
}

template <class T>
inline CVector3<T> CVector3<T>::operator +() const
{
    return this;
}

template <class T>
inline CVector3<T> CVector3<T>::operator -() const
{
    return CVector3<T>(-x, -y, -z);
}

template <class T>
inline CVector3<T> CVector3<T>::operator +(const CVector3<T>& v) const
{
    return CVector3<T>(x + v.x, y + v.y, z + v.z);
}

template <class T>
inline CVector3<T> CVector3<T>::operator -(const CVector3<T>& v) const
{
    return CVector3<T>(x - v.x, y - v.y, z - v.z);
}

template <class T>
inline CVector3<T> CVector3<T>::operator *(const CVector3<T>& v) const
{
    return CVector3<T>(x * v.x, y * v.y, z * v.z);
}

template <class T>
inline const CVector3<T>& CVector3<T>::operator +=(const CVector3<T>& v)
{
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

template <class T>
inline const CVector3<T>& CVector3<T>::operator -=(const CVector3<T>& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
}

template <class T>
inline const CVector3<T>& CVector3<T>::operator *=(T t)
{
    x *= t;
    y *= t;
    z *= t;

    return *this;
}

template <class T>
inline CVector3<T> CVector3<T>::operator *(const float f) const
{
	return CVector3<T>(x*f, y*f, z*f);
}

template <class T>
inline const CVector3<T>& CVector3<T>::operator /=(T t)
{
    x /= t;
    y /= t;
    z /= t;

    return *this;
}

template <class T>
inline bool CVector3<T>::operator ==(const CVector3<T>& v) const
{
    return ((std::abs(x - v.x) <= std::numeric_limits<T>::epsilon()) &&
            (std::abs(y - v.y) <= std::numeric_limits<T>::epsilon()) &&
            (std::abs(z - v.z) <= std::numeric_limits<T>::epsilon()));
}

template <class T>
inline bool CVector3<T>::operator !=(const CVector3<T>& v) const
{
    return !(*this == v);
}

template <class T>
inline CVector3<T>::operator T*()
{
    return &x;
}

template <class T>
inline CVector3<T> operator *(const CVector3<T>& v, T t)
{
    return CVector3<T>(v.x * t, v.y * t, v.z * t);
}

template <class T>
inline CVector3<T> operator /(const CVector3<T>& v, T t)
{
    return CVector3<T>(v.x / t, v.y / t, v.z / t);
}

template <class T>
inline CVector3<T> operator *(T t, const CVector3<T>& v)
{
    return v * t;
}

template <class T>
inline T Dot(const CVector3<T>& v1, const CVector3<T>& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <class T>
inline CVector3<T> Cross(const CVector3<T>& v1, const CVector3<T>& v2)
{
    return CVector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

template <class T>
inline std::istream& operator >>(std::istream& Stream, CVector3<T>& Vector)
{
    return Stream >> Vector.x >> Vector.y >> Vector.z;
}

template <class T>
inline std::ostream& operator <<(std::ostream& Stream, const CVector3<T>& Vector)
{
    return Stream << Vector.x << " " << Vector.y << " " << Vector.z;
}

/*!
 * Vector by matrix product.
 */

template <class T>
CVector3<T> operator*(const CVector3<T>& vec, const Mat4x4& mat)
{
	return Vector3(vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31 + mat._41,
				vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32 + mat._42,
				vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33 + mat._43);
}

template <class T>
CVector3<T> operator*(const Mat4x4& mat, const CVector3<T>& vec) {
	return Vector3(vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31 + mat._41,
				vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32 + mat._42,
				vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33 + mat._43);
}

template <class T>
CVector3<T>& CVector3<T>::operator*=(const Mat4x4& mat) {
	*this = *this * mat;
	return *this;
}
