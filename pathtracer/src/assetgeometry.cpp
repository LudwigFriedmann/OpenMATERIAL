//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetgeometry.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-30
/// @brief     AssetGeometry class

#include <cstring> // for memcpy

#include <cstddef> // std:size_t
#include <iostream>
#include <vector>

#include "assetgeometry.h"
#include "assetmaterial.h"
#include "defines.h"
#include "doctest.h"
#include "exceptions.h"
#include "mesh.h"
#include "node.h"
#include "transformation.h"
#include "vector.h"
#include "intersect.h"
#include "tiny_gltf.h"
#include "referencelink.h"
#include "json.hpp"
#include "utils.h"
#include "CImg.h"

using nlohmann::json;
using std::string;
using std::vector;
using::size_t;
using std::to_string;
using namespace cimg_library;
using namespace std;

/// @brief Unit test for AssetGeometry::AssetGeometry
TEST_CASE("Testing AssetGeometry::AssetGeometry")
{
	std::string fileName = "../objects/cube_gold.gltf";
	#ifdef _WIN32
		std::replace(fileName.begin(), fileName.end(), '/', '\\');
	#endif

	// Check if loaded geometry has nodes
	AssetGeometry assetGeometry(fileName); //calls AssetGeometry::loadGltfModel
	CHECK(assetGeometry.getviSceneNodes().size() == 1);
}

/// @brief Unit test for AssetGeometry::getBBox
TEST_CASE("Testing AssetGeometry::getBBox")
{
	std::string fileName = "../objects/cube_gold.gltf";
	#ifdef _WIN32
		std::replace(fileName.begin(), fileName.end(), '/', '\\');
	#endif
	
	// Check if the bounding box has spatial extension
	AssetGeometry assetGeometry(fileName);
	CHECK(assetGeometry.getBBox().getMax().length() > 0.0f);
}

