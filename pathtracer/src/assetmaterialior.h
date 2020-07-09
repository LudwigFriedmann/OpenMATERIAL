//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetmaterialior.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-08-02
/// @brief     Data for index of refraction

#ifndef ASSETMATERIALIOR_H
#define ASSETMATERIALIOR_H

#include <vector>

#include "assetinfo.h"
#include "defines.h"
#include "exceptions.h"
#include "interpolation.h"
#include "uuid.h"

/// @brief Support for material IOR assets
///
/// This class allows access to the index of refraction assets.
class AssetMaterialIor : public AssetInfo
{
private:
    /// Internal struct to save data
    typedef struct {
        /// Temperature in Kelvin
        Float fTemperature = 0;
        /// Interpolation object for real part n of index of refraction
        Interpolation<Float,Float> interpolationN;
        /// Interpolation object for imaginary part k of index of refraction
        Interpolation<Float,Float> interpolationK;
        /// Smallest wavelength in meters for which data for n,k is available
        Float fIorMin = 0;
        /// Largest wavelength in meters for which data for n,k is available
        Float fIorMax = 0;
        /// Data for Lorentz oscillator model
        std::vector<Float> vLoData;
        /// Minimum value for which Lorentz oscillator model is applicable
        Float fLOMin = 0;
        /// Maximum value for which Lorentz oscillator model is applicable
        Float fLOMax = 0;
    } ior_data_t;

    /// Comparison function to sort the vector elements by first element of tuples
    static bool lessthan(ior_data_t& a, ior_data_t& b)
    {
        return (a.fTemperature < b.fTemperature);
    }

    /// Vector of IOR data for different temperatures
    std::vector<ior_data_t> m_iorData;

    void loadPropertiesFromJson(const nlohmann::json& j);

    /// Find index of the temperature closest to fTemp
    std::size_t find_closest(Float fTemp) const;

public:
    AssetMaterialIor(const nlohmann::json &j, const std::string rcsDirectory="");
    AssetMaterialIor(const std::string rcsFilename);
    AssetMaterialIor(const char * psFilename);

    Float getIor(Float fTemp, Float wl, Float &n, Float &k) const;
    Float getDomain(Float fTemp, Float &wlMin, Float &wlMax) const;
};

#endif // ASSETMATERIALIOR_H

