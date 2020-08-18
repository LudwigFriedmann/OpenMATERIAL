//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      transformation.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Support for affine transformations

#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "matrix.h"
#include "quaternion.h"
#include "ray.h"
#include "vector.h"

/// @brief Support of affine transformations
///
/// The affine transformation have the form
/// \f[
///     f(\vec{x}) = M\vec{x}+\vec{d}
/// \f]
/// where \f$M\f$ denotes a 3x3 matrix, and \f$\vec{d}\f$ is a translation
/// vector.
///
/// The affine transformation \f$f(\vec{x})\f$ transforms a point \f$\vec x\f$
/// from local coordinates to world coordinates \f$\vec x'\f$, i.e.,
/// \f$\vec x'=f(\vec x)\f$. The transformation from world coordinates to local
/// coordinates is given by
/// \f[
///     f^{-1}(\vec x') = M^{-1} \vec x' - \vec d
/// \f]
/// where \f$M^{-1}\f$ denotes the inverse of \f$M\f$. The affine
/// transformation needs to be bijective which means that \f$M\f$ must be
/// invertible (the determinant of \f$M\f$ must not be 0).
class Transformation
{
private:
    /// 3x3 matrix of the affine transformation
    Matrix3x3 m_m;

    /// Inverse matrix of m_m
    Matrix3x3 m_mInv;

    /// Translation of the affine transformation
    Vector3 m_d;

public:
    /// Create an empty (uninitialized) object
    Transformation() = default;

    /// @brief Create transformation from 3x3 matrix and translation vector
    ///
    /// Create an affine transformation
    /// \f[
    ///     f(\vec{x}) = M\vec{x}+\vec{d}
    /// \f]
    /// from the 3x3 transformation matrix \f$M\f$ and the translation vector
    /// \f$\vec d\f$. The inverse of \f$M\f$ will be computed numerically.
    ///
    /// @param [in] M 3x3 transformation matrix \f$M\f$
    /// @param [in] d translation vector \f$\vec d\f$
    Transformation(const Matrix3x3& M, const Vector3& d)
        : m_m(M), m_mInv(M.invert()), m_d(d) {}

    /// @brief Create transformation from 3x3 matrix and translation vector
    ///
    /// This method is like \ref Transformation(const Matrix3x3& M, const Vector3& d)
    /// except that the inverse of \f$M\f$ is not computed numerically but
    /// given by MInv.
    ///
    /// @param [in] M 3x3 transformation matrix \f$M\f$
    /// @param [in] MInv inverse of \f$M\f$; \f$\mathrm{MInv} = M^{-1}\f$
    /// @param [in] d translation vector \f$\vec d\f$
    Transformation(const Matrix3x3& M, const Matrix3x3& MInv, const Vector3& d)
        : m_m(M), m_mInv(MInv), m_d(d) {}

    /// Create identity transformation
    static Transformation Identity()
    {
        return {Matrix3x3::Identity(), Matrix3x3::Identity(), Vector3(0,0,0)};
    }

    /// Create translation
    static Transformation Translation(const Vector3& translation)
    {
        return {Matrix3x3::Identity(), Matrix3x3::Identity(), translation};
    }

    /// Create rotation transformation around x-axis with angle fPhi
    static Transformation RotationX(Float fPhi)
    {
        Matrix3x3 m = Matrix3x3::RotationX(fPhi);
        return {m, m.transpose(), Vector3(0,0,0)};
    }

    /// Create rotation transformation around y-axis with angle fPhi
    static Transformation RotationY(Float fPhi)
    {
        Matrix3x3 m = Matrix3x3::RotationY(fPhi);
        return {m, m.transpose(), Vector3(0,0,0)};
    }

    /// Create rotation transformation around z-axis with angle fPhi
    static Transformation RotationZ(Float fPhi)
    {
        Matrix3x3 m = Matrix3x3::RotationZ(fPhi);
        return {m, m.transpose(), Vector3(0,0,0)};
    }

    /// Create rotation matrix around vector n with angle fPhi
    static Transformation Rotation(const Vector3 &n, Float fPhi)
    {
        Matrix3x3 m = Matrix3x3::Rotation(n, fPhi);
        return {m, m.transpose(), Vector3(0,0,0)};
    }

    /// Create transformation from scaling; transformation matrix=diag(x,y,z)
    static Transformation Scale(Float x, Float y, Float z)
    {
        Matrix3x3 m = Matrix3x3::Scale(x,y,z);
        return {m, m.transpose(), Vector3(0,0,0)};
    }

    /// Create transformation (a rotation) from quaternion q
    static Transformation Rotation(const Quaternion& q)
    {
        Matrix3x3 m = q.normalize().toMatrix();
        return {m, m.transpose(), Vector3(0,0,0)};
    }