/// Copy indices from glTF data format
///
/// Copy count elements of data type componentType from address src to the
/// memory address given by the pointer dest.
///
///
/// @param [in] dest destination address
/// @param [in] src source address
/// @param [in] componentType e.g. 5126 for float (see glTF specification for more details)
/// @param [in] count number of elements
static void copy_indices_from_array(unsigned int *dest, void const *src, int componentType, size_t count)
{
    if(componentType == TINYGLTF_COMPONENT_TYPE_BYTE)
    {
        auto p = reinterpret_cast<const signed char * const>(src);
        for(size_t i = 0; i < count; i++)
            dest[i] = p[i];
    }
    else if(componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
    {
        auto p = reinterpret_cast<const unsigned char * const>(src);
        for(size_t i = 0; i < count; i++)
            dest[i] = p[i];
    }
    else if(componentType == TINYGLTF_COMPONENT_TYPE_SHORT)
    {
        auto p = reinterpret_cast<const short * const>(src);
        for(size_t i = 0; i < count; i++)
            dest[i] = p[i];
    }
    else if(componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
    {
        auto p = reinterpret_cast<const unsigned short * const>(src);
        for(size_t i = 0; i < count; i++)
            dest[i] = p[i];
    }
    else if(componentType == TINYGLTF_COMPONENT_TYPE_INT)
    {
        auto p = reinterpret_cast<const int * const>(src);
        for(size_t i = 0; i < count; i++)
            dest[i] = p[i];
    }
    else if(componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
    {
        auto p = reinterpret_cast<const unsigned int * const>(src);
        for(size_t i = 0; i < count; i++)
            dest[i] = p[i];
    }
}

/// @brief Check if node uNodeNumber is a root node
///
/// @param [in] gltfModel tinygltf object
/// @param [in] uNodeNumber node number
static bool checkNodeIsRoot(const tinygltf::Model& gltfModel, size_t uNodeNumber)
{
    // Check if any node contains uNodeNumber as child...
    for(const auto &gltfNode : gltfModel.nodes)
    {
        const auto& children = gltfNode.children;
        if(std::find(children.begin(), children.end(), uNodeNumber) != children.end())
            return false;
    }

    return true;
}


/// @brief Recalculates Normals after displacement
///
/// @param [in] IndexBuffer(triangles), Vertexes and Normals lists
static void recalculateNormals(vector<unsigned int>& vIndexBuffer, vector<Float>& vNormalBuffer, vector<Float>& vVertexBuffer)
{
    int FN=static_cast<int>(vIndexBuffer.size()/3);
    int VN=static_cast<int>(vVertexBuffer.size()/3);
    // For (auto v:vNormalBuffer) v=0.0;
        for (int i=0; i<VN; ++i){
            vNormalBuffer[3*i]=0.0;
            vNormalBuffer[3*i+1]=0.0;
            vNormalBuffer[3*i+2]=0.0;
        }
    for (int i=0; i<FN; ++i)
    {
        Vector3 V0(vVertexBuffer[vIndexBuffer[3*i]*3],vVertexBuffer[vIndexBuffer[3*i]*3+1],vVertexBuffer[vIndexBuffer[3*i]*3+2]);
        Vector3 V1(vVertexBuffer[vIndexBuffer[3*i+1]*3],vVertexBuffer[vIndexBuffer[3*i+1]*3+1],vVertexBuffer[vIndexBuffer[3*i+1]*3+2]);
        Vector3 V2(vVertexBuffer[vIndexBuffer[3*i+2]*3],vVertexBuffer[vIndexBuffer[3*i+2]*3+1],vVertexBuffer[vIndexBuffer[3*i+2]*3+2]);
        Vector3 S0=V1-V0;
        Vector3 S1=V2-V0;
        Vector3 N=S0.cross(S1).normalize();
        if(isnan(N[0]) or (isnan(N[2]))){N[0]=1;N[1]=0;N[2]=0;} //FIXME Why sometime the N vector is a nan?

        for (int j=0; j<3; ++j)
        {
            vNormalBuffer[vIndexBuffer[3*i+j]*3]+=N[0];
            vNormalBuffer[vIndexBuffer[3*i+j]*3+1]+=N[1];
            vNormalBuffer[vIndexBuffer[3*i+j]*3+2]+=N[2];
        }
    }
    for (int i=0; i<VN; ++i)
    {
        Float L=1.0/Vector3(vNormalBuffer[3*i],vNormalBuffer[(3*i)+1],vNormalBuffer[(3*i)+2]).length();
        vNormalBuffer[3*i]*=L;
        vNormalBuffer[3*i+1]*=L;
        vNormalBuffer[3*i+2]*=L;
    }
}

/// @brief Calculates the UV mapping for simple objects
///
/// @param [in] Vertex coordinates, UV map extension, UV map and type of UV
/// @param [out] corresponding (gray) value of the pixel in UV map
static float UVmapping(float x,float y,float z, int width, int height, const string &maptype, CImg<unsigned char> dispMap){
    double u=0.0;
    double v=0.0;
    if (maptype=="sphere" || maptype=="cylinder" || maptype=="cube"){
        if (maptype=="sphere"){
	    u = 0.5 + atan2(z, x) / (2*M_PI);
	    v = 0.5 - asin(y) / M_PI;
            if(isnan(v)) {v=1;}
        }
        if (maptype=="cylinder"){ //https://stackoverflow.com/questions/34958072/programmatically-generate-simple-uv-mapping-for-models
            u = atan2(x,z)/M_PI *0.5 +0.5;
            v = y/2+0.5;
        }
        if (maptype=="cube"){ //https://en.wikipedia.org/wiki/Cube_mapping
            float offu=0.0;
            float offv=0.0;
            float absX = fabs(x); float absY = fabs(y); float absZ = fabs(z);
            int isXPositive = x > 0 ? 1 : 0; int isYPositive = y > 0 ? 1 : 0; int isZPositive = z > 0 ? 1 : 0;
            if (isXPositive && absX >= absY && absX >= absZ) {offu = 0.5; offv = 0.333;} //whichFace=0 maxAxis = absX; uc = -z; vc = y;
            if (!isXPositive && absX >= absY && absX >= absZ) {offu = 0; offv = 0.333;}  //whichFace=1 {maxAxis = absX; uc = z; vc = y;
            if (isYPositive && absY >= absX && absY >= absZ) {offu = 0.25; offv = 0.666;}  //whichFace=2 maxAxis = absY; uc = x; vc = -z;
            if (!isYPositive && absY >= absX && absY >= absZ) {offu = 0.25; offv = 0;}  //whichFace=3 maxAxis = absY; uc = x;vc = z;
            if (isZPositive && absZ >= absX && absZ >= absY) {offu = 0.25; offv = 0.333;}  //whichFace=4 maxAxis = absZ; uc = x; vc = y;
            if (!isZPositive && absZ >= absX && absZ >= absY) {offu = 0.75; offv = 0.333;}  //whichFace=5 maxAxis = absZ; uc = -x; vc = y;
            u = x/8.0 + 1/8.0 + offu;
            v = y/6.0 + 1/6.0 + offv;
         }
    }
    else{
        std::random_device rd;
        std::map<int, int> hist;
        std::uniform_int_distribution<int> dist(0, 99);
        u=dist(rd)%100/100.0;
        v=dist(rd)%100/100.0;
    }

    if (v<0 or isnan(v)){v=0;} // Protection agaist not-normalized objects
    if (v>1){v=1;}
    if (u<0 or isnan(u)){u=0;}
    if (u>1){u=1;}
    int u_map=static_cast<int>(u*width);
    int v_map=static_cast<int>(v*height);
    return dispMap(u_map,v_map,0,0);
}

/// @brief Read and produce the gray scale of an arbitrary displacement map
/// @param [in] Average of the grey scale pixels, name of the coloured map
/// @param [out] gray scale version of the coloured map

CImg<unsigned char> readDisplacementMap(float &graymean, const string &mapName){
    CImg<unsigned char> src(mapName.c_str());
    CImg<unsigned char> gray(src.width(),src.height(),1,1,0);
    int width = src.width();
    int height = src.height();
    for (int r = 0; r < height; r++)
            for (int c = 0; c < width; c++){
                   int grayValue = (int)(0.33*(int)src(r,c,0,0) + 0.33*(int)src(r,c,0,1) + 0.33*(int)src(r,c,0,2));
                   gray(r,c,0,0) = grayValue;
                   graymean+=static_cast<float>(grayValue);
            }
   graymean=graymean/static_cast<float>(width*height);
   return gray;
}

/// @brief Generates displacements of vertexes according to the normal direction
///
/// @param [in] Vertex coordinates, UV map extension, UV map and type of UV
///
static void generateDisplacement(vector<Float>& vNormalBuffer, vector<Float>& vVertexBuffer, const string &mapName, float magnify, const string &UVmaptype){
    float graymean=0;
    CImg<unsigned char> dispMap=readDisplacementMap(graymean, mapName);
    int width = dispMap.width();
    int height = dispMap.height();
    for (unsigned i=0; i < vVertexBuffer.size()/3; i++) {
        float d=(UVmapping(vVertexBuffer[i*3],vVertexBuffer[i*3+1],vVertexBuffer[i*3+2],width,height,UVmaptype,dispMap)-graymean)/graymean*magnify;
        vVertexBuffer[3*i]=vVertexBuffer[3*i]+d*vNormalBuffer[3*i];
        vVertexBuffer[3*i+1]=vVertexBuffer[3*i+1]+d*vNormalBuffer[3*i+1];
        vVertexBuffer[3*i+2]=vVertexBuffer[3*i+2]+d*vNormalBuffer[3*i+2];
    }
}

/// @brief Destructor
///
/// The destructor calls \ref cleanup.
AssetGeometry::~AssetGeometry()
{
    cleanup();
}

/// @brief Free allocated memory.
///
/// Free allocated memory. This method is called by the destructor.
void AssetGeometry::cleanup()
{
    // free memory for the materials
    for(const AssetMaterial *pMaterial : m_vpMaterials)
        delete pMaterial;

    // free memory for meshes
    for(const Mesh *pMesh : m_vpMeshes)
        delete pMesh;

    // free memory for nodes
    for(const Node *pNode : m_vpNodes)
        delete pNode;
}

/// @brief Load all materials in the glTF model
///
/// Load all materials of the glTF file and save it in m_vpMaterials.
///
/// @param [in] gltfModel tiny_gltf model
void AssetGeometry::loadMaterials(const tinygltf::Model& gltfModel)
{
    const json jMaterials = gltfModel.j.at("materials");

    for(const json& jMaterial : jMaterials)
    {
        const ReferenceLink referenceLink = ReferenceLink(jMaterial);
        string sFilename = m_sDirectory + referenceLink.getUri();
        auto *pMaterial = new AssetMaterial(sFilename);
        m_vpMaterials.push_back(pMaterial);
    }
}


/// @brief Load node hierarchy
///
/// Load all nodes from the the glTF file.
///
/// Note: \ref loadMeshes must be called before this method.
///
/// @param [in] gltfModel tiny_gltf model
void AssetGeometry::loadNodes(const tinygltf::Model& gltfModel)
{
    // number of all nodes
    size_t uNodes = gltfModel.nodes.size();

    // Create uninitialized nodes
    m_vpNodes.reserve(uNodes);
    for(size_t i = 0; i < uNodes; i++)
        m_vpNodes.push_back(new Node(i));

    // Initialize the nodes
    for(size_t uNodeNumber = 0; uNodeNumber < uNodes; uNodeNumber++)
    {
        const tinygltf::Node& gltfNode = gltfModel.nodes[uNodeNumber];
        Node *pCurrentNode = m_vpNodes[uNodeNumber];

        // Set name of the current node
        pCurrentNode->setName(gltfNode.name);

        // Add the mesh to the node; a node has either one or no mesh. The mesh
        // number is saved in gltfNode.mesh iff the number is not negative.
        if(gltfNode.mesh >= 0)
        {
            size_t uMeshNumber = gltfNode.mesh;
            if(uMeshNumber >= m_vpMeshes.size())
                throw GltfError(getUuid() + ": unresolved reference to mesh " + to_string(uMeshNumber));

            pCurrentNode->setMesh(m_vpMeshes[uMeshNumber]);
        }

        // Read the transformation for the node
        if(!gltfNode.matrix.empty())
        {
            Matrix3x3 M;
            Vector3 t(gltfNode.matrix[3], gltfNode.matrix[7], gltfNode.matrix[11]);

            // 4x4 transformation matrix is stored in column-major format
            M.set(0,0, gltfNode.matrix[0]);
            M.set(1,0, gltfNode.matrix[1]);
            M.set(2,0, gltfNode.matrix[2]);

            M.set(0,1, gltfNode.matrix[4]);
            M.set(1,1, gltfNode.matrix[5]);
            M.set(2,1, gltfNode.matrix[6]);

            M.set(0,2, gltfNode.matrix[8]);
            M.set(1,2, gltfNode.matrix[9]);
            M.set(2,2, gltfNode.matrix[10]);

            m_vpNodes[uNodeNumber]->setTransformation(Transformation(M, t));
        }
        else
        {
            // Default values that correspond to identity
            Vector3 translation(0,0,0);
            Vector3 scale(1,1,1);
            Quaternion rotation(0,0,0,1);

            if(!gltfNode.scale.empty())
                scale = Vector3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]);
            if(!gltfNode.rotation.empty())
                rotation = Quaternion(gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2], gltfNode.rotation[3]);
            if(!gltfNode.translation.empty())
                translation = Vector3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]);

            Transformation transformation = Transformation::TRS(translation, rotation, scale);
            m_vpNodes[uNodeNumber]->setTransformation(transformation);
        }

        // Add children
        for(size_t uChild : gltfNode.children)
        {
            // Check that the referenced node exists
            if(uChild >= m_vpNodes.size())
                throw GltfError(getUuid() + ": unresolved reference to node " + to_string(uChild));

            Node *childNode = m_vpNodes[uChild];
            // Add child to current node
            pCurrentNode->addChild(childNode);
            // Set parent of child node
            childNode->setParent(pCurrentNode);
        }
    }

    // Check that all nodes in scene are root nodes
    for(int iNodeNumber : m_viSceneNodes)
        if(!checkNodeIsRoot(gltfModel, iNodeNumber))
            throw GltfError(getUuid() + ": node " + to_string(iNodeNumber) + " not a root node");

    // Check that node structure is not cyclic
    // The node structure is cyclic if one of node has a descent that is
    // itself; i.e., if one parent, grandparent... has the same node number
    for(const Node *pNode : m_vpNodes)
        if(pNode->hasParent(pNode->getNodeNumber()))
            throw GltfError(getUuid() + ": node structure is cyclic");
}


