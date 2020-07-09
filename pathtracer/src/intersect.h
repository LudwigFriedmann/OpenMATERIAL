//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//
 
/// @file      intersect.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-06-30
/// @brief     Support ray triangle intersection, provide information on intersection point

#ifndef INTERSECT_H
#define INTERSECT_H

#include <limits>
#include <vector>

class Mesh;

#include "assetmaterial.h"
#include "defines.h"
#include "mesh.h"
#include "ray.h"
#include "transformation.h"
#include "vector.h"

/// @brief Information on intersection
///
/// The class allows access on information of an intersection like the
/// intersection point, barycentric coordinates or the material.
class Intersection
{
private:
    /// Flag indicating if ray hits geometry
    bool m_bHit = false;

    /// Index of triangle
    std::size_t m_uTriangleIndex = 0;

    /// Number of node the triangle we intersected belongs to
    unsigned int m_uNodeNumber = 0;

    /// Origin of incident ray (in world coordinates)
    Vector3 m_vRayOrigin;

    /// Direction of incident ray (in world coordinates)
    Vector3 m_vRayDirection;

    /// Pointer to the mesh corresponding to the intersection point
    const Mesh *m_pMesh = nullptr;

    /// Transformation from local to world coordinates
    Transformation m_transformation = Transformation::Identity();

    /// Distance from origin of the ray to intersection point
    Float m_t = NAN;

    /// Barycentric coordinate u
    Float m_u = NAN;

    /// Barycentric coordinate v
    Float m_v = NAN;

public:

    /// @brief Create new intersection object
    ///
    /// @param [in] crIncidentRay incident ray in world coordinates
    Intersection(const Ray& crIncidentRay)
    {
        m_vRayOrigin = crIncidentRay.getOrigin(); // world coordinates
        m_vRayDirection = crIncidentRay.getDirection(); // world coordinates
    }

    /// @brief Set mesh
    ///
    /// Set pointer to the mesh, as well as the triangle index and the
    /// barycentric coordinates t,u,v.
    ///
    /// @param pMesh pointer to mesh
    /// @param uTriangleIndex index of triangle
    /// @param t separation from origin of ray to intersection point
    /// @param u barycentric coordinate u
    /// @param v varycentric coordinate v
    void setMesh(const Mesh *pMesh, std::size_t uTriangleIndex, Float t, Float u, Float v)
    {
        m_bHit = true;

        m_uTriangleIndex = uTriangleIndex;
        m_t = t;
        m_u = u;
        m_v = v;
        m_pMesh = pMesh;
    }

    /// Return pointer to mesh
    const Mesh *getMesh() const
    {
        return m_pMesh;
    }

    /// @brief Get curvature tensor at intersection point
    ///
    /// Important: The curvature tensor is in *local* coordinates.
    ///
    /// @param [out] xu normalized tangent vector, xu = d/du x(u,v) / |d/du x(u,v)|
    /// @param [out] xv normalized tangent vector, xv = d/dv x(u,v) / |d/dv x(u,v)|
    /// @retval Q curvature tensor
    Matrix2x2 getCurvatureTensor(Vector3& xu, Vector3& xv) const
    {
        return m_pMesh->getCurvatureTensor(m_uTriangleIndex, m_u, m_v, xu, xv, m_transformation);
    }

    /// @brief Get normal
    ///
    /// Return smooth normal ("Phong shading") if vertex normals are available
    /// or otherwise flat normal ("flat shading").
    ///
    /// @retval v surface normal of intersection point
    const Vector3 getNormal() const
    {
        const Vector3 vNormal = m_pMesh->getNormal(m_uTriangleIndex, m_u, m_v);
        return m_transformation.transformNormalToWorld(vNormal);
    }

    /// @brief Get flat normal
    ///
    /// The method always returns the flat normal even if vertex normals are
    /// present.
    ///
    /// @retval v surface normal of intersection point (flat normal)
    Vector3 getFlatNormal() const
    {
        const Vector3 vNormal = m_pMesh->getFlatNormal(m_uTriangleIndex);
        return m_transformation.transformNormalToWorld(vNormal);
    }

    /// Set transformation to crTransformation
    void setTransformation(const Transformation& crTransformation)
    {
        m_transformation = crTransformation;
    }

    /// Return transformation
    const Transformation& getTransformation() const
    {
        return m_transformation;
    }

    /// Return pointer to material
    const AssetMaterial *getMaterial() const
    {
        return m_pMesh->getMaterial();
    }

    /// Set node number
    void setNodeNumber(unsigned int uNode)
    {
        m_uNodeNumber = uNode;
    }

    /// Get intersection point
    const Vector3 getIntersectionPoint() const
    {
        return m_vRayOrigin + m_t*m_vRayDirection;
    }

    /// Get barycentric coordinates t,u,v
    void getBaryCentric(Float& t, Float& u, Float& v) const
    {
        t = m_t;
        u = m_u;
        v = m_v;
    }

    /// Get separation from origin of ray to intersection point
    Float getSeparation() const
    {
        return m_t;
    }

    /// Get number of node
    unsigned int getNodeNumber() const { return m_uNodeNumber; }

    /// Get direction of incident ray
    const Vector3 getDirection() const
    {
        return m_vRayDirection;
    }

