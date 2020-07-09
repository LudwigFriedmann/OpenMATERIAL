//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      mesh.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Support for meshes

#ifndef MESH_H
#define MESH_H

#include <vector>

#include "assetmaterial.h"
#include "bbox.h"
#include "defines.h"
#include "matrix.h"
#include "ray.h"
#include "transformation.h"

/// @brief This class stores supports mesh storage as well as BVH and intersection calculation

class Intersection;

/// Mesh class
class Mesh
{
private:
    /// Name of the mesh (string may be empty)
    std::string m_sName;

    /// Axis aligned bounding boxes
    std::vector<BBox> m_vBbox;

    /// @brief Material associated to the mesh
    ///
    /// TODO: mapping needs to be done in scene graph
    const AssetMaterial *m_pMaterial = nullptr;

    /// Buffer containing the vertex indices
    std::vector<unsigned int> m_IndexBuffer;

    /// Buffer containing the vertices
    std::vector<Float> m_VertexBuffer;

    /// Buffer containing the normals (if empty use flat shading)
    std::vector<Float> m_NormalBuffer;

    std::size_t binaryTreeBoundaries(std::size_t uLeaf, std::size_t& uLeft) const;


public:
    /// Create a new mesh with name given by crsName
    Mesh(const std::string& crsName="") : m_sName(crsName) {}

    void buildBVH();

    /// Get name of mesh
    std::string getName() const { return m_sName; }

    /// Get axis-aligned bounding box of mesh
    const BBox& getBbox() const { return m_vBbox.at(0); }

    /// Get reference to the index buffer
    std::vector<unsigned int>& getIndexBuffer()
    {
        return m_IndexBuffer;
    }

    /// Get reference to the vertex buffer
    std::vector<Float>& getVertexBuffer()
    {
        return m_VertexBuffer;
    }

    /// Get reference to the normal buffer
    std::vector<Float>& getNormalBuffer()
    {
        return m_NormalBuffer;
    }

    /// Set material of the mesh to pMaterial
    void setMaterial(const AssetMaterial *pMaterial) { m_pMaterial = pMaterial; }

    /// Get a pointer to the material of the mesh
    const AssetMaterial *getMaterial() const { return m_pMaterial; }

    /// Return the number of triangles stored in the mesh
    std::size_t getNumberOfTriangles() const { return m_IndexBuffer.size()/3; }

    Matrix2x2 getCurvatureTensor(std::size_t uTriangleIndex, Float u, Float v, Vector3 &xu, Vector3 &xv, Transformation trafo = Transformation::Identity()) const;

    void getVerticesOfTriangle(std::size_t uTriangleIndex, Vector3& V0, Vector3& V1, Vector3& V2) const;

    bool getNormalsOfTriangle(std::size_t uTriangleIndex, Vector3& N0, Vector3& N1, Vector3& N2) const;

    Vector3 getNormal(std::size_t uTriangleIndex, Float u=0.33333333, Float v=0.33333333) const;

    Vector3 getFlatNormal(std::size_t uTriangleIndex) const;

    bool intersectRay(const Ray& ray, Intersection& rIntersection, Float tmin, Float& tmax, std::size_t uLeaf=0) const;
};

#endif // MESH_H
