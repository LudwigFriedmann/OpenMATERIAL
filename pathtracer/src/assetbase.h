//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetbase.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-08
/// @brief     Main asset class

// Conditional compilation to avoid multiple inclusion:
#ifndef ASSETBASE_H
#define ASSETBASE_H

#include <fstream>
#include <string>
#include <vector>

#include "exceptions.h"
#include "json.hpp"
#include "utils.h"
#include "uuid.h"

/// All available asset types
enum e_AssetType {
    ASSET_TYPE_UNKNOWN = 0,
    ASSET_TYPE_REFERENCE,
    ASSET_TYPE_GEOMETRY,
    ASSET_TYPE_MATERIAL,
    ASSET_TYPE_MATERIAL_IOR,
    ASSET_TYPE_SCENE,
    ASSET_TYPE_SENSOR
};

/// @brief Main asset class
///
/// This class should not be directly instanciated, but derived classes
/// should be used. Other asset implementations should inherit from this class.
/// Each asset has a unique id.
class AssetBase {
private:
/// Asset types as string (must be in the same order as in \ref e_AssetType)
    std::vector<std::string> m_assetTypeString{
            "unknown",
            "reference",
            "geometry",
            "material",
            "material_ior",
            "scene",
            "sensor" };

protected:
    /// Unique id of asset
    Uuid m_uId;

    /// Type of asset
    e_AssetType m_eType = ASSET_TYPE_UNKNOWN;

    /// If set the directory containing the loaded glTF file
    std::string m_sDirectory = "";

public:
    /// Read crsFilename and return json object
    static nlohmann::json readJsonFile(const std::string& crsFilename)
    {
        nlohmann::json j;
        std::ifstream gltfFile(crsFilename);
        if(!gltfFile.is_open())
            throw OSError("Cannot open file '"+crsFilename+"' for reading");

        gltfFile >> j;
        gltfFile.close();

        return j;
    }

    /// Returns true if json object j contains key, otherwise returns false
    static bool hasKey(const nlohmann::json& j, const std::string& key)
    {
        return j.find(key) != j.end();
    }

    /// Get category as string
    std::string assetTypeToString(e_AssetType eType)
    {
        return m_assetTypeString.at(eType);
    }

    /// Get number of asset type (case insensitive)
    e_AssetType stringToAssetType(const std::string &crsType)
    {
        // To lower cases
        std::string sTypeLower = crsType;
        for(auto elem : sTypeLower)
            elem = std::tolower(elem);

        // Find in vector
        for(std::size_t i = 0; i < m_assetTypeString.size(); i++)
            if(sTypeLower == m_assetTypeString[i])
                return static_cast<e_AssetType>(i);

        throw GltfError(getUuid().toString() + ": unknown asset type " + crsType);
    }

    /// Create uninitialized object of AssetBase
    AssetBase() = default;

    /// Create new object of AssetBase
    ///
    /// @param [in] uuid unique id of asset
    /// @param [in] eType type of asset
    explicit AssetBase(const Uuid &uuid, e_AssetType eType = ASSET_TYPE_UNKNOWN)
    {
        m_uId = uuid;
        m_eType = eType;
    }

    /// Create new asset from json object
    explicit AssetBase(const nlohmann::json &j, const std::string& rcsDirectory="")
    {
        m_sDirectory = rcsDirectory + utils::path::getFileSeparator();

        nlohmann::json j_asset_info;

        try
        {
            j_asset_info = j.at("asset").at("extensions").at("OpenMaterial_asset_info");
        }
        catch(const nlohmann::detail::out_of_range& exception)
        {
            throw GltfError("Gltf extension OpenMaterial_asset_info not found");
        }

        std::string sUuid = j_asset_info.at("id").get<std::string>();
        m_uId = Uuid(sUuid);

        std::string sType = j_asset_info.at("asset_type").get<std::string>();
        m_eType = stringToAssetType(sType);
    }

    /// Create new asset from glTF file
    explicit AssetBase(const std::string& crsFilename)
        : AssetBase(readJsonFile(crsFilename), utils::path::dirname(crsFilename))
    {}

    /// Create new asset from glTF file
    explicit AssetBase(const char *prsFilename) : AssetBase(std::string(prsFilename))
    {}

    /// Destructor
    virtual ~AssetBase() = default;

    /// Get unique id of asset
    Uuid getUuid() const { return m_uId; }

    /// Get unique id of asset as string
    std::string getUuidAsString() const { return m_uId.toString(); }

    /// Get directory
    std::string getDirectory() const { return m_sDirectory; }

    /// Get type of asset
    e_AssetType getType() const { return m_eType; }

    /// Get type of asset as string
    std::string getTypeString() const { return m_assetTypeString.at(m_eType); }

    /// Returns true if type of asset is unknown, otherwise returns false
    bool typeIsUnknown() const { return m_eType == ASSET_TYPE_UNKNOWN; }

    /// Returns true if type of asset is reference, otherwise returns false
    bool typeIsReference() const { return m_eType == ASSET_TYPE_REFERENCE; }

    /// Returns true if type of asset is geometry, otherwise returns false
    bool typeIsGeometry() const { return m_eType == ASSET_TYPE_GEOMETRY; }

    /// Returns true if type of asset is material, otherwise returns false
    bool typeIsMaterial() const { return m_eType == ASSET_TYPE_MATERIAL; }

    /// Returns true if type of asset is materialior, otherwise returns false
    bool typeIsMaterialIor() const { return m_eType == ASSET_TYPE_MATERIAL_IOR; }

    /// Returns true if type of asset is scene, otherwise returns false
    bool typeIsScene() const { return m_eType == ASSET_TYPE_SCENE; }

    /// Returns true if type of asset is sensor, otherwise returns false
    bool typeIsSensor() const { return m_eType == ASSET_TYPE_SENSOR; }
};

#endif // ASSETBASE_H