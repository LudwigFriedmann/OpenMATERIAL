//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      vector.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-04-08
/// @brief     Support for 3D vectors

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <iostream>
#include <vector>

#include "defines.h"

/// Support for three-dimensional vectors
template<class T>
class Vector {
private:
    /// Input vector
    T m_v[3] = {0,0,0};

public:
    /// Create new vector (x,y,z)
    Vector(T x, T y, T z)
        : m_v{ x, y, z }
    {}

    /// Create new vector (v[0],v[1],v[2])
    Vector(const T v[3])
        : m_v{ v[0], v[1], v[2] }
    {}

    /// Create new vector from std::vector<T>
    Vector(const std::vector<T>& v)
        : m_v{ v.at(0), v.at(1), v.at(2) }
    {}

    /// Create a copy of vector v
    Vector(const Vector& v)
        : m_v{ v.m_v[0], v.m_v[1], v.m_v[2] }
    {}

    /// Create a zero vector
    Vector(void) {}

    /// Get matrix element v[m]
    T get(int m) const { return m_v[m]; }

    /// Set matrix element v[m]=value
    void set(int m, T value) { m_v[m] = value; }

    /// Compute dot product with vector v
    T dot(const Vector& v) const
    {
        return m_v[0]*v.m_v[0] + m_v[1]*v.m_v[1] + m_v[2]*v.m_v[2];
    }

    /// Compute cross product with vector v
    Vector cross(const Vector &v3fVec) const
    {
        const T* u = m_v;
        const T* v = v3fVec.m_v;
        // cross product u x v
        return Vector(
            u[1] * v[2] - u[2] * v[1], // u_y*v_z - u_z-v_y
            u[2] * v[0] - u[0] * v[2], // u_z*v_x - u_x-v_z
            u[0] * v[1] - u[1] * v[0]  // u_x*v_y - u_y-v_x
        );
    }

    /// Return square of Euclidean length x²+y²+z² of vector.
    T length2() const
    {
        return m_v[0]*m_v[0] + m_v[1]*m_v[1] + m_v[2]*m_v[2];
    }

    /// Return Euclidean length sqrt(x²+y²+z²) of vector.
    Float length() const
    {
        // Sqrt(x²+y²+z²)
        return std::sqrt(length2());
    }

    /// Return normalized vector with Euclidean length 1
    Vector normalize() const
    {
        Float inv = 1/length();
        return Vector(inv*m_v[0], inv*m_v[1], inv*m_v[2]);
    }

    /// @brief Return two vectors orthogonal to the input vector v
    ///
    /// The orthogonal vectors are written to t1 and t2. The vectors t1, t2
    /// are normalized. The vectors in the order v,t1,t2 constitute a
    /// right-handed basis.
    ///
    /// This method does not work for the vector (0,0,0).
    ///
    /// @param [out] t1 1st vector orthogonal to input vector a
    /// @param [out] t2 2nd vector orthogonal to input vector a
    void getOrthogonalVectors(Vector& t1, Vector& t2) const
    {
        T absx = std::abs(m_v[0]), absy = std::abs(m_v[1]), absz = std::abs(m_v[2]);
        if(absx < absy && absx < absz)
            t1 = cross(Vector(1,0,0)).normalize();
        else if(absy < absz)
            t1 = cross(Vector(0,1,0)).normalize();
        else
            t1 = cross(Vector(0,0,1)).normalize();

        t2 = cross(t1).normalize();
    }

    /// Get the spherical coordinates of the vector
    void toSpherical(T& r, T& theta, T& phi) const
    {
        // Radius
        r = length();
        // Theta = acos(z/r)
        theta = std::acos(m_v[2]/r);
        // Phi = atan(y/x)
        phi = std::atan2(m_v[1], m_v[0]);		
    }

    /// Overload unary operator +: +v where v is a vector.
    Vector operator+() const
    {
        const T* u = m_v;
        return Vector(u[0], u[1], u[2]);
    }

    /// Overload unary operator -: -v where v is a vector.
    Vector operator-() const
    {
        const T* u = m_v;
        return Vector(-u[0], -u[1], -u[2]);
    }

    /// Overload "<<" operator
    friend std::ostream& operator<<(std::ostream& os, const Vector &v3fVec)
    {
        const T* u = v3fVec.m_v;
        os << "(" << u[0] << "," << u[1] << "," << u[2] << ")";
        return os;
    }

    /// Overload * operator to support operation value*v
    friend Vector operator*(const T value, const Vector& v3fVec)
    {
        const T *u = v3fVec.m_v;
        return Vector(value*u[0], value*u[1], value*u[2]);
    }

    /// Overload + operator to support operation u+v
    Vector operator+(const Vector& v3fVec) const
    {
        const T *u = m_v, *v = v3fVec.m_v;
        return Vector(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
    }

    /// Overload - operator to support operation u-v
    Vector operator-(const Vector& v3fVec) const
    {
        const T *u = m_v, *v = v3fVec.m_v;
        return Vector(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
    }

    /// Overload * operator to support operation c*value
    Vector operator*(const T value) const
    {
        const T *u = m_v;
        return Vector(value*u[0], value*u[1], value*u[2]);
    }

    /// Overload += operator to support operation v+=u
    void operator+=(const Vector& v3fVec)
    {
        T *u = m_v;
        const T *v = v3fVec.m_v;
        for(int i = 0; i < 3; i++)
            u[i] += v[i];
    }

    /// Overload -= operator to support operation v-=u
    void operator-=(const Vector& v3fVec)
    {
        T *u = m_v;
        const T *v = v3fVec.m_v;
        for(int i = 0; i < 3; i++)
            u[i] -= v[i];
    }

    /// Overload *= operator to support operation v*=2
    void operator*=(const T value)
    {
        T *u = m_v;
        for(int i = 0; i < 3; i++)
            u[i] *= value;
    }

    /// Overload = operation to support operation u=v
    void operator=(const Vector& v3fVec)
    {
        for(int i = 0; i < 3; i++)
            m_v[i] = v3fVec.m_v[i];
    }

    /// Overload == operation to support comparisons
    bool operator==(const Vector& v3fVec) const
    {
        for(int i = 0; i < 3; i++)
            if(m_v[i] != v3fVec.m_v[i])
                return false;

        return true;
    }

    /// Operator [] to get matrix elements
    T operator[](int m) const
    {
        return m_v[m];
    }

    /// Operator [] to set matrix elements
    T &operator[](int m)
    {
        return m_v[m];
    }
};

typedef Vector<Float> Vector3;

#endif // VECTOR_H

