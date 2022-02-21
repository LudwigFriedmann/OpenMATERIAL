//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetmaterial.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-08-01
/// @brief     Material properties and methods to load from glTF model

#ifndef ASSETMATERIAL_H
#define ASSETMATERIAL_H

#include <vector>

#include "assetinfo.h"
#include "assetmaterialior.h"
#include "defines.h"
#include "exceptions.h"
#include "uuid.h"

class AssetMaterial;

/// @brief Material schemes
enum e_MaterialScheme
{
    /// Surface interaction
    MATERIAL_SCHEME_SURFACE = 0,
    /// Sub-surface interaction
    MATERIAL_SCHEME_SUB_SURFACE,
    /// Volume interaction
    MATERIAL_SCHEME_VOLUME,
};

/// @brief List of sensors that are applicable
struct st_ApplicableSensors
{
    /// Material is applicable for camera sensor (visible light)
    bool bCamera = false;

    /// Material is applicable for LIDAR sensor
    bool bLidar = false;

    /// Material is applicable for RADAR sensor
    bool bRadar = false;

    /// Material is applicable for ultrasound sensor
    bool bUltrasound = false;
};

/// @brief Type of material
struct st_MaterialType
{
    /// True if material is isotropic, false if anisotropic
    bool bIsotropic = true;

    /// True if material is homogeneous, false if nonhomogeneous
    bool bHomogeneous = true;

    /// True if material is magnetic, false if nonmagnetic
    bool bMagnetic = false;
};

/// @brief Struct for subsurface interaction
struct st_Subsurface
{
    /// If true, subsurface interactions will be considered in addition to
    /// surface interactions
    bool bSubsurface = false;

    /// Thickness of volume to be considered as subsurface. The default value
    /// is the penetration depth which will be calculated by material model.
    Float fSubsurfaceThickness = 0.0;
};

/// @brief Coating refers to a layer of transparent or semi-transparent material on top of another material
/// (layer of oil or water on top of asphalt)
struct st_CoatingMaterial
{
    /// Reference to external material (uri)
    std::string sMaterialRef;

    /// Pointer to external material with coating
    AssetMaterial *pCoatingMaterial = nullptr;

    /// Thickness of the coating layer in micrometer
    Float fLayerThickness;
};

///@brief Ingredients are considered as impurities on top of the main material (oxidization might be an ingredient of a metal)
struct st_Ingredient
{
    /// Reference to external material (uri)
    std::string sMaterialRef;

    /// Pointer to external material with coating
    AssetMaterial *pMaterialIngredient = nullptr;

    /// Link to external map of material distribution.
    /// Describes how material and the ingredients are distributed over the geometry.
    std::string sDistributionPatternUri;
};

/// @brief Struct for roughness parameters
struct st_SurfaceRoughness
{
    /// Surface height root-mean-square in micrometer
    Float fSurfaceHeight = 0;

    /// Surface correlation length in micrometer
    Float fSurfaceCorrelationLength = 0;
};

/// @brief Support for material assets
///
/// The class AssetMaterial is a container for all parameters of an asset of
/// type material. The class is a data container and allows reading material
/// parameters from the glTF extension OpenMaterial_material_parameters, and the
/// access to the parameters.
class AssetMaterial : public AssetInfo
{
public:
    AssetMaterial();
    explicit AssetMaterial(const nlohmann::json &j, const std::string& rcsDirectory="");
    explicit AssetMaterial(const std::string& rcsFilename);
    explicit AssetMaterial(const char *cpFilename);
    ~AssetMaterial() override;

    std::string getName() const;
    bool getGeometricalOptics() const;
    bool getIncludeDiffraction() const;
    bool getIncludeNumericalSimulation() const;
    e_MaterialScheme getMaterialScheme() const;
    std::string getMaterialSchemeAsString() const;
    std::string getMaterialClassification() const;
    st_MaterialType getMaterialType() const;
    st_ApplicableSensors getApplicableSensors() const;
    Float getLambertEmission() const;
    std::string getIndexOfRefractionUri() const;
    Float getMeanFreePath() const;
    Float getParticleDensity() const;
    Float getParticleCrossSection() const;
    std::string getEmissivityCoefficientUri() const;
    Float getTemperature() const;
    st_Subsurface getSubsurface() const;
    std::string getSurfaceDisplacementUri() const;
    st_SurfaceRoughness getSurfaceRoughness() const;
    std::vector<st_CoatingMaterial> getCoatingMaterials() const;
    std::vector<st_Ingredient> getIngredients() const;
    const AssetMaterialIor *getIorPointer() const;

