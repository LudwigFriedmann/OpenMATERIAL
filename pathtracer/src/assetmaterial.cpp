//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetmaterial.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-08-01
/// @brief     Material properties and methods to load from glTF model

#include "assetmaterial.h"
#include "doctest.h"

#include <fstream>
#include <exception>

/// @brief Unit test for AssetMaterial::AssetMaterial 
TEST_CASE("Testing AssetMaterial::AssetMaterial")
{
	std::string fileName = "../materials/aluminium.gltf";
	#ifdef _WIN32
		std::replace(fileName.begin(), fileName.end(), '/', '\\');
	#endif

	// Check if correct material is loaded
	auto *pMaterial = new AssetMaterial(fileName);
	std::string title = "material_aluminium";
	CHECK(title.compare(pMaterial->getTitle()) == 0);
}

/// Destructor
AssetMaterial::~AssetMaterial()
{
	delete m_pMaterialIor;
}

/// Create uninitialized object
AssetMaterial::AssetMaterial() = default;

/// @brief Load material parameters from glTF file
///
/// @param [in] j json object
/// @param [in] rcsDirectory path to the directory containing the glTF file
AssetMaterial::AssetMaterial(const nlohmann::json& j, const std::string& rcsDirectory)
    : AssetInfo(j, rcsDirectory)
{
    // Check if asset is of type material
    if(!typeIsMaterial())
        throw GltfError(getUuid() + ": asset is not of type material");

    try
    {
        loadPropertiesFromJson(j);
    }
    catch(const nlohmann::detail::out_of_range& exception)
    {
        throw GltfError(getUuid() + ": " + exception.what());
    }
}

/// Create new asset from filename
AssetMaterial::AssetMaterial(const std::string& rcsFilename)
    : AssetMaterial(readJsonFile(rcsFilename), utils::path::dirname(rcsFilename)) {}

/// Create new asset from filename
AssetMaterial::AssetMaterial(const char *cpFilename)
    : AssetMaterial(std::string(cpFilename)) {}

/// Get human-readable name of material
std::string AssetMaterial::getName() const
{
    return m_sName;
}

/// Get pointer to IOR data
const AssetMaterialIor *AssetMaterial::getIorPointer() const
{
    return m_pMaterialIor;
}

/// Flag indicating whether to use geometrical optics
bool AssetMaterial::getGeometricalOptics() const
{
    return m_bGeometricalOptics;
}

/// Flag indicating whether to include diffraction in the calculation
bool AssetMaterial::getIncludeDiffraction() const
{
    return m_bIncludeDiffraction;
}

/// Flag indicating whether to perform numerical simulation
bool AssetMaterial::getIncludeNumericalSimulation() const
{
    return m_bIncludeNumericalSimulation;
}

/// Get the material scheme (surface, subsurface or volume)
e_MaterialScheme AssetMaterial::getMaterialScheme() const
{
    return m_eMaterialScheme;
}

/// Get the material scheme as string
std::string AssetMaterial::getMaterialSchemeAsString() const
{
    switch(m_eMaterialScheme)
    {
        case MATERIAL_SCHEME_SURFACE:     return "surface";
        case MATERIAL_SCHEME_SUB_SURFACE: return "subsurface";
        case MATERIAL_SCHEME_VOLUME:      return "volume";
        default: return "";
    }
}

/// Get classification of the material, e.g., "solid-metal-aluminum"
std::string AssetMaterial::getMaterialClassification() const
{
    return m_sMaterialClassification;
}

/// Get material type (isotropy, homogeneity, magnetic)
st_MaterialType AssetMaterial::getMaterialType() const
{
    return m_stMaterialType;
}

/// Get list of applicable sensors
st_ApplicableSensors AssetMaterial::getApplicableSensors() const
{
    return m_stApplicableSensors;
}

/// Get value of Lambertian emission
Float AssetMaterial::getLambertEmission() const
{
    return m_fLambertEmission;
}

/// Get URI to glTF file with IOR (index of refraction) data
std::string AssetMaterial::getIndexOfRefractionUri() const
{
    return m_sIndexOfRefractionUri;
}

