//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      materialmodel.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-10
/// @brief     Abstract definition of the material model interface

#ifndef MATERIALMODEL_H
#define MATERIALMODEL_H

#include <vector>

#include "intersect.h"
#include "ray.h"

// forward declaration
class MaterialModel;

class Sensor;
class Renderer;

/// Abstract definition of MaterialModel interface
class MaterialModel
{
public:
    /// Create uninitialized material object
    MaterialModel() = default;

    /// @brief Return vector of bounced rays
    ///
    /// @param [in] incidentRay the incident ray
    /// @param [in] crIntersection intersection object
    /// @param [in] rSensor reference to sensor object
    /// @param [in] rRenderer reference to renderer object
    /// @retval vBouncedRays vector of bounced rays
    virtual std::vector<Ray> bounce(const Ray& incidentRay, const Intersection& crIntersection, Sensor& rSensor, Renderer &rRenderer) = 0;
};

#endif // MATERIALMODEL_H