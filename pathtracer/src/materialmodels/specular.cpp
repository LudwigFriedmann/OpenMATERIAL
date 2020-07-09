//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      materialmodels/specular.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-06-19
/// @brief     Specular material model

#include <cmath>
#include <complex>

#include "defines.h"
#include "ray.h"
#include "intersect.h"
#include "materialmodel.h"
#include "materialmodels/specular.h"
#include "materialmodels/common.h"

/// @brief Create new specular material model
///
/// This material model only support specular reflection, ignoring polarization.
MaterialModelSpecular::MaterialModelSpecular()
{
}

/// @brief Create bounced rays
///
/// For the incident ray given by incidentRay generate bounced rays. Bounced
/// rays might correspond to reflected or transmitted rays. The information
/// about the intersection is given in rIntersection.
///
/// At the moment only perfectly specular materials are supported. Polished
/// metals, i.e., metals without roughness, are an example for such materials.
/// As a consequence, the behaviour is mirror-like (specular reflection).
///
/// @param [in] incidentRay incident ray
/// @param [in] crIntersection information on the intersection
/// @param [in] rSensor reference to sensor object
/// @param [in] rRenderer reference for renderer object
/// @retval bounced_vector vector of bounced arrays
std::vector<Ray> MaterialModelSpecular::bounce(const Ray& incidentRay, const Intersection& crIntersection, Sensor& rSensor, Renderer &rRenderer)
{
    // Yet unused variables
    (void)rRenderer;
    (void)rSensor;

    // Vector for bounced rays
    std::vector<Ray> bouncedRays;

    // Incident vector
    const Vector3 vIncident = incidentRay.getDirection().normalize();

    // Oriented normal to the surface triangle
    Vector3 vNormal = crIntersection.getNormal();

    // Cosine of incident angle between (angle between -vIncident and vNormal)
    Float fCosIncidenceAngle = -vIncident.dot(vNormal);

    // The cosine of the incidence angle can be > 1 because of finite floating
    // point precision. In this case assume the incidence angle is 90°.
    if(fCosIncidenceAngle > 1)
        fCosIncidenceAngle = 1;
    // This might happen due to Phong shading (linear interpolation of the
    // normals). In this case, fall back to flat normal.
    else if(fCosIncidenceAngle < 0)
    {
        // Use flat normal
        Vector3 vNormalFlat = crIntersection.getFlatNormal();

        fCosIncidenceAngle = -vIncident.dot(vNormalFlat);
        if(fCosIncidenceAngle < 0)
        {
	   if (m_verbose_mat) {
                std::cout<<std::endl;
                std::cout<<"-----------------------------------"<<std::endl;
                std::cerr << "fCosIncidenceAngle is negative. Correction for Phong-Shading effect or flipped Normal is enabled. Please debug. First value is:" << fCosIncidenceAngle << std::endl;
                std::cout<<"-----------------------------------"<<std::endl;
                this->disableVerboseMat();
            }
            if (m_applyFlippedNormalCorrection){
                vNormal=-vNormal;
            }
            else{
            return {};
            }
        }
      else{vNormal=-vNormal;}
    }

    // Reflected vector
    const Vector3 vReflected = (vIncident-2*vNormal.dot(vIncident)*vNormal).normalize();

    // Intersection point
    const Vector3 vPoint = crIntersection.getIntersectionPoint();

    // Compute the reflected ray
    Ray reflectedRay = incidentRay.bounce(vPoint, vReflected, crIntersection.getSeparation());

    const AssetMaterial *pMaterial = crIntersection.getMaterial();
    const AssetMaterialIor *pIor = pMaterial->getIorPointer();

    // Temperature of the asset
    const Float fTemperature = pMaterial->getTemperature();

    SpectrumRGB *pSpectrum = incidentRay.getDataRGBUnpolarized();
    reflectedRay.setDataRGBUnpolarized(pSpectrum);

    // For each wavelength in the spectrum
    for(std::size_t i = 0; i < pSpectrum->size(); i++)
    {
        Float n, k, fTermP, fTermS;
        auto elem = (*pSpectrum)[i];
        Float& fWavelength = elem.first;
        Float& fRadiance   = elem.second;

        pIor->getIor(fTemperature, fWavelength, n, k);
        Complex fN(n,k);

        // Assume unpolarized light
        fresnelReflection(fN, fCosIncidenceAngle, fTermP, fTermS);
        fRadiance = fRadiance * (fTermS+fTermP)/2;
    }

    bouncedRays.push_back(reflectedRay);

    return bouncedRays;
}

