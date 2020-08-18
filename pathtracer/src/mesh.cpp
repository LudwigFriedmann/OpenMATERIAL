//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      mesh.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Support for meshes

#include "defines.h"
#include "doctest.h"
#include "mesh.h"
#include "intersect.h"
#include "sort_r.h"
#include "tiny_gltf.h"

/// @brief Unit test for Mesh::Mesh 
TEST_CASE("Testing Mesh::Mesh")
{
	std::string fileName = "../objects/cube_gold.gltf";
	#ifdef _WIN32
		std::replace(fileName.begin(), fileName.end(), '/', '\\');
	#endif

	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err, warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);
	if (!ret)
		throw GltfError("Could not load glTF file " + fileName + ": " + err);

	// Check if correct mesh is loaded
	std::string name = "Cube";
	bool matchingName = false;
	for (const tinygltf::Mesh& gltfMesh : model.meshes)
	{
		Mesh *pMesh = new Mesh(gltfMesh.name);
		std::string meshName = pMesh->getName();
		if (meshName == name)
			matchingName = true;
	}
	CHECK(matchingName);
}


/// @brief Struct which links the vertex coordinates with its index number.
/// It is used in the boundary volume hierarchy for fast ray intersection
typedef struct {
    ///Pointer to the location of buffer which collects all vertexes position (Meshes)
    Float *pVertexBuffer;
    ///Offset to navigate among the X,Y,Z vertex coordinates
    int index;
} sort_param_t;

static int compareFunction(const void *a_, const void *b_, void *args)
{
    auto *params = static_cast<sort_param_t *>(args);
    const Float *pVertexBuffer = params->pVertexBuffer;
    const int index = params->index;

    const auto *a = static_cast<const unsigned int*>(a_);
    const auto *b = static_cast<const unsigned int*>(b_);

    Float x = (pVertexBuffer[3*a[0]+index]+pVertexBuffer[3*a[1]+index]+pVertexBuffer[3*a[1]+index])/3;
    Float y = (pVertexBuffer[3*b[0]+index]+pVertexBuffer[3*b[1]+index]+pVertexBuffer[3*b[1]+index])/3;

    return x<y;
}

/// @brief Get the left boundary and the number of elements for a leaf
///
/// @param [in] uLeaf leaf number
/// @param [out] uLeft left triangle index
/// @retval uElems number of elements
std::size_t Mesh::binaryTreeBoundaries(std::size_t uLeaf, std::size_t& uLeft) const
{
    std::size_t k = uLeaf+1;
    std::size_t level = std::log2(1+uLeaf);

    uLeft = 0;
    std::size_t uRight = m_IndexBuffer.size()/3-1;
    for(std::size_t i = 0; i < level; i++)
    {
        std::size_t middle = (uLeft+uRight)/2;
        if(k & (1 << (level-i-1)))
            uLeft = middle+1;
        else
            uRight = middle;
    }

    return uRight+1-uLeft;
}

/// @brief Build BVH for this mesh
///
/// This method has to be called before any call to \ref getBbox or
/// \ref intersectRay.
///
/// Build a boundary volume hierarchy for fast ray intersection. The BVH is a
/// balanced binary tree.
void Mesh::buildBVH()
{
    unsigned int *pIndexBuffer = m_IndexBuffer.data();
    Float *pVertexBuffer = m_VertexBuffer.data();

    std::size_t uNumberOfTriangles = m_IndexBuffer.size()/3;
    std::size_t uLevels = std::max(std::log2(uNumberOfTriangles)-2,0.);
    std::size_t uNumberOfLeafs = (1<<uLevels)-1;

    m_vBbox.reserve(uNumberOfLeafs);

    for(std::size_t uLeaf = 0; uLeaf < uNumberOfLeafs; uLeaf++)
    {
        std::size_t uLeft;
        const std::size_t uElems = binaryTreeBoundaries(uLeaf, uLeft);

        Float xmin = fInfinity, ymin = fInfinity, zmin = fInfinity;
        Float xmax = -fInfinity, ymax = -fInfinity, zmax = -fInfinity;
        for(std::size_t i = 0; i < 3*uElems; i++)
        {
            Float x = pVertexBuffer[3*pIndexBuffer[3*uLeft+i]+0];
            xmin = std::min(x,xmin);
            xmax = std::max(x,xmax);

            Float y = pVertexBuffer[3*pIndexBuffer[3*uLeft+i]+1];
            ymin = std::min(y,ymin);
            ymax = std::max(y,ymax);

            Float z = pVertexBuffer[3*pIndexBuffer[3*uLeft+i]+2];
            zmin = std::min(z,zmin);
            zmax = std::max(z,zmax);
        }

        const Vector3 vMin = Vector3(xmin,ymin,zmin);
        const Vector3 vMax = Vector3(xmax,ymax,zmax);

        m_vBbox.emplace_back(vMin,vMax);

        sort_param_t args;
        args.pVertexBuffer = m_VertexBuffer.data();

        Vector3 vDiff = vMax-vMin;
        if(vDiff[0] > vDiff[1] && vDiff[0] > vDiff[2])
            args.index = 0;
        else if(vDiff[1] > vDiff[2])
            args.index = 1;
        else
            args.index = 2;

		sort_r(&pIndexBuffer[3 * uLeft], uElems, 3 * sizeof(unsigned int), compareFunction, &args);
    }
}

