//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetgeometry.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     AssetGeometry class

#ifndef ASSETGEOMETRY_H
#define ASSETGEOMETRY_H

#include <vector>

#include "assetinfo.h"
#include "assetmaterial.h"
#include "bbox.h"
#include "constants.h"
#include "defines.h"
#include "mesh.h"
#include "intersect.h"
#include "ray.h"
#include "node.h"
#include "exceptions.h"
#include "tiny_gltf.h"

/// @brief Class for assets of type geometry
///
/// This class tries to be as close to the idea of the glTF standard as
/// possible. From a glTF file the active scene is loaded. A scene consists
/// of one or more nodes. The node structure is hierarchical and corresponds to
/// a tree. Each node can define a local coordinate system and have zero or one
/// meshes of type Mesh. A mesh contains the vertices of the triangles and is
/// associated with a material of type AssetMaterial.
class AssetGeometry : public AssetInfo
{
private:
    /// List of materials
    std::vector<const AssetMaterial *> m_vpMaterials;

    /// List of pointers to all nodes
    std::vector<Node *> m_vpNodes;

    /// List of the scene nodes
    std::vector<int> m_viSceneNodes;

    /// List of pointers to all meshes
    std::vector<Mesh *> m_vpMeshes;

    /// Directory containing the asset (necessary to resolve OpenMaterial_reference_link links)
    std::string m_sDirectory;

    void cleanup();

    void loadMeshes(const tinygltf::Model& gltfModel);
    void loadMaterials(const tinygltf::Model& gltfModel);
    void loadNodes(const tinygltf::Model& gltfModel);

public:
    static tinygltf::Model loadGltfModel(const std::string& crsFilename);

    explicit AssetGeometry(const tinygltf::Model& gltfModel, const std::string& crsDirectory="");

    /// Create asset geometry specifying the tinygltf model
    explicit AssetGeometry(const std::string& crsFilename);

    ~AssetGeometry() override;

    BBox getBBox() const;
    /// It returns the nodes which are loaded in the scene
    std::vector<int> getviSceneNodes(){return m_viSceneNodes;};
    
    void loadDisplacementMap(const std::string& mapName, float m_displacementmap, const std::string& UVmaptype);
    void BVH();

    bool intersectRay(const Ray& ray, Intersection& rIntersection, Float tmin=0, Float tmax=fInfinity) const;
};

#endif // ASSETGEOMETRY_H