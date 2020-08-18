//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      quaternion.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-06-30
/// @brief     Support for quaternions

#ifndef QUATERNION_H
#define QUATERNION_H

#include <cmath>

#include "defines.h"
#include "matrix.h"
#include "vector.h"

/// Support for quaternions
class Quaternion
{
private:
    /// Vector part of quaternion
    Vector3 m_v = Vector3(0,0,0);

    /// Scalar part of quaternion
    Float m_w = 0;

public:
    /// Create an uninitialized quaternion
    Quaternion() = default;

    /// Create a quaternion with the imaginary vector v and the scalar part w
    Quaternion(const Vector3 &v, Float w) : m_v(v), m_w(w) {}

    /// Create a quaternion q = w + x*i + y*j + z*k
    Quaternion(Float x, Float y, Float z, Float w)
        : m_v(Vector3(x,y,z)), m_w(w) {}

    /// Create a copy of the quaternion q
    Quaternion(const Quaternion& q) : m_v(q.m_v), m_w(q.m_w)
    {
    }

    /// Create quaternion from rotation matrix
    explicit Quaternion(const Matrix3x3& m)
    {
        m_w = std::sqrt(1+m.trace())*0.5;

        Float x = std::copysign(0.5*std::sqrt(1+m.get(0,0)-m.get(1,1)-m.get(2,2)), m.get(1,2)-m.get(2,1));
        Float y = std::copysign(0.5*std::sqrt(1-m.get(0,0)+m.get(1,1)-m.get(2,2)), m.get(2,0)-m.get(0,2));
        Float z = std::copysign(0.5*std::sqrt(1-m.get(0,0)-m.get(1,1)+m.get(2,2)), m.get(0,1)-m.get(1,0));

        m_v = Vector3(x,y,z);
    }

    /// Compute the dot product with the quaternion q
    Float dot(const Quaternion &q) const
    {
        return m_v.dot(q.m_v) + m_w*q.m_w;
    }

    /// Get length of quaternion
    Float length() const
    {
        return std::sqrt(this->dot(*this));
    }

    /// Get squared length of quaternion
    Float length2() const
    {
        return this->dot(*this);
    }

    /// Return conjugate of quaternion: q^* = (w+v)^* = w-v
    Quaternion conjugate() const
    {
        return Quaternion(-m_v, m_w);
    }

    /// Return a normalized vector
    Quaternion normalize() const
    {
        Quaternion q = Quaternion(*this);
        return q*(1./q.length());
    }

    /// Return the corresponding rotation matrix
    Matrix3x3 toMatrix() const
    {
        // See https://en.wikipedia.org/w/index.php?title=Quaternions_and_spatial_rotation&oldid=913385002#Quaternion-derived_rotation_matrix
        Matrix3x3 m;
        const Float qw = m_w, qx = m_v[0], qy = m_v[1], qz = m_v[2];

        m.set(0,0, 1-2*(qy*qy+qz*qz));
        m.set(0,1, 2*(qx*qy-qz*qw));
        m.set(0,2, 2*(qx*qz+qy*qw));

        m.set(1,0, 2*(qx*qy+qz*qw));
        m.set(1,1, 1-2*(qx*qx+qz*qz));
        m.set(1,2, 2*(qy*qz-qx*qw));

        m.set(2,0, 2*(qx*qz-qy*qw));
        m.set(2,1, 2*(qy*qz+qx*qw));
        m.set(2,2, 1-2*(qx*qx+qy*qy));

        return m;
    }

    /// Overload unary operator +: +q where q is a quaternion
    Quaternion operator+() const
    {
        return Quaternion(m_v, m_w);
    }

    /// Overload unary operator -: -q where q is a quaternion
    Quaternion operator-() const
    {
        return Quaternion(-m_v, -m_w);
    }

    /// Overload "<<" operator
    friend std::ostream& operator<<(std::ostream& os, const Quaternion& q)
    {
        os << q.m_w << "+" << q.m_v[0] << "i+" << q.m_v[1] << "j+" << q.m_v[2] << "k";
        return os;
    }

    /// Overload * operator to support operation fValue*q
    friend Quaternion operator*(const Float fValue, const Quaternion& q)
    {
        return Quaternion(q.m_v*fValue, q.m_w*fValue);
    }

    /// Overload * operator to support operation q1*q2
    Quaternion operator*(const Quaternion& q)
    {
        Vector3 v = m_v.cross(q.m_v) + m_w*q.m_v + q.m_w*m_v;
        Float w = m_w*q.m_w-m_v.dot(q.m_v);
        return Quaternion(v, w);
    }

    /// Overload + operator to support operation q1+q2
    Quaternion operator+(const Quaternion& q) const
    {
        return Quaternion(m_v+q.m_v, m_w+q.m_w);
    }

    /// Overload - operator to support operation q1-q2
    Quaternion operator-(const Quaternion& q) const
    {
        return Quaternion(m_v-q.m_v, m_w-q.m_w);
    }

    /// Overload * operator to support operation q*fValue
    Quaternion operator*(const Float fValue) const
    {
        return Quaternion(fValue*m_v, fValue*m_w);
    }

    /// Overload += operator to support operation q+=q2
    void operator+=(const Quaternion& q)
    {
        m_v += q.m_v;
        m_w += q.m_w;
    }

    /// Overload -= operator to support operation q-=q2
    void operator-=(const Quaternion& q)
    {
        m_v -= q.m_v;
        m_w -= q.m_w;
    }

    /// Overload *= operator to support operation q*=fValue
    void operator*=(const Float fValue)
    {
        m_v *= fValue;
        m_w *= fValue;
    }

    /// Overload == operation to support comparisons
    bool operator==(const Quaternion& q) const
    {
        return m_w == q.m_w && m_v == q.m_v;
    }
};

#endif // QUATERNION_H