    /// Return true if ray hits geometry, otherwise false
    bool hit() const { return m_bHit; }

};


/// @brief Compute intersection of a ray with a triangle
///
/// This method computes the intersection of a ray with a triangle. The triangle
/// is represented by its three vertices V0, V1, V2, and the ray is parametrized
/// as
///     R(t) = O + t*D .
///
/// If bCulling is true, back-facing triangles are discared.
///
/// If the ray hits the triangle, the separation t between origin and the
/// intersection point is tmin <= t <= tmax, the function returns 1 or -1.
/// Otherwise, 0 is returned.
///
/// If the ray hits the triangle, the separation t between the origin of the ray
/// and the intersection point, and the barycentric coordinates u,v are written
/// to rf_t, rf_u, rf_v, respectively.
///
/// The code implements the ideas of this paper:
/// T. Möller, B. Trumbore, Fast, Minimum Storage Ray-Triangle Intersection,
/// J. Graph. Tools 2, 21-28 (1997)
///
/// @param [in] V0 position of the 1st vertex of the triangle
/// @param [in] V1 position of the 2nd vertex of the triangle
/// @param [in] V2 position of the 3rd vertex of the triangle
/// @param [in] O origin of ray
/// @param [in] D direction of ray
/// @param [out] rf_t separation from origin of ray to hit point
/// @param [out] rf_u barycentric coordinate u
/// @param [out] rf_v barycentric coordinate v
/// @param [in] tmin minimum value of t (see description)
/// @param [in] tmax maximum value of t (see description)
/// @param [in] bCulling discard back-facing triangles if set to true
/// @retval 1 if ray hits the triangle, and tmin <= t <= tmax, and positive orientation
/// @retval -1 if ray hits the triangle, and tmin <= t <= tmax, and positive orientation
/// @retval 0 if ray does not hit the triangle
inline int intersectRayTriangle(
    const Vector3& V0, const Vector3& V1, const Vector3& V2,
    const Vector3& O, const Vector3& D,
    Float& rf_t, Float& rf_u, Float& rf_v,
    Float tmin=0, Float tmax=fInfinity, bool bCulling=true)
{
    // see inline equations above (5)
    const Vector3 E1 = V1-V0;
    const Vector3 E2 = V2-V0;
    const Vector3 T = O-V0;

    // P=D x E2; see inline equation after (6)
    const Vector3 P = D.cross(E2);

    // compute determinant: det=P.dot(E1); see denominator of (6)
    const Float det = P.dot(E1);
    if(det == 0)
        return 0;

    if(bCulling && det < 0)
        // ray does not intersect the plane triangle lies in
        return 0;

    const Float invdet = 1/det;

    /* u and v are barycentric coordinates. A point P inside the triangle
     * can be represented as
     *     P(u,v) = w*V0 + u*V1 + v*V2
     * where w=1-u-v and 0<=u,v<=1.
     */
    const Float u = invdet*P.dot(T); // (6)
    if(u < 0 || u > 1) // u and v are barycentric coordinate
        return 0;

    // Q = T x E1; see inline equation after (6)
    const Vector3 Q = T.cross(E1);

    const Float v = invdet*Q.dot(D); // (6)
    if(v < 0 || (u+v) > 1)
        return 0;

    // t is the distance from the origin O
    const Float t = invdet*Q.dot(E2); // (6)

    if(t < tmin || t > tmax)
        return 0;

    rf_t = t;
    rf_u = u;
    rf_v = v;

    return det > 0 ? 1 : -1;
}

/// @brief Compute intersection of a ray with a plane
///
/// This method computes the intersection of a ray with a plane. The plane is
/// represented by a point P on the plane and the normal to the plane N. The ray
/// is represented as
///     R(t) = O + t*D .
///
/// A point R is part of the plane iff (R-P)*N=0. Thus, for the intersection:
///     (O+t*D-P)*N = (O-P)*N - t*D*N = 0  =>  t = (O-P)*N/N*D
///
/// If the ray hits the plane, the separation t between origin and the
/// intersection point is tmin <= t <= tmax, the function returns 1 or -1.
/// Otherwise, 0 is returned. The separation t is written to rf_t.
///
/// @param [in] P point on the plane
/// @param [in] N normal of the plane
/// @param [in] O origin of ray
/// @param [in] D direction of ray
/// @param [out] rf_t separation from origin of ray to hit point
/// @param [in] tmin minimum value of t (see description)
/// @param [in] tmax maximum value of t (see description)
/// @retval 1 if ray hits  plane, and tmin <= t <= tmax, and positive orientation
/// @retval -1 if ray hits the plane, and tmin <= t <= tmax, and positive orientation
/// @retval 0 if ray does not hit the plane
inline int intersectRayPlane(
    const Vector3& P, const Vector3& N,
    const Vector3& O, const Vector3& D,
    Float& rf_t,
    Float tmin=0, Float tmax=fInfinity)
{
    const Float NdotD = N.dot(D);
    if(NdotD == 0)
        return 0;

    const Float t = (P-O).dot(N)/NdotD;
    if(t < tmin || t > tmax)
        return 0;

    rf_t = t;
    if(t*NdotD < 0)
        return 1;
    else
        return -1;
}

#endif // INTERSECT_H