/// Get mean free path (in micrometers)
Float AssetMaterial::getMeanFreePath() const
{
    return m_fMeanFreePath;
}

/// Get density of scatterers in the volume (in micrometer^3)
Float AssetMaterial::getParticleDensity() const
{
    return m_fParticleDensity;
}

/// Get cross section (in micrometer^2) of scatterers in the volume
Float AssetMaterial::getParticleCrossSection() const
{
    return m_fParticleCrossSection;
}

/// Get URI to file with emissivity coefficient values
std::string AssetMaterial::getEmissivityCoefficientUri() const
{
    return m_sEmissivityCoefficientUri;
}

/// Get struct for subsurface interaction
st_Subsurface AssetMaterial::getSubsurface() const
{
    return m_stSubsurface;
}

/// Get URI to surface displacement glTF file
std::string AssetMaterial::getSurfaceDisplacementUri() const
{
    return m_sSurfaceDisplacementUri;
}

/// Get struct with information on surface roughness
st_SurfaceRoughness AssetMaterial::getSurfaceRoughness() const
{
    return m_stSurfaceRoughness;
}

/// Get temperature of material
Float AssetMaterial::getTemperature() const
{
    return m_fTemperature;
}

////Radar Parameters
////

/// Get effective particle area
Float AssetMaterial::getEffectiveParticleArea() const
{
    return m_fEffectiveParticleArea;
}

/// Get Relative Permittivity
Float AssetMaterial::getRelativePermittivity() const
{
    return m_fRelativePermittivity;
}

/// Get Relative Permittivity
Float AssetMaterial::getRelativePermeability() const
{
    return m_fRelativePermeability;
}

/// Get Electrical Resistivity
Float AssetMaterial::getElectricalResistivity() const
{
    return m_fElectricalResistivity;
}

////Ultrasound Parameters
////

/// Get Acoustic Impedence
Float AssetMaterial::getAcousticImpedance() const
{
    return m_fAcousticImpedance;
}

/// Get Shear Velocity
Float AssetMaterial::getShearVelocity() const
{
    return m_fShearVelocity;
}

/// Get list of coating materials
std::vector<st_CoatingMaterial> AssetMaterial::getCoatingMaterials() const
{
    return m_stCoatingMaterial;
}

/// Get list of ingredients
std::vector<st_Ingredient> AssetMaterial::getIngredients() const
{
    return m_stIngredient;
}

