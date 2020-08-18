//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      matrix.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-15, 2019-09-13
/// @brief     Support for NxN matrix operations

#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <cmath>
#include <type_traits>

#include "defines.h"
#include "vector.h"

/// DIM x DIM matrix of type T
template<class T, int DIM>
class Matrix
{
private:
    /// Matrix elements; the elements are stored in row-major order
    T m_M[DIM*DIM] = { 0 };

public:
    /// Convert matrix given by src from row-major order to column-major order,
    /// or from column-major order to row-major order.
    template<class X>
    static void convertMajorOrder(const X *src, X *dest, int dim)
    {
        for (int m = 0; m < dim; m++)
            for (int n = 0; n < dim; n++)
                dest[n*dim+m] = src[m*dim+n];
    }

    // Constructors

    /// Create zero matrix
    Matrix() = default;

    /// Copy matrix
    Matrix(const Matrix& M)
    {
        for(int i = 0; i < DIM*DIM; i++)
            m_M[i] = M.m_M[i];
    }

    /// Create matrix where all matrix elements are given by fValue
    explicit Matrix(T fValue) { setAll(fValue); }

    ///  @brief Create matrix (a,b,c) from column vectors a,b,c
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    Matrix(const Vector3& a, const Vector3& b, const Vector3& c)
    {
        for(int i = 0; i < 3; i++)
        {
            m_M[i*3]   = a[i];
            m_M[i*3+1] = b[i];
            m_M[i*3+2] = c[i];
        }
    }

    /// Create identity matrix
    static Matrix Identity()
    {
        Matrix M = Matrix(0);
        for(int i = 0; i < DIM; i++)
            M.set(i,i,1);

        return M;
    }

    ///  @brief Check if matrix is zero matrix
    bool isZero() const
    {
        for(int i = 0; i < DIM*DIM; i++)
            if(m_M[i] != 0)
                return false;

        return true;
    }

    ///  @brief Create rotation matrix around x-axis with angle phi
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    static Matrix RotationX(Float phi)
    {
        Matrix M = Identity();
        const Float c = std::cos(phi), s = std::sin(phi);

        M.set(1,1, c);
        M.set(1,2,-s);
        M.set(2,1, s);
        M.set(2,2, c);

        return M;
    }

    ///  @brief Create rotation matrix around y-axis with angle phi
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    static Matrix RotationY(Float phi)
    {
        Matrix M = Identity();
        const Float c = std::cos(phi), s = std::sin(phi);

        M.set(0,0, c);
        M.set(0,2, s);
        M.set(2,0,-s);
        M.set(2,2, c);

        return M;
    }

    ///  @brief Create rotation matrix around z-axis with angle phi
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    static Matrix RotationZ(Float phi)
    {
        Matrix M = Identity();
        const Float c = std::cos(phi), s = std::sin(phi);

        M.set(0,0, c);
        M.set(0,1,-s);
        M.set(1,0, s);
        M.set(1,1, c);

        return M;
    }

    ///  @brief Create rotation matrix around vector n with angle phi
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    static Matrix Rotation(const Vector3 &n, Float phi)
    {
        Float n1,n2,n3;
        const Vector3 v = n.normalize();
        n1 = v[0];
        n2 = v[1];
        n3 = v[2];
        Float fCosPhi = std::cos(phi), fSinPhi = std::sin(phi);

        Matrix M;
        M.set(0,0, n1*n1*(1-fCosPhi)+fCosPhi);
        M.set(0,1, n1*n2*(1-fCosPhi)-n3*fSinPhi);
        M.set(0,2, n1*n3*(1-fCosPhi)+n2*fSinPhi);

        M.set(1,0, n2*n1*(1-fCosPhi)+n3*fSinPhi);
        M.set(1,1, n2*n2*(1-fCosPhi)+fCosPhi);
        M.set(1,2, n2*n3*(1-fCosPhi)-n1*fSinPhi);

        M.set(2,0, n3*n1*(1-fCosPhi)-n2*fSinPhi);
        M.set(2,1, n3*n2*(1-fCosPhi)+n1*fSinPhi);
        M.set(2,2, n3*n3*(1-fCosPhi)+fCosPhi);

        return M;
    }