    //Radar
    Float getEffectiveParticleArea() const;
    std::string getRelativePermittivityUri() const;
    std::string getRelativePermeabilityUri() const;
    std::string getConductivityUri() const;

    //Ultrasound
    Float getAcousticImpedance() const;
    Float getShearVelocity() const;
    
private:
    void loadPropertiesFromJson(const nlohmann::json& j);

    /// Flag indicating whether to use geometrical optics approximation
    bool m_bGeometricalOptics = true;

    /// Flag indicating whether to include diffraction computations
    bool m_bIncludeDiffraction = false;

    /// Flag indicating whether to use numerical methods like BEM, FEM, FDTD, or FMM
    bool m_bIncludeNumericalSimulation = false;

    /// Material scheme (surface, subsurface or volume)
    e_MaterialScheme m_eMaterialScheme = MATERIAL_SCHEME_SURFACE;

    /// Classification of the material, e.g., "solid-metal-aluminum"
    std::string m_sMaterialClassification = "unknown";

    /// Material type (isotropy, homogeneity, magnetic)
    st_MaterialType m_stMaterialType;

    /// List of applicable sensors
    st_ApplicableSensors m_stApplicableSensors;

    /// Material is a Lambertian emitter
    Float m_fLambertEmission = 0;

    /// URI to glTF file with IOR (index of refraction) data
    std::string m_sIndexOfRefractionUri;

    /// Mean free path in micrometer (for volumetric materials)
    Float m_fMeanFreePath = 0;

    /// Density of scatterers in the volume (in micrometer^3)
    Float m_fParticleDensity = 0;

    /// Effective cross section (in micrometer^2) of scatterers in the volume
    Float m_fParticleCrossSection = 0;

    /// URI to file with emissivity coefficient values
    std::string m_sEmissivityCoefficientUri;

    /// Temperature of the asset in Kelvin
    Float m_fTemperature = 300;

    /// Struct for subsurface interaction
    st_Subsurface m_stSubsurface;

    /// URI to an external file with displacement data
    std::string m_sSurfaceDisplacementUri;

    /// List of coating materials
    std::vector<st_CoatingMaterial> stCoatingMaterial;

    /// List of ingredients
    std::vector<st_Ingredient> stIngredient;
    
    /// Struct with roughness parameters
    st_SurfaceRoughness m_stSurfaceRoughness;

    /// List of coating materials
    //std::vector<ReferenceLink> m_stCoatingMaterials;

    /// List of ingredients
    //std::vector<ReferenceLink> m_stIngredients;

    /// Name of material
    std::string m_sName;

    /// Coating material
    std::vector<st_CoatingMaterial> m_stCoatingMaterial;

    /// Material ingredients
    std::vector<st_Ingredient> m_stIngredient;

    //Radar Part
    /// Effective particle area of a material.
    Float m_fEffectiveParticleArea = 0;
    /// Ratio of the capacitance of a capacitor using that material as a dielectric, compared with a similar capacitor that has vacuum as its dielectric. URI to an external file with permittivity data.
    std::string m_sRelativePermittivityUri;
    /// Ratio of the permeability of a specific medium to the permeability of free space. URI to an external file with permeability data.
    std::string m_sRelativePermeabilityUri;
    /// It quantifies how the material conducts electric current.
    std::string m_sConductivityUri;

    //Ultrasound Part
    /// It is a physical property of tissue. It describes how much resistance an ultrasound beam encounters as it passes through a tissue (in kg/(m2s))
    Float m_fAcousticImpedance = 0;
    /// Shear velocity is used to describe shear-related motion in moving fluids.
    Float m_fShearVelocity = 0;

    /// Pointer to IOR (index of refraction) data
    AssetMaterialIor *m_pMaterialIor = nullptr;

    friend std::ostream& operator<<(std::ostream &os, const AssetMaterial &rcAssetMaterial);
};

#endif // ASSETMATERIAL_H