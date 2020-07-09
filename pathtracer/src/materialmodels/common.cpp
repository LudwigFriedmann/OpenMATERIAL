//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      materialmodels/common.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-10
/// @brief     Common functions for material classes

#include <cmath>
#include <complex>

#include "common.h"
#include "defines.h"
#include "doctest.h"
#include "vector.h"

using std::complex;

/// @brief Unit test for material::fresnel
TEST_CASE("Testing material::fresnel")
{
    Float fTermP = 0, fTermS = 0;
    Float n = 1.5;
    Complex fN(n,0); // Air to glass

    // Check normal incidence
    fresnelReflection(fN, 0, fTermP, fTermS);
    CHECK(fTermP == fTermS);
    CHECK(std::abs(fTermP - pow((n-1)/(n+1),2)) < 1.0f);
}

/// @brief Reflection at a plane surface
///
/// Compute the reflectance for p-polarized waves
/// \f[
///     R_p = \left| \frac{\left(\frac{n_2}{n_1}\right)^2\cos\theta_i - \sqrt{\left(\frac{n_2}{n_1}\right)^2-\sin^2\theta_i}}{\left(\frac{n_2}{n_1}\right)^2\cos\theta_i + \sqrt{\left(\frac{n_2}{n_1}\right)^2-\sin^2\theta_i}} \right|^2,
/// \f]
/// and for s-polarized waves
/// \f[
///     R_s = \left| \frac{\cos\theta_i - \sqrt{\left(\frac{n_2}{n_1}\right)^2-\sin^2\theta_i}}{\cos\theta_i + \sqrt{\left(\frac{n_2}{n_1}\right)^2-\sin^2\theta_i}} \right|^2
/// \f]
/// respectively.
///
/// References:
///   - J. D. Jackson, Classical Electrodynamics
///   - Wikipedia: https://en.wikipedia.org/wiki/Fresnel_equations
///
/// @param [in] fN \f$n_2/n_1\f$: IOR_second_medium/IOR_first_medium
/// @param [in] fCosTheta cosine of incident angle
/// @param [out] fTermP reflectance for p-polarized light
/// @param [out] fTermS reflectance for s-polarized light
void fresnelReflection(Complex fN, Float fCosTheta, Float &fTermP, Float &fTermS)
{
    Complex fN2 = fN*fN;
    Float fCos2Theta = fCosTheta*fCosTheta;
    Float fSin2Theta = 1-fCos2Theta;

    // Fresnel coefficient for p-polarization
    Complex fNumerator_p   = fN2*fCosTheta - sqrt(fN2 - fSin2Theta);
    Complex fDenominator_p = fN2*fCosTheta + sqrt(fN2 - fSin2Theta);
    Complex ratio_p = fNumerator_p / fDenominator_p;
    fTermP = real(ratio_p * conj(ratio_p));

    // Fresnel coefficient for s-polarization
    Complex fNumerator_s   = fCosTheta - sqrt(fN2 - fSin2Theta);
    Complex fDenominator_s = fCosTheta + sqrt(fN2 - fSin2Theta);
    Complex ratio_s = fNumerator_s/fDenominator_s;
    fTermS = real(ratio_s * conj(ratio_s));
}

/// @brief Compute reflection at a mirror
///
/// Compute the reflection of an incident vector at a plane.
///
/// @param [in] vIncident incident direction
/// @param [in] vNormal normal of plane
/// @retval vReflected direction of reflected ray
Vector3 reflect(const Vector3& vIncident, const Vector3& vNormal)
{
    return (vIncident-2*vNormal.dot(vIncident)*vNormal).normalize();
}