    /// Create diagonal matrix with diagonal elements x,y,z
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    static Matrix Scale(Float x, Float y, Float z)
    {
        Matrix M = Matrix(0);
        M.set(0,0,x);
        M.set(1,1,y);
        M.set(2,2,z);

        return M;
    }

    /// @brief Check if matrix contains not-a-numbers
    ///
    /// @retval true if at least one matrix element is a NAN
    /// @retval false if no matrix element is a NAN
    bool containsNAN() const
    {
        for(int i = 0; i < DIM*DIM; i++)
            if(std::isnan(m_M[i]))
                return true;
        return false;
    }

    /// Set all matrix elements to value
    void setAll(T value)
    {
        for(int i = 0; i < DIM*DIM; i++)
            m_M[i] = value;
    }

    /// Get matrix elements M[m,n]
    T get(int m, int n) const
    {
        return m_M[m*DIM+n];
    }

    /// Set matrix element to M[m,n]=value
    void set(int m, int n, T value)
    {
        m_M[m*DIM+n] = value;
    }

    /// Get pointer to data
    const T *getPointer() const
    {
        return m_M;
    }

    /// Return transposed matrix
    Matrix transpose() const
    {
        Matrix M;

        for(int m = 0; m < DIM; m++)
            for(int n = 0; n < DIM; n++)
                M.m_M[n*DIM+m] = m_M[m*DIM+n];

        return M;
    }

    /*** Overload unary operators + and - ***/
    /// Overload unary + operator to support operation +A
    Matrix operator+() const
    {
        Matrix M = Matrix();
        for(int i = 0; i < DIM*DIM; i++)
            M.m_M[i] = m_M[i];
        return M;
    }

    /// Overload unary - operator to support operation -A
    Matrix operator-() const
    {
        Matrix M = Matrix();
        for(int i = 0; i < DIM*DIM; i++)
            M.m_M[i] = -m_M[i];
        return M;
    }

    /*** Overload binary operators +, - ***/
    /// Overload + operator to support operation A+B
    Matrix operator+(const Matrix& B) const
    {
        Matrix M = Matrix();
        for(int i = 0; i < DIM*DIM; i++)
            M.m_M[i] = m_M[i]+B.m_M[i];
        return M;
    }

    /// Overload + operator to support operation A-B
    Matrix operator-(const Matrix& B) const
    {
        Matrix M = Matrix();
        for(int i = 0; i < DIM*DIM; i++)
            M.m_M[i] = m_M[i]-B.m_M[i];
        return M;
    }

    /// Overload * operator to support operation A*fValue
    Matrix operator*(T value) const
    {
        Matrix M = Matrix();
        for(int i = 0; i < DIM*DIM; i++)
            M.m_M[i] = value*m_M[i];
        return M;
    }

    /// Overload / operator to support operation A/fValue
    Matrix operator/(T value) const
    {
        Matrix M = Matrix();
        for(int i = 0; i < DIM*DIM; i++)
            M.m_M[i] = m_M[i]/value;
        return M;
    }

    /*** Overload inplace operators ***/
    /// Overload += operator to support operation A+=B
    void operator+=(const Matrix& B)
    {
        for(int i = 0; i < DIM*DIM; i++)
            m_M[i] += B.m_M[i];
    }

    /// Overload -= operator to support operation A-=B
    void operator-=(const Matrix &B)
    {
        for(int i = 0; i < DIM*DIM; i++)
            m_M[i] -= B.m_M[i];
    }

    /// Overload * operator to support operation A*value
    void operator*=(T value)
    {
        for(int i = 0; i < DIM*DIM; i++)
            m_M[i] *= value;
    }

    /// Overload = operator to support operation A=B
    void operator=(const Matrix &B)
    {
        for(int i = 0; i < DIM*DIM; i++)
            m_M[i] = B.m_M[i];
    }

    ///  @brief Apply the matrix A to the vector v and return Av
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    Vector3 apply(const Vector3 &v) const
    {
        Vector3 w(0,0,0);

        for(int m = 0; m < 3; m++)
            for(int n = 0; n < 3; n++)
                w[m] += m_M[m*3+n]*v[n];

        return w;
    }