/// @brief Load properties from glTF material
/// @param [in] Json file location
void AssetMaterial::loadPropertiesFromJson(const nlohmann::json& j)
{
    using nlohmann::json;
    using std::string;

    const json jMaterial = j.at("materials").at(0);

    m_sName = jMaterial.at("name").get<string>();

    const json jMaterialParameters = jMaterial.at("extensions").at("OpenMaterial_material_parameters");

    // User preferences
    const json jUserPreferences = jMaterialParameters.at("user_preferences");
    m_bGeometricalOptics = jUserPreferences.at("geometrical_optics").get<bool>();

    m_bIncludeDiffraction = jUserPreferences.at("include_diffraction").get<bool>();
    m_bIncludeNumericalSimulation = jUserPreferences.at("include_numerical_simulation").get<bool>();

    string sMaterialScheme = jUserPreferences.at("material_scheme").get<string>();
    if(sMaterialScheme == "surface")
        m_eMaterialScheme = MATERIAL_SCHEME_SURFACE;
    else if(sMaterialScheme == "subsurface")
        m_eMaterialScheme = MATERIAL_SCHEME_SUB_SURFACE;
    else if(sMaterialScheme == "volume")
        m_eMaterialScheme = MATERIAL_SCHEME_VOLUME;
    else
        throw GltfError(getUuid() + ": unknown material scheme");

    m_sMaterialClassification = jUserPreferences.at("material_classification").get<string>();

    const json jMaterialType = jUserPreferences.at("material_type");
    m_stMaterialType.bIsotropic = jMaterialType.at("isotropic").get<bool>();
    m_stMaterialType.bHomogeneous = jMaterialType.at("homogeneous").get<bool>();
    m_stMaterialType.bMagnetic = jMaterialType.at("magnetic").get<bool>();

    m_fTemperature = jUserPreferences.at("temperature").get<double>();
    if(m_fTemperature < 0)
        throw GltfError(getUuid() + ": temperature must be non-negative");

    m_sSurfaceDisplacementUri = jUserPreferences.at("surface_displacement_uri").get<string>();
    
    const json jSurfaceRoughness = jUserPreferences.at("surface_roughness");
    m_stSurfaceRoughness.fSurfaceHeight = jSurfaceRoughness.at("surface_height").get<double>();
    m_stSurfaceRoughness.fSurfaceCorrelationLength = jSurfaceRoughness.at("surface_correlation_length").get<double>();
    if(m_stSurfaceRoughness.fSurfaceHeight < 0)
        throw GltfError(getUuid() + ": surface_height must be non-negative");
    if(m_stSurfaceRoughness.fSurfaceCorrelationLength < 0)
        throw GltfError(getUuid() + ": surface_correlation_length must be non-negative");

    //Coating
    const json jCoating = jUserPreferences.at("coating_materials");
    for(const json& jC : jCoating)
    {
        st_CoatingMaterial mC;
        mC.sMaterilaRef = jC.at("material_ref").get<std::string>();
	mC.fLayerThickness = jC.at("layer_thickness").get<double>();
	m_stCoatingMaterial.push_back(mC);
    }
    //Ingredients
    const json jIngredient = jUserPreferences.at("ingredients");
    for(const json& jC : jIngredient)
    {
        st_Ingredient mC;
        mC.sMaterilaRef = jC.at("material_ref").get<std::string>();
	mC.sDistributionPatternUri = jC.at("order").get<std::string>();
	m_stIngredient.push_back(mC);
    }
    // Note: coating_materials is not supported yet

    m_fLambertEmission = jUserPreferences.at("lambert_emission").get<double>();
    if(m_fLambertEmission < 0)
        throw GltfError(getUuid() + ": lambert_emission must be non-negative");

    const json jSubsurface = jUserPreferences.at("subsurface");
    m_stSubsurface.bSubsurface = jSubsurface.at("subsurface").get<bool>();
    m_stSubsurface.fSubsurfaceThickness = jSubsurface.at("subsurface_thickness").get<double>();
    if(m_stSubsurface.bSubsurface && m_stSubsurface.fSubsurfaceThickness < 0)
        throw GltfError(getUuid() + ": subsurface_thickness must be non-negative");

    // Note: ingredients is not supported yet

    // Physical properties
    const json jPhysicalProperties = jMaterialParameters.at("physical_properties");

    m_sIndexOfRefractionUri = jPhysicalProperties.at("refractive_index_uri").get<string>();
    if(m_sIndexOfRefractionUri.length() > 0)
    {
        const string csFilename = getDirectory() + m_sIndexOfRefractionUri;
        m_pMaterialIor = new AssetMaterialIor(csFilename);
    }

    m_fMeanFreePath = jPhysicalProperties.at("mean_free_path").get<double>();
    if(m_fMeanFreePath < 0)
        throw GltfError(getUuid() + ": mean_free_path must be non-negative");

    m_fParticleDensity = jPhysicalProperties.at("particle_density").get<double>();
    if(m_fParticleDensity < 0)
        throw GltfError(getUuid() + ": particle_density must be non-negative");

    m_fParticleCrossSection = jPhysicalProperties.at("particle_cross_section").get<double>();
    if(m_fParticleCrossSection < 0)
        throw GltfError(getUuid() + ": particle_cross_section must be non-negative");

    m_sEmissivityCoefficientUri = jPhysicalProperties.at("emissive_coefficient_uri").get<string>();

    const json jApplicableSensors = jPhysicalProperties.at("applicable_sensors");
    for(const json& jSensor : jApplicableSensors)
    {
        const string sSensor = jSensor.get<string>();
        if(sSensor == "camera")
            m_stApplicableSensors.bCamera = true;
        else if(sSensor == "lidar")
            m_stApplicableSensors.bLidar = true;
        else if(sSensor == "radar")
            m_stApplicableSensors.bRadar = true;
        else if(sSensor == "ultrasound")
            m_stApplicableSensors.bUltrasound = true;
        else
            throw GltfError(getUuid() + ": unknown sensor type " + sSensor);
    }

    //Radar
    m_fEffectiveParticleArea = jPhysicalProperties.at("effective_particle_area").get<Float>();
    m_fRelativePermittivity = jPhysicalProperties.at("relative_permittivity").get<Float>();
    m_fRelativePermeability = jPhysicalProperties.at("relative_permeability").get<Float>();
    m_fElectricalResistivity = jPhysicalProperties.at("electrical_resistivity").get<Float>();

    //Ultrasound
    m_fAcousticImpedance = jPhysicalProperties.at("acoustic_impedance").get<Float>();
    m_fShearVelocity = jPhysicalProperties.at("shear_velocity").get<Float>();
}

