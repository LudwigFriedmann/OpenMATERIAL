//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetinfo.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-15
/// @brief     Information about asset

// Conditional compilation to avoid multiple inclusion:
#ifndef ASSETINFO_H
#define ASSETINFO_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "assetbase.h"
#include "json.hpp"

/// All asset categories
enum e_AssetCategory {
    ASSET_CATEGORY_UNKNOWN = 0,
    ASSET_CATEGORY_UNLABELED,
    ASSET_CATEGORY_EGO_VEHICLE,
    ASSET_CATEGORY_RECTIFICATION_BORDER,
    ASSET_CATEGORY_OUT_OF_ROI,
    ASSET_CATEGORY_STATIC,
    ASSET_CATEGORY_DYNAMIC,
    ASSET_CATEGORY_GROUND,
    ASSET_CATEGORY_ROAD,
    ASSET_CATEGORY_SIDEWALK,
    ASSET_CATEGORY_PARKING,
    ASSET_CATEGORY_RAIL_TRACK,
    ASSET_CATEGORY_BUILDING,
    ASSET_CATEGORY_WALL,
    ASSET_CATEGORY_FENCE,
    ASSET_CATEGORY_GUARD_RAIL,
    ASSET_CATEGORY_BRIDGE,
    ASSET_CATEGORY_TUNNEL,
    ASSET_CATEGORY_POLE,
    ASSET_CATEGORY_POLEGROUP,
    ASSET_CATEGORY_TRAFFIC_LIGHT,
    ASSET_CATEGORY_TRAFFIC_SIGN,
    ASSET_CATEGORY_VEGETATION,
    ASSET_CATEGORY_TERRAIN,
    ASSET_CATEGORY_SKY,
    ASSET_CATEGORY_PERSON,
    ASSET_CATEGORY_RIDER,
    ASSET_CATEGORY_CAR,
    ASSET_CATEGORY_TRUCK,
    ASSET_CATEGORY_BUS,
    ASSET_CATEGORY_CARAVAN,
    ASSET_CATEGORY_TRAILER,
    ASSET_CATEGORY_TRAIN,
    ASSET_CATEGORY_MOTORCYCLE,
    ASSET_CATEGORY_BICYCLE,
    ASSET_CATEGORY_LICENSE_PLATE
};

/// @brief Additional information of assets
///
/// This class inherits directly from the AssetBase class and extends it
/// with additional asset properties like title, creator or description.
class AssetInfo : public AssetBase {
private:
/// Human readable strings of asset categories (must be in the same order as in
/// \ref e_AssetCategory
    std::vector<std::string> m_assetCategoryString{
            "unknown",
            "unlabeled",
            "ego-vehicle",
            "rectification-border",
            "out-of-roi",
            "static",
            "dynamic",
            "ground",
            "road",
            "sidewalk",
            "parking",
            "rail-track",
            "building",
            "wall",
            "fence",
            "guard-rail",
            "bridge",
            "tunnel",
            "pole",
            "polegroup",
            "traffic-light",
            "traffic-sign",
            "vegetation",
            "terrain",
            "sky",
            "person",
            "rider",
            "car",
            "truck",
            "bus",
            "caravan",
            "trailer",
            "train",
            "motorcycle",
            "bicycle",
            "license-plate" };

protected:
    // required properties in OpenMaterial_asset_info (id, asset_type are in
    // AssetBase)

    /// Title of asset
    std::string m_sTitle;

    /// Creator of asset
    std::string m_sCreator;

    // Optional properties in OpenMaterial_asset_info
    // not implemented: asset_parent, asset_version, asset_variation, sources

    /// Category of asset
    e_AssetCategory m_eCategory = ASSET_CATEGORY_UNKNOWN;

    /// Creation date of asset
    std::string m_sCreationDate = "";

    /// Human readable description of the asset
    std::string m_sDescription = "";

    /// Tags of the asset
    std::string m_sTags = "";

public:
    /// Get category as string
    std::string assetCategoryToString(e_AssetCategory eCategory)
    {
        return m_assetCategoryString.at(eCategory);
    }

