//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      node.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Support for nodes

#ifndef NODE_H
#define NODE_H

#include <vector>

#include "bbox.h"
#include "defines.h"
#include "mesh.h"
#include "transformation.h"
#include "intersect.h"

/// @brief This class supports the node storage. Transformations are also saved.
class Node;

/// Node class
class Node
{
private:
    /// Name of node
    std::string m_sName;

    /// Number of node / node index
    std::size_t m_uNodeNumber;

    /// Transformation of the node
    Transformation m_transformation = Transformation::Identity();

    /// Pointer to the corresponding mesh (may be nullptr)
    const Mesh *m_pMesh = nullptr;

    /// Pointer to parent node (may be nullptr if node has no parent)
    const Node *m_pParent = nullptr;

    /// List of children (list might be empty if node has no children)
    std::vector<const Node *> m_pChildren;

    /// @brief Compute the minimum axis-aligned bounding box of this node
    ///
    /// This method is intended for internal use only. For more information
    /// refer to the public getBBox method.
    ///
    /// @param [in,out] vMin minimum edge of bounding box
    /// @param [in,out] vMax maximum edge of bounding box
    /// @param [in] transformation transformation from local to world coordinates
    void getBBox(Vector3& vMin, Vector3& vMax, Transformation transformation) const
    {
        transformation = m_transformation.apply(transformation);

        // If this node contains a mesh, compute the bbox of the mesh
        if(m_pMesh != nullptr)
        {
            const std::size_t uNumberOfTriangles = m_pMesh->getNumberOfTriangles();
            for(std::size_t uTriangleNumber = 0; uTriangleNumber < uNumberOfTriangles; uTriangleNumber++)
            {
                // Vector containing the three triangle vertices
                Vector3 V[3];
                m_pMesh->getVerticesOfTriangle(uTriangleNumber, V[0], V[1], V[2]);

                // For each vertex of the triangle...
                for(int i = 0; i < 3; i++)
                {
                    // ...transform to world coordinates...
                    Vector3 v = transformation.transformPointToWorld(V[i]);

                    // ...and update bounding box
                    for(int j = 0; j < 3; j++)
                    {
                        vMin[j] = std::min(v[j], vMin[j]);
                        vMax[j] = std::max(v[j], vMax[j]);
                    }
                }
            }
        }

        // Consider children
        for(const Node *pNodeChild : m_pChildren)
            pNodeChild->getBBox(vMin, vMax, transformation);
    }

public:
    /// Create new node with node number uNodeNumber and name given by crsName
    Node(std::size_t uNodeNumber, const std::string& crsName="")
    {
        m_uNodeNumber = uNodeNumber;
        m_sName = crsName;
    }

    /// Return name of node (might be empty string)
    std::string getName() const { return m_sName; }


    /// @brief Compute minimum axis-aligned bounding box
    ///
    /// This method computes the minimum axis-aligned bounding box for this
    /// node. Due to the hierarchical structure of nodes and as nodes can have
    /// transformations, this method needs to iterate over each vertex belonging
    /// to this node and transfer the vertex point to world coordinate. For this
    /// reason, this method is rather expensive to call.
    ///
    /// @retval bbox minimum axis-aligned bounding box
    BBox getBBox() const
    {
        Vector3 vMin = Vector3(+fInfinity, +fInfinity, +fInfinity);
        Vector3 vMax = Vector3(-fInfinity, -fInfinity, -fInfinity);
        getBBox(vMin, vMax, Transformation::Identity());
        return BBox(vMin, vMax);
    }

    /// Set name of node to crsName
    void setName(const std::string& crsName) { m_sName = crsName; }

    /// Return node number / node index
    std::size_t getNodeNumber() const { return m_uNodeNumber; }

    /// @brief Determine if node has a specific descent
    ///
    /// Return true if the parent or any other descent of the node has the
    /// node number giben by uNodeNumber. Otherwise, return false.
    bool hasParent(std::size_t uNodeNumber) const
    {
        const Node *pNode = this;

        while((pNode = pNode->getParent()) != nullptr)
            if(pNode->getNodeNumber() == uNodeNumber)
                return true;

        return false;
    }

    /// Set parent of node to pParent
    void setParent(Node *pParent)
    {
        m_pParent = pParent;
    }

    /// Return pointer to parent node
    const Node *getParent() const { return m_pParent; }

    /// Add pChild as a child
    void addChild(const Node *pChild)
    {
        m_pChildren.push_back(pChild);
    }

    /// Set mesh of node to pMesh
    void setMesh(const Mesh *pMesh)
    {
        m_pMesh = pMesh;
    }

    /// Set transformation of node
    void setTransformation(const Transformation& crTransformation)
    {
        m_transformation = crTransformation;
    }

    /// Return transformation of node
    const Transformation& getTransformation() const
    {
        return m_transformation;
    }

    /// @brief Intersect ray with mesh group
    ///
    /// This method checks if the ray intersects one of the triangles of the mesh
    /// group. If the ray intersects a triangle and the distance t from the origin
    /// is between tmin and tmax, tmin <= t <= tmax, then rIntersection is set
    /// and true is returned. Otherwise, the method returns false.
    ///
    /// This method always finds the closest intersection point with respect to the
    /// origin of the ray. On exit, the distance from the origin of the ray to the
    /// hit point is stored in the variable tmax.
    ///
    /// @param [in] ray incident ray
    /// @param [out] rIntersection information on the intersection point
    /// @param [in] tmin minimum value of t
    /// @param [in,out] tmax maximum value of t; on exit distance from origin to hit point
    /// @retval true if ray hits a triangle
    /// @retval false if ray does not hit a triangle
    bool intersectRay(const Ray& ray, Intersection& rIntersection, Float tmin, Float& tmax) const
    {
        int hit = false;

        // Transform ray to local coordinates
        const Ray rayLocal = m_transformation.transformRayToLocal(ray);

        // Check if we have a hit with one of the triangles in the mesh
        if(m_pMesh != nullptr)
            if(m_pMesh->intersectRay(rayLocal, rIntersection, tmin, tmax))
            {
                rIntersection.setNodeNumber(m_uNodeNumber);
                hit = true;
            }

        // Check the childen
        for(const Node *pChildNode : m_pChildren)
            hit += pChildNode->intersectRay(rayLocal, rIntersection, tmin, tmax);

        return hit;
    }
};

#endif // NODE_H