/// @brief Return normal of triangle
///
/// Return the normal of the triangle corresponding to the index
/// triangleIndex at barycentric coordinates u,v. If vertex normals are present,
/// smooth shading will be used (Phong shading) and the vertex normals are used
/// to interpolate the normal at the barycentric coordinates u,v.
///
/// If vertex normals are not present, flat normals are computed and
/// \ref getFlatNormal will be called.
///
/// @param [in] uTriangleIndex index of triangle
/// @param [in] u barycentric coordinate
/// @param [in] v barycentric coordinate
/// @retval N normal of triangle
Vector3 Mesh::getNormal(std::size_t uTriangleIndex, Float u, Float v) const
{
    Vector3 N0, N1, N2;

    if(getNormalsOfTriangle(uTriangleIndex, N0, N1, N2))
    {
        // Smooth shading / Phong shading

        assert(u >=0 && u <= 1);
        assert(v >=0 && v <= 1);
        assert((u+v) <= 1);

        return ((1-u-v)*N0 + u*N1 + v*N2).normalize();
    }
    else
        // Flat shading
        return getFlatNormal(uTriangleIndex);
}

/// @brief Return flat normal of triangle
///
/// Return the flat normal of the triangle corresponding to the index
/// triangleIndex. In contrast to \ref getNormal this method also returns the
/// flat normal (without interpolation / without Phong shading) if normal
/// vertices are present.
///
/// @param [in] uTriangleIndex index of triangle
/// @retval N flat normal of triangle
Vector3 Mesh::getFlatNormal(std::size_t uTriangleIndex) const
{
    // Flat normal
    Vector3 V0,V1,V2;

    getVerticesOfTriangle(uTriangleIndex, V0, V1, V2);
    const Vector3 E0 = V1-V0;
    const Vector3 E1 = V2-V0;
    return E0.cross(E1).normalize();
}

/// @brief Get vertices of triangle
///
/// Write the three vertices of the triangle corresponding to the index
/// uTriangleIndex to the vectors V1, V2, V3.
///
/// @param [in] uTriangleIndex index of triangle
/// @param [out] V0 first vertex
/// @param [out] V1 second vertex
/// @param [out] V2 third vertex
void Mesh::getVerticesOfTriangle(std::size_t uTriangleIndex, Vector3& V0, Vector3& V1, Vector3& V2) const
{
    const Float *VertexBuffer = m_VertexBuffer.data();
    const unsigned int *indexBuffer = m_IndexBuffer.data();
    V0 = Vector3(&VertexBuffer[3*indexBuffer[3*uTriangleIndex+0]]);
    V1 = Vector3(&VertexBuffer[3*indexBuffer[3*uTriangleIndex+1]]);
    V2 = Vector3(&VertexBuffer[3*indexBuffer[3*uTriangleIndex+2]]);
}

/// @brief Get normals of triangle
///
/// If vertex normals are present, write the three normals of the triangle
/// corresponding to the index uTriangleIndex to the vectors N0, N1, N2. If
/// vertex normals are not present, the method returns false and does not write
/// to N0, N1, N2.
///
/// @param [in] uTriangleIndex index of triangle
/// @param [out] N0 first vertex normal
/// @param [out] N1 second vertex normal
/// @param [out] N2 third vertex normal
/// @retval true if vertex normals are present
/// @retval false if vertex normals are not present
bool Mesh::getNormalsOfTriangle(std::size_t uTriangleIndex, Vector3& N0, Vector3& N1, Vector3& N2) const
{
    if(m_NormalBuffer.empty())
        // We have no information on normals
        return false;

    const Float *pNormalBuffer = m_NormalBuffer.data();
    const unsigned int *pIndexBuffer = m_IndexBuffer.data();
    N0 = Vector3(&pNormalBuffer[3*pIndexBuffer[3*uTriangleIndex+0]]);
    N1 = Vector3(&pNormalBuffer[3*pIndexBuffer[3*uTriangleIndex+1]]);
    N2 = Vector3(&pNormalBuffer[3*pIndexBuffer[3*uTriangleIndex+2]]);

    return true;
}