    ///  @brief Apply the matrix A^T to the vector v and return (A^T)v
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    Vector3 applyTransposed(const Vector3& v) const
    {
        Vector3 w(0,0,0);

        for(int m = 0; m < 3; m++)
            for(int n = 0; n < 3; n++)
                w[m] += m_M[n*3+m]*v[n];

        return w;
    }

    /// Return the trace of the matrix
    T trace() const
    {
        T sum = 0;
        for(int i = 0; i < DIM; i++)
            sum += get(i,i);

        return sum;
    }

    ///  @brief Compute determinant of 3x3 matrix A
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    T det() const
    {
        const T *u = m_M;

        T A =   u[4]*u[8] - u[5]*u[7];
        T B = -(u[3]*u[8] - u[5]*u[6]);
        T C =   u[3]*u[7] - u[4]*u[6];

        return u[0]*A + u[1]*B + u[2]*C;
    }

    ///  @brief Compute determinant of 2x2 matrix A
    template <int D = DIM, typename std::enable_if<D == 2, void>::type* = nullptr>
    T det() const
    {
        return m_M[0]*m_M[3] - m_M[1]*m_M[2];
    }

    /// Compute and return inverse of the 2x2 matrix A, A^-1
    template <int D = DIM, typename std::enable_if<D == 2, void>::type* = nullptr>
    Matrix invert() const
    {
        T a = get(0,0);
        T b = get(0,1);
        T c = get(1,0);
        T d = get(1,1);
        T invDet = 1/(a*d-b*c);

        Matrix Minv;
        Minv.set(0,0, +d*invDet);
        Minv.set(0,1, -b*invDet);
        Minv.set(1,0, -c*invDet);
        Minv.set(1,1, +a*invDet);

        return Minv;
    }

    ///  @brief Compute and return inverse of Matrix A, A^-1
    template <int D = DIM, typename std::enable_if<D == 3, void>::type* = nullptr>
    Matrix invert() const
    {
        Matrix N = Matrix();
        const T *m = m_M; // matrix
        T *n = N.m_M; // inverse of matrix

        n[0] =   m[4]*m[8] - m[5]*m[7];
        n[1] = -(m[1]*m[8] - m[2]*m[7]);
        n[2] =   m[1]*m[5] - m[2]*m[4];

        n[3] = -(m[3]*m[8] - m[5]*m[6]);
        n[4] =   m[0]*m[8] - m[2]*m[6];
        n[5] = -(m[0]*m[5] - m[2]*m[3]);

        n[6] =   m[3]*m[7] - m[4]*m[6];
        n[7] = -(m[0]*m[7] - m[1]*m[6]);
        n[8] =   m[0]*m[4] - m[1]*m[3];

        T determinant = m[0]*n[0]+m[1]*n[3]+m[2]*n[6];

        for(int i = 0; i < 9; i++)
            n[i] /= determinant;

        return N;
    }

    ///  @brief Compute matrix multiplication A*B
    Matrix matmult(const Matrix &B) const
    {
        Matrix C = Matrix(0);
        const T *a = m_M, *b = B.m_M;
        T *c = C.m_M;

        for(int m = 0; m < DIM; m++)
            for(int n = 0; n < DIM; n++)
                for(int j = 0; j < DIM; j++)
                    c[m*DIM+n] += a[m*DIM+j]*b[j*DIM+n];

        return C;
    }

    /// Overload * operator to support operation value*A
    friend Matrix operator*(T value, const Matrix &B)
    {
        Matrix M = Matrix();
        for(int i = 0; i < DIM*DIM; i++)
            M.m_M[i] = value*B.m_M[i];
        return M;
    }

    /// Overload << operator
    friend std::ostream& operator<<(std::ostream &os, const Matrix &M)
    {
        for(int m = 0; m < DIM; m++)
        {
            for(int n = 0; n < DIM; n++)
            {
                os << M.get(m,n);

                if(n != DIM-1)
                    os << "  ";
            }

            if(m != DIM-1)
                os << std::endl;
        }

        return os;
    }
};

/// 3x3 matrix of Floats
typedef Matrix<Float,3> Matrix3x3;

/// 2x2 matrix of Floats
typedef Matrix<Float,2> Matrix2x2;

#endif // MATRIX_H