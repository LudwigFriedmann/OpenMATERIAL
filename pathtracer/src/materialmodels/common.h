//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      materialmodels/common.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-10
/// @brief     Common function for Material classes

#ifndef MATERIAL_MODELS_COMMON_H
#define MATERIAL_MODELS_COMMON_H

#include <complex>

#include "defines.h"
#include "vector.h"

/// @brief Class to Compute reflection at a plane surface
/// Compute the reflectance for p-polarized waves and for s-polarized waves
/// References:
///   - J. D. Jackson, Classical Electrodynamics
///   - Wikipedia: https://en.wikipedia.org/wiki/Fresnel_equations
void fresnelReflection(Complex fN, Float fCosTheta, Float& fTermP, Float& fTermS);

Vector3 reflect(const Vector3& vIncident, const Vector3& vNormal);

#endif // MATERIAL_MODELS_COMMON_H

