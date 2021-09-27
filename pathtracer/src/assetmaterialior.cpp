//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetmaterialior.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-08-02
/// @brief     Data for index of refraction

#include "assetmaterialior.h"
#include "doctest.h"
#include "exceptions.h"
#include "interpolation.h"

#include "constants.h"

#include <complex>

using std::size_t;

/// @brief Unit test for AssetMaterialIor::AssetMaterialIor 
TEST_CASE("Testing AssetMaterialIor::AssetMaterialIor")
{
    std::string fileName = "../materials/data/aluminium_ior.gltf";
    #ifdef _WIN32
        std::replace(fileName.begin(), fileName.end(), '/', '\\');
    #endif  

    // Check if correct IOR table is loaded
    auto *pMaterialIor = new AssetMaterialIor(fileName);
    std::string title = "IOR aluminium";
    CHECK(title.compare(pMaterialIor->getTitle()) == 0);
}

/// @brief Create new IOR (index of refraction) data object from JSON
///
/// @param [in] j json object
/// @param [in] rcsDirectory path to the directory containing the glTF file
AssetMaterialIor::AssetMaterialIor(const nlohmann::json& j, const std::string &rcsDirectory) : AssetInfo(j,rcsDirectory)
{
    // check if asset is of type material
    if(!typeIsMaterialIor())
        throw GltfError(getUuid() + ": asset is not of type materialior");

    try
    {
        loadPropertiesFromJson(j);
    }
    catch(const nlohmann::detail::out_of_range& exception)
    {
        throw GltfError(getUuid() + ": " + exception.what());
    }
}

/// Create new AssetMaterialIor object from filename (std::string)
AssetMaterialIor::AssetMaterialIor(const std::string &rcsFilename)
    : AssetMaterialIor(readJsonFile(rcsFilename),utils::path::dirname(rcsFilename))
{}

/// Create new AssetMaterialIor object from filename (C string)
AssetMaterialIor::AssetMaterialIor(const char *psFilename)
    : AssetMaterialIor(std::string(psFilename))
{}

/// @brief Load from json object
///
/// @param [in] j json object
void AssetMaterialIor::loadPropertiesFromJson(const nlohmann::json& j)
{
    using nlohmann::json;
    using std::string;

    const json& jData = j.at("extensions").at("OpenMaterial_ior_data");

    // For all temperatures
    for (const auto& it : jData.at("data"))
    {
        const Float fTemperature = it.at("temperature").get<double>();

        if(fTemperature < 0)
            throw GltfError(getUuid() + ": temperature must be non-negative");

        m_iorData.emplace_back();
        ior_data_t& data = m_iorData.back();
        data.fTemperature = fTemperature;


        // Read IOR data if present
        if(it.find("n") != it.end() && it.find("k") != it.end())
        {
            data.interpolationN.setInterpolationType(INTERPOLATION_LINEAR);
            data.interpolationK.setInterpolationType(INTERPOLATION_LINEAR);

            // Read in real part n of IOR (index of refraction)
            for(const auto& v : it.at("n"))
            {
                Float wl = v.at(0).get<double>();
                Float n  = v.at(1).get<double>();

                data.interpolationN.addPoint(wl,n);
            }

            // Read in imaginary part k of IOR (index of refraction)
            for(const auto& v : it.at("k"))
            {
                Float wl = v.at(0).get<double>();
                Float k  = v.at(1).get<double>();

                data.interpolationK.addPoint(wl,k);
            }

            // Sort the interpolation objects
            data.interpolationN.sort();
            data.interpolationK.sort();

            // Set minimum and maximum wavelength
            data.fIorMin = std::max(data.interpolationN.xMin(), data.interpolationK.xMin());
            data.fIorMax = std::min(data.interpolationN.xMax(), data.interpolationK.xMax());
        }
    }

    // Sort according to temperatures
    std::sort(m_iorData.begin(), m_iorData.end(), lessthan);
}

/// Find index of the temperature closest to fTemp
size_t AssetMaterialIor::find_closest(Float fTemp) const
{
    size_t index = 0;
    Float delta = std::fabs(m_iorData.at(0).fTemperature-fTemp);

    for(size_t i = 1; i < m_iorData.size(); i++)
    {
        Float delta_i = std::fabs(m_iorData.at(i).fTemperature-fTemp);
        if(delta_i < delta)
        {
            index = i;
            delta = delta_i;
        }
    }

    return index;
}


/// @brief Get index of refrection for temperature fTemp and wavelength wl
///
/// The data for the temperature closest to fTemp is used. In this data the
/// real part n and the imaginary part k of the refractive index are obtained
/// using linear interpolation of the data points.
///
/// @param [in] fTemp desired temperature
/// @param [in] wl wavelength in meters
/// @param [out] n real part of index of refraction
/// @param [out] k imaginary part of index of refrection
/// @retval temp actual temperature of the data
Float AssetMaterialIor::getIor(Float fTemp, Float wl, Float& n, Float& k) const
{
    // Find closest temperature
    size_t index = find_closest(fTemp);

    const auto& data = m_iorData[index];

    if(data.fIorMin <= wl && wl <= data.fIorMax)
    {
        // Perform linear interpolation of optical data
        n = data.interpolationN.get(wl);
        k = data.interpolationK.get(wl);
    }
    else if(data.fLOMin <= wl && wl <= data.fLOMax)
    {
        // Use lorentz oscillator model
        const Float omega = (2*M_PI*PhysicalConstants::c)/wl;
        const Float omega2 = omega*omega;

        // Dielectric function
        Complex eps(1);
        Complex imag(0,1);

        for(size_t i = 0; i < data.vLoData.size()/3; i++)
        {
            const Float omegap2 = data.vLoData[3*i+0];
            const Float omega1  = data.vLoData[3*i+1];
            const Float gamma   = data.vLoData[3*i+2];

            eps += omegap2/(omega1-imag*omega*gamma-omega2);
        }

        const Float eps_real = eps.real();
        const Float eps_imag = eps.imag();
        const Float s = std::hypot(eps_real,eps_imag);

        // Real part of IOR
        n = std::sqrt(0.5*(s+eps_real));
        // Imaginary part of IOR
        k = std::sqrt(0.5*(s-eps_real));
    }
    else
        throw RuntimeError(getUuid() + ": no IOR available for wavelength " + std::to_string(wl) + " and temperature " + std::to_string(fTemp));

    return data.fTemperature;
}