/// Overload << operator
std::ostream &operator<<(std::ostream &os, const AssetMaterial &rcmAssetMaterial)
{
    using std::endl;

    const st_MaterialType type = rcmAssetMaterial.getMaterialType();
    const st_SurfaceRoughness roughness = rcmAssetMaterial.getSurfaceRoughness();
    const st_Subsurface subsurface = rcmAssetMaterial.getSubsurface();
    const st_ApplicableSensors applicableSensors = rcmAssetMaterial.getApplicableSensors();

    // Lambda function to convert bool to string
    auto toBoolean = [](bool b)
    {
        if(b)
            return "true";
        else
            return "false";
    };

    os << rcmAssetMaterial.getName() << ":" << endl
       << "    user_preferences:" << endl
       << "        geometricalOptics: " << toBoolean(rcmAssetMaterial.getGeometricalOptics()) << endl
       << "        includeDiffraction: " << toBoolean(rcmAssetMaterial.getIncludeDiffraction()) << endl
       << "        includeNumericalSimulation: " << toBoolean(rcmAssetMaterial.getIncludeNumericalSimulation()) << endl
       << "        materialScheme: " << rcmAssetMaterial.getMaterialSchemeAsString() << endl
       << "        materialClassification: \"" << rcmAssetMaterial.getMaterialClassification() << "\"" << endl
       << "        materialType:" << endl
       << "            isotropic: " << toBoolean(type.bIsotropic) << endl
       << "            homogeneous: " << toBoolean(type.bHomogeneous) << endl
       << "            magnetic: " << toBoolean(type.bMagnetic) << endl
       << "        temperature: " << rcmAssetMaterial.getTemperature() << endl
       << "        surfaceDisplacementUri: \"" << rcmAssetMaterial.getSurfaceDisplacementUri() << "\"" << endl
       << "        surfaceRoughness:" << endl
       << "            surfaceHeight: " << roughness.fSurfaceHeight << endl
       << "            surfaceCorrelationLength: " << roughness.fSurfaceCorrelationLength << endl
       << "        lambertEmission: " << rcmAssetMaterial.getLambertEmission() << endl
       << "        subsurface:" << endl
       << "            subsurface: " << toBoolean(subsurface.bSubsurface) << endl
       << "            subsurfaceThickness: " << subsurface.fSubsurfaceThickness << endl
       << "    physical_properties:" << endl
       << "        refractiveIndexUri: \"" << rcmAssetMaterial.getIndexOfRefractionUri() << "\"" << endl
       << "        meanFreePath: " << rcmAssetMaterial.getMeanFreePath() << endl
       << "        particleDensity: " << rcmAssetMaterial.getParticleDensity() << endl
       << "        particleCrossSection: " << rcmAssetMaterial.getParticleCrossSection() << endl
       << "        emissivityCoefficientUri: \"" << rcmAssetMaterial.getEmissivityCoefficientUri() << "\"" << endl
       << "        applicableSensors:" << endl
       << "            camera: " << toBoolean(applicableSensors.bCamera) << endl
       << "            lidar: " << toBoolean(applicableSensors.bLidar) << endl
       << "            radar: " << toBoolean(applicableSensors.bRadar) << endl;

    return os;
}