    /// Get number of string category (case insensitive)
    e_AssetCategory stringToAssetCategory(const std::string& crsCategory)
    {
        std::string sCategoryLower = crsCategory;
        for(auto elem : sCategoryLower)
            elem = std::tolower(elem);

        for(std::size_t i = 0; i < m_assetCategoryString.size(); i++)
            if(sCategoryLower == m_assetCategoryString[i])
                return static_cast<e_AssetCategory>(i);

        throw GltfError(getUuid() + ": unknown asset category " + crsCategory);
    }

    /// Create uninitialized object of AssetInfo
    AssetInfo() = default;

    /// Create new object of AssetInfo
    ///
    /// @param [in] uuid unique id of asset
    /// @param [in] assetType type of asset
    /// @param [in] sTitle title of asset
    /// @param [in] sCreator creator of asset
    AssetInfo(const Uuid &uuid, e_AssetType assetType, std::string sTitle="", std::string sCreator="") :
      AssetBase(uuid, assetType),
      m_sTitle(std::move(sTitle)),
      m_sCreator(std::move(sCreator))
    { }

    /// Create new object from json object
    ///
    /// @param [in] j json object
    /// @param [in] rcsDirectory directory of json file or "" otherwise
    explicit AssetInfo(const nlohmann::json& j, const std::string &rcsDirectory="")
        : AssetBase(j, rcsDirectory)
    {
        // Parse OpenMaterial_asset_info
        const nlohmann::json j_asset_info = j.at("asset").at("extensions").at("OpenMaterial_asset_info");

        // User_preferences.at("material_scheme").Get<std::string>()
        m_sTitle   = j_asset_info.at("title").get<std::string>();
        m_sCreator = j_asset_info.at("creator").get<std::string>();

        // Parse optional properties
        if(hasKey(j_asset_info, "category"))
        {
        	std::string sCategory = j_asset_info.at("category").get<std::string>();
        	setCategory(stringToAssetCategory(sCategory));
        }
        if(hasKey(j_asset_info, "creation_date"))
        {
        	std::string sCreationDate = j_asset_info.at("creation_date").get<std::string>();
        	setCreationDate(sCreationDate);
        }
        if(hasKey(j_asset_info, "description"))
        {
        	std::string sDescription = j_asset_info.at("description").get<std::string>();
        	setDescription(sDescription);
        }
        if(hasKey(j_asset_info, "tags"))
        {
        	std::string sTags = j_asset_info.at("tags").get<std::string>();
        	setTags(sTags);
        }
    }

    /// Create new asset from filename
    explicit AssetInfo(const std::string& rcsFilename) : AssetInfo(readJsonFile(rcsFilename), utils::path::dirname(rcsFilename)) {}

    /// Create new asset from filename
    explicit AssetInfo(const char *cpFilename) : AssetInfo(std::string(cpFilename)) {}

    /// Destructor
    ~AssetInfo() override = default;

    /// Get title of asset
    std::string getTitle() const { return m_sTitle; }

    /// Get creator of asset
    std::string getCreator() const { return m_sCreator; }


    // --- Setters and getters for optional parameters ---

    /// Set category of asset
    void setCategory(e_AssetCategory eAssetCategory) { m_eCategory = eAssetCategory; }

    /// Get category of asset
    e_AssetCategory getCategory() const { return m_eCategory; }

    /// Get category of asset as string
    std::string getCategoryString() const { return m_assetCategoryString.at(m_eCategory); }

    /// Set creation date
    void setCreationDate(std::string sCreationDate) { m_sCreationDate = std::move(sCreationDate); }

    /// Get creation date
    std::string getCreationDate() const { return m_sCreationDate; }

    /// Set description
    void setDescription(std::string sDescription) { m_sDescription = std::move(sDescription); }

    /// Get description
    std::string getDescription() const { return m_sDescription; }

    /// Set tags
    void setTags(std::string sTags) { m_sTags = std::move(sTags); }

    /// Get tags
    std::string getTags() const { return m_sTags; }
};

#endif // ASSETINFO_H