//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      materialmodels/specular.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-06-19
/// @brief     Material model

#ifndef MATERIAL_MODEL_SPECULAR_H
#define MATERIAL_MODEL_SPECULAR_H

#include <complex>
#include <vector>

#include "materialmodel.h"
#include "ray.h"
#include "renderer.h"
#include "intersect.h"

/// Specular material model
class MaterialModelSpecular : public MaterialModel
{
private:
    /// Allow extra-verbosity for debugging purpose
    bool m_verbose_mat = true;
    /// Allow correction of normals which are not pointing on the ray direction
    bool m_applyFlippedNormalCorrection = true;

 public:
    MaterialModelSpecular();

    std::vector<Ray> bounce(const Ray& incidentRay, const Intersection& crIntersection, Sensor& rSensor, Renderer &rRenderer);
    /// Function to set verbosity
    void disableVerboseMat(){m_verbose_mat=false;}
};

#endif // MATERIAL_MODEL_SPECULAR_H

