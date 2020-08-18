//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      referencelink.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-08-07
/// @brief     Support for glTF extension OpenMaterial_reference_link

#include <fstream>
#include <string>
#include <algorithm>

#include "referencelink.h"

#include "doctest.h"
#include "exceptions.h"
#include "utils.h"

using nlohmann::json;

/// @brief Unit test for Referencelink::Referencelink 
TEST_CASE("Testing Referencelink::Referencelink")
{
	std::string fileName = "../objects/cube_gold.gltf";
	#ifdef _WIN32
		std::replace(fileName.begin(), fileName.end(), '/', '\\');
	#endif

	json j;
	std::ifstream gltfFile(fileName);
	gltfFile >> j;
	gltfFile.close();
	const json jMaterials = j.at("materials");

	// Check if correct material is referenced
	std::string title = "gold";
	bool matchingTitle = false;
	for (const json& jMaterial : jMaterials)
	{
		const ReferenceLink referenceLink = ReferenceLink(jMaterial);
		std::string referenceTitle = referenceLink.getTitle();
		if (referenceTitle == title)
			matchingTitle = true;
	}
	CHECK(matchingTitle);
}

/// @brief Check if json object contains OpenMaterial_reference_link
///
/// The function checks if the json object has the property
/// extensions->OpenMaterial_reference_link. If the property exists, true is
/// returned. Otherwise false.
///
/// @param [in] j json object
/// @retval true if json object contains OpenMaterial_reference_link
/// @retval false otherwise
bool ReferenceLink::hasReferenceLink(const json& j)
{
    if(j.find("extensions") == j.end())
        return false;

    const json jExtensions = j.at("extensions");
    return !(jExtensions.find("OpenMaterial_reference_link") == jExtensions.end());
}

/// Create ReferenceLink object from json object
///
/// If the json object does not contain OpenMaterial_reference_link, a
/// ValueError is thrown. You can check using
/// \ref ReferenceLink::hasReferenceLink if the json object contains a
/// OpenMaterial_reference_link.
///
/// @param [in] j json object
ReferenceLink::ReferenceLink(const nlohmann::json& j)
{
    if(!hasReferenceLink(j))
        throw ValueError("does not contain extensions OpenMaterial_reference_link");

    const json jReferenceLink = j.at("extensions").at("OpenMaterial_reference_link");

    m_sUri = jReferenceLink.at("uri").get<std::string>();

#ifdef _WIN32
	std::replace(m_sUri.begin(), m_sUri.end(), '/', '\\');
#endif

    if(jReferenceLink.find("title") != jReferenceLink.end())
        m_sTitle = jReferenceLink.at("title").get<std::string>();
}

/// @brief Get title of referenced asset
///
/// The title might be an empty string.
std::string ReferenceLink::getTitle() const
{
    return m_sTitle;
}

/// Get URI of referenced asset
std::string ReferenceLink::getUri() const
{
    return m_sUri;
}