/// @brief Compute the curvature tensor
///
/// This method returns the curvature tensor for the triangle corresponding to
/// uTriangleIndex at the barycentric coordinates u,v. If no vertex normals are
/// present, the curvature matrix is the zero matrix.
///
/// The algorithm is described in [1].
///
/// References:
///  - [1] Theisel, Rössl, Zayer, Seidel, Normal based estimation of the
///    curvature tensor for triangular meshes, https://doi.org/10.1109/PCCGA.2004.1348359
///
/// @param [in] uTriangleIndex index of triangle
/// @param [in] u barycentric coordinate
/// @param [in] v barycentric coordinate
/// @param [out] xu normalized tangent vector, xu = d/du x(u,v) / |d/du x(u,v)|
/// @param [out] xv normalized tangent vector, xv = d/dv x(u,v) / |d/dv x(u,v)|
/// @param [in] trafo transformation
/// @retval Q curvature tensor
Matrix2x2 Mesh::getCurvatureTensor(std::size_t uTriangleIndex, Float u, Float v, Vector3& xu, Vector3& xv, const Transformation &trafo) const
{
    // Curvature matrix
    Matrix2x2 Q;

    // Get normals of the three vertices
    Vector3 N0, N1, N2;
    if(!getNormalsOfTriangle(uTriangleIndex, N0, N1, N2))
        // If vertex normals not available, return zero matrix
        return Q;

    // Get triangle vertices
    Vector3 V0, V1, V2;
    getVerticesOfTriangle(uTriangleIndex, V0, V1, V2);

    // Normalized normal at point (u,v)
    const Vector3 normal = trafo.transformNormalToWorld(((1-u-v)*N0 + u*N1 + v*N2));

    const Vector3 nu = N1-N0;
    const Vector3 nv = N2-N0;

    xu = trafo.transformVectorToWorld((V1-V0) - (V1-V0).dot(normal)*normal);
    xv = trafo.transformVectorToWorld((V2-V0) - (V2-V0).dot(normal)*normal);

    Float E = xu.dot(xu);
    Float F = xu.dot(xv);
    Float G = xv.dot(xv);
    Float L = -nu.dot(xu);
    Float M1 = -nu.dot(xv);
    Float M2 = -nv.dot(xu);
    Float N = -nv.dot(xv);

    Float invDenom = 1/(E*G-F*F);
    Q.set(0,0, (L*G-M1*F)*invDenom);
    Q.set(1,0, (M1*E-L*F)*invDenom);
    Q.set(0,1, (M2*G-N*F)*invDenom);
    Q.set(1,1, (N*E-M2*F)*invDenom);

    return Q;
}


/// @brief Intersect ray with mesh
///
/// Intersect a ray with the triangles in this mesh and find the closest
/// intersection point for which tmin <= t <= tmax holds. If the ray
/// intersects a triangle, the barycentric coordinates, the pointer to the
/// material and the normal are saved in rIntersection, and tmax is set to t.
/// Otherwise, false is returned.
///
/// @param [in] ray incident ray
/// @param [out] rIntersection information on the intersection
/// @param [in] tmin minimum value of t (see description)
/// @param [in,out] tmax maximum value of t; if there is an intersection tmax=t (see description)
/// @param [in] uLeaf number of the leaf number of the BVH tree
/// @retval true if ray intersects a triangle
/// @retval false otherwise
bool Mesh::intersectRay(const Ray& ray, Intersection& rIntersection, Float tmin, Float& tmax, std::size_t uLeaf) const
{
    int hit = false;
    if (m_vBbox.empty()) return false; //No Boxes, no intersection.
    if(!m_vBbox[uLeaf].intersectRay(ray, tmin, tmax))
        return false;

    std::size_t childLeft = 2*uLeaf+1, childRight = 2*uLeaf+2;

    if(childRight >= m_vBbox.size())
    {
        Vector3 V0, V1, V2;
        std::size_t uLeft;
        const std::size_t uElems = binaryTreeBoundaries(uLeaf, uLeft);

        const Vector3& O = ray.getOrigin();
        const Vector3& D = ray.getDirection();

        for(std::size_t i = 0; i < uElems; i++)
        {
            std::size_t triangleIndex = uLeft+i;
            getVerticesOfTriangle(triangleIndex, V0, V1, V2);

            Float t,u,v;
            int orientation = intersectRayTriangle(V0, V1, V2, O, D, t, u, v, tmin, tmax);
            if(orientation != 0)
            {
                // there is a hitpoint
                hit = true;
                tmax = t;
                rIntersection.setMesh(this, triangleIndex, t, u, v);
            }
        }
    }
    else
    {
        // left child
        hit += intersectRay(ray, rIntersection, tmin, tmax, childLeft);
        // right child
        hit += intersectRay(ray, rIntersection, tmin, tmax, childRight);
    }

    return hit;
}