    /// Create transformation from translation, rotation, and scaling
    static Transformation TRS(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
    {
        // m = R*S
        Matrix3x3 S = Matrix3x3::Scale(scale[0], scale[1], scale[2]);
        Matrix3x3 R = rotation.toMatrix();
        Matrix3x3 m = R.matmult(S);

        // m^-1 = (R*S)^-1 = S^-1 * R^-1 = Sinv*Rinv
        Matrix3x3 Sinv = Matrix3x3::Scale(1/scale[0], 1/scale[1], 1/scale[2]); // inverse of S
        Matrix3x3 Rinv = R.transpose(); // inverse of R is the transpose of R
        Matrix3x3 minv = Sinv.matmult(Rinv);

        return {m, minv, translation};
    }

    /// Write affine transformation as a 3x4 matrix (3x3 linear transformation)
    /// plus a translation vector) to xfm in row-major form
    template<class T>
    void toXfm(T xfm[12]) const
    {
        xfm[0] = m_m.get(0,0);
        xfm[1] = m_m.get(0,1);
        xfm[2] = m_m.get(0,2);
        xfm[3] = m_d[0];

        xfm[4] = m_m.get(1,0);
        xfm[5] = m_m.get(1,1);
        xfm[6] = m_m.get(1,2);
        xfm[7] = m_d[1];

        xfm[8]  = m_m.get(2,0);
        xfm[9]  = m_m.get(2,1);
        xfm[10] = m_m.get(2,2);
        xfm[11] = m_d[2];
    }

    /// @brief Apply transformation to transformation T
    ///
    /// Apply the transformation to the transformation T and return a new
    /// transformation that represents the combined action of the two
    /// transformations.
    ///
    /// For example, T3 = T1.apply(T2) is the transformation of first applying
    /// T2 and then T1.
    ///
    /// @param T transformation
    /// @retval Tcombined new transformation
    Transformation apply(const Transformation &T) const
    {
        Matrix3x3 m = m_m.matmult(T.m_m);
        Matrix3x3 mInv = T.m_mInv.matmult(m_mInv);
        Vector3 d = m_d + m_m.apply(T.m_d);
        return {m,mInv,d};
    }

    /// Transform point from local to world coordinates
    Vector3 transformPointToWorld(const Vector3& point) const
    {
        return m_m.apply(point)+m_d;
    }

    /// Transform position from world to local coordinates
    Vector3 transformPointToLocal(const Vector3& point) const
    {
        return m_mInv.apply(point-m_d);
    }

    /// Transform vector from world to local coordinates
    Vector3 transformVectorToLocal(const Vector3& vector) const
    {
        return m_mInv.apply(vector);
    }

    /// Transform direction from local to world coordinates
    Vector3 transformVectorToWorld(const Vector3& vector) const
    {
        return m_m.apply(vector);
    }

    /// Transform normal from world to local coordinates
    Vector3 transformNormalToLocal(const Vector3& vNormal) const
    {
        return (m_m.applyTransposed(vNormal)).normalize();
    }

    /// Transform normal from local to world coordinates
    Vector3 transformNormalToWorld(const Vector3& vNormal) const
    {
        return (m_mInv.applyTransposed(vNormal)).normalize();
    }

    /// Transform ray from world to local coordinates
    Ray transformRayToLocal(const Ray& ray) const
    {
        Ray ray_local = ray;

        ray_local.setOrigin(transformPointToLocal(ray.getOrigin()));
        ray_local.setDirection(transformVectorToLocal(ray.getDirection()));

        return ray_local;
    }

    /// Transform ray from local to world coordinates
    Ray transformRayToWorld(const Ray& ray) const
    {
        Ray ray_world = ray;

        ray_world.setOrigin(transformPointToWorld(ray.getOrigin()));
        ray_world.setDirection(transformVectorToWorld(ray.getDirection()));
        return ray_world;
    }

    /// Overload "<<" operator
    friend std::ostream& operator<<(std::ostream& os, const Transformation& transformation)
    {
        os << transformation.m_d << std::endl;
        os << transformation.m_m;
        return os;
    }

    //Transformation to 4x4 row-major matrix
    template<class T>
    void to4x4RowMajor(T M[][4]) const
    {
        for (int i=0; i<3; ++i)
        {
            for (int j=0; j<3; ++j)
                M[i][j]=static_cast<T>(m_m.get(i,j));
            M[i][3]=static_cast<T>(m_d[i]);
        }
        M[3][0]=M[3][1]=M[3][2]=static_cast<T>(0.0);
        M[3][3]=static_cast<T>(1.0);
    }

    //Transformation to 4x4 column-major matrix
    template<class T>
    void to4x4ColumnMajor(T M[][4]) const
    {
        for (int i=0; i<3; ++i)
        {
            for (int j=0; j<3; ++j)
                M[i][j]=static_cast<T>(m_m.get(j,i));
            M[3][i]=static_cast<T>(m_d[i]);
        }
        M[0][3]=M[1][3]=M[2][3]=static_cast<T>(0.0);
        M[3][3]=static_cast<T>(1.0);
    }
};

#endif // TRANSFORMATION_H