/// @brief Function to load the parameters contained in GLFT files
///
/// @param [in] File stringname
///
/// @param [out] Model to extract the parameters

tinygltf::Model AssetGeometry::loadGltfModel(const string& crsFilename)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    string err, warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, crsFilename);
    if(!ret)
        throw GltfError("Could not load glTF file " + crsFilename + ": " + err);

    return model;
}

/// @brief Load all meshes
///
/// Load all meshes from the glTF file.
///
/// Note: \ref loadMaterials must be called before this method.
///
/// @param [in] gltfModel tiny_gltf model
void AssetGeometry::loadMeshes(const tinygltf::Model& gltfModel)
{
    for(const tinygltf::Mesh& gltfMesh : gltfModel.meshes)
    {
        Mesh *pMesh = new Mesh(gltfMesh.name);
        m_vpMeshes.push_back(pMesh);

        vector<Float>& vVertexBuffer = pMesh->getVertexBuffer();
        vector<Float>& vNormalBuffer = pMesh->getNormalBuffer();
        vector<unsigned int>& vIndexBuffer = pMesh->getIndexBuffer();

        for(const auto& meshPrimitive : gltfMesh.primitives)
        {
            const auto& indicesAccessor = gltfModel.accessors[meshPrimitive.indices];
            const auto& bufferView = gltfModel.bufferViews[indicesAccessor.bufferView];
            const auto& buffer = gltfModel.buffers[bufferView.buffer];
            const auto dataAddress = buffer.data.data() + bufferView.byteOffset + indicesAccessor.byteOffset;

            // Only triangles are supported as primitives
            if(meshPrimitive.mode != TINYGLTF_MODE_TRIANGLES)
                throw NotImplementedError(getUuid() + ": primitives other than triangles not supported");

            if(meshPrimitive.material >= 0){
                pMesh->setMaterial(m_vpMaterials.at(meshPrimitive.material));
            }
            else
                throw ValueError(getUuid() + ": no material for mesh " + pMesh->getName());

            // Buffer for the triangle indices:
            //   - each element in the buffer consists of three integers
            //     representing the three vertices of each triangle
            //     => size = 3*sizeof(unsigned int)
            //   - the number of elements corresponds to the number of
            //     triangles is indicesAccessor.count/3
            //
            // Summary: There are indicesAccessor.count of unsigned int
            vIndexBuffer.resize(indicesAccessor.count);
            copy_indices_from_array(vIndexBuffer.data(), dataAddress, indicesAccessor.componentType, indicesAccessor.count);

            for(const auto& attribute : meshPrimitive.attributes)
            {
                const auto attribAccessor = gltfModel.accessors[attribute.second];
                const auto& meshBufferView = gltfModel.bufferViews[attribAccessor.bufferView];
                const auto& meshBuffer = gltfModel.buffers[meshBufferView.buffer];
                const auto dataPtr = meshBuffer.data.data() + meshBufferView.byteOffset + attribAccessor.byteOffset;

                if(attribute.first == "POSITION")
                {
                    if(attribAccessor.type != TINYGLTF_TYPE_VEC3)
                        throw NotImplementedError(getUuid() + ": accessor data type not supported");
                    if(attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                        throw NotImplementedError(getUuid() + ": accessor component type not supported");

                    // vertex buffer
                    // 3D vector of float
                    //
                    // there are 3*indicesAccessor.count of floats
                    vVertexBuffer.resize(3*attribAccessor.count);
                    memcpy(vVertexBuffer.data(), dataPtr, 3 * sizeof(Float) * attribAccessor.count);
                }
                else if(attribute.first == "NORMAL")
                {
                    if (attribAccessor.type != TINYGLTF_TYPE_VEC3)
                        throw NotImplementedError(getUuid() + ": accessor data type not supported");

                    if (attribAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                        throw NotImplementedError(getUuid() + ": accessor component type not supported");

                    // 3D vector of float
                    vNormalBuffer.resize(3*attribAccessor.count);
                    memcpy(vNormalBuffer.data(), dataPtr, 3 * sizeof(Float) * attribAccessor.count);
                }
            }
        }
    }
}


/// Create asset geometry object from tingltf model
AssetGeometry::AssetGeometry(const tinygltf::Model& gltfModel, const std::string &crsDirectory) : AssetInfo(gltfModel.j)
{
    // Check if asset is of type geometry
    if(!typeIsGeometry())
        throw GltfError(getUuid() + ": asset is not of type geometry");

    m_sDirectory = crsDirectory + utils::path::getFileSeparator();

    // Get the default scene. The property "scene" is optional and tinygltf will
    // set it to -1 if not present. If scene is not present, use the first scene
    // and set uDefaultScene to 0.
    size_t uDefaultScene = 0;
    if(gltfModel.defaultScene >= 0)
        uDefaultScene = gltfModel.defaultScene;

    // Check if the default scene is present in scenes
    if(gltfModel.scenes.size() <= uDefaultScene)
        throw GltfError(getUuid() + ": default scene not present");

    // Save the nodes belonging to the scene
    m_viSceneNodes = gltfModel.scenes[uDefaultScene].nodes;

    try
    {
        // Do not change order: loadMaterials, loadMeshes, loadNodes

        // Load materials
        loadMaterials(gltfModel);
        // Load meshes
        loadMeshes(gltfModel);
        // Load nodes
        loadNodes(gltfModel);

        // Check that all referenced scene nodes actually exist
        for(size_t node : m_viSceneNodes)
            if(node >= m_vpNodes.size())
                throw GltfError(getUuid() + ": unresolved reference to node " + to_string(node));
    }
    catch(...)
    {
        // Tidy up and rethrow exception
        cleanup();
        throw;
    }
}

/// @brief Compute the bounding box of this asset
///
/// The minimum axis-aligned bounding box in world coordinates is returned.
///
/// This method is rather expensive to call. As nodes are hierarchical and
/// contain transformations, this method has to iterate over every vertex point,
/// transform the vertex point to world coordinates, and thereby compute the
/// bounding box.
///
/// @retval bbox axis-aligned minimum bounding box
BBox AssetGeometry::getBBox() const
{
    BBox bbox;

    for(std::size_t i = 0; i < m_vpNodes.size(); i++)
    {
        const Node *pNode = m_vpNodes[i];
        if(i == 0)
            bbox = pNode->getBBox();
        else
            bbox = bbox.surrround(pNode->getBBox());
    }

    return bbox;
}

AssetGeometry::AssetGeometry(const std::string& crsFilename)
    : AssetGeometry(loadGltfModel(crsFilename), utils::path::dirname(crsFilename)) {}

/// @brief Intersect ray with this asset
///
/// Intersect ray with this asset. The distance t from the origin of the ray to
/// the hit point must satisfy tmin <= 0 <= tmax. If the ray intersects a
/// triangle of a mesh and tmin <= 0 <= tmax, true is returned and rIntersection
/// contains information on the hit point.
///
/// The method checks every scene node and calls the corresponding
/// Node::intersectRay methods.
///
/// Please note that this method overwrites tmax. If the ray intersects geometry,
/// tmax equals t. If not, tmax should be ignored.
///
/// @param [in] ray ray
/// @param [out] rIntersection reference to intersection object
/// @param [in] tmin minimum value of t (see text description)
/// @param [in,out] tmax maximum value of t (see text description)
bool AssetGeometry::intersectRay(const Ray& ray, Intersection& rIntersection, Float tmin, Float tmax) const
{
    bool hit = false;

    for(size_t uNodeNumber : m_viSceneNodes)
    {
        const Node *node = m_vpNodes[uNodeNumber];
        if(node->intersectRay(ray, rIntersection, tmin, tmax))
            hit = true;
    }

    if(hit)
    {
        const Node *pNode = m_vpNodes[rIntersection.getNodeNumber()];
        Transformation transformation = pNode->getTransformation();

        while((pNode = pNode->getParent()) != nullptr)
        {
            transformation = pNode->getTransformation().apply(transformation);
        }

        rIntersection.setTransformation(transformation);
    }

    return hit;
}

/// @brief Generates vertexes displacement according to some displacement map and recalculates normals
/// @param [in] Name of the coloured displacement map
/// @param [in] displacement magnificator facotr (to magnify/reduce the displacement. Default value is 10%)
/// @param [in] Select which UV mapping should be applied (spherical, cylindrical, cube or random)
void AssetGeometry::loadDisplacementMap(const std::string& mName, float m_displacementmap, const std::string& m_UVmaptype)
{
    if (!mName.empty()){
      std::string mapName=mName;
      std::replace(mapName.begin(), mapName.end(), '/', utils::path::getFileSeparator());
        std::cout<<"Displacement Mapping on->"<<mapName<<", displacement value->"<<m_displacementmap<<", UV mapping ->"<<m_UVmaptype<<std::endl;
        for(Mesh *pMesh : m_vpMeshes){
            vector<Float>& vVertexBuffer = pMesh->getVertexBuffer();
            vector<Float>& vNormalBuffer = pMesh->getNormalBuffer();
            vector<unsigned int>& vIndexBuffer = pMesh->getIndexBuffer();
            generateDisplacement(vNormalBuffer, vVertexBuffer,mapName, m_displacementmap, m_UVmaptype);
            recalculateNormals(vIndexBuffer, vNormalBuffer, vVertexBuffer);
            }
    }
}

/// @brief Apply the Bonding Volume Hierarchy (BVH) algo to generate the bounding boxes
/// it retrieves all the meshes through the m_vpMeshes container and apply the algo, which is
/// specified in Mesh.cpp
void AssetGeometry::BVH()
{
    for(Mesh *pMesh : m_vpMeshes){
        pMesh->buildBVH();
    }
}