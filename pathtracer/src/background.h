//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      background.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Background class

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "exceptions.h"
#include "ray.h"
#include "spectrum.h"

/// @brief Background class
///
/// For each ray during rendering in \ref Renderer::trace there are two
/// options: Either the ray hits geometry or the ray does not hit geometry.
///
/// In the case that the ray hits geometry, the method \ref MaterialModel::bounce
/// is called which generates zero, one or more bounced rays. Bounced rays are
/// either reflected rays (this is the most common case) or refracted rays.
/// (Refracted rays are rays that travel inside the medium, for example rays
/// inside of glass.)
///
/// If the ray does not hit geometry, the renderer calls the method \ref hit
/// of a Background object if a background object is given. If no background is
/// given to the renderer, the renderer just ignores the ray. Unlike the bounce
/// method, the method \ref hit does not generate new rays but it modifies the
/// properties of the incident ray. For the HDR example, the hit method will
/// adjust the red, green, and blue color value of the incident ray.
/// In other words, the background is a light source, therefore it modifies the
/// energy (e.g., the RGB values) of the ray, but it does not generate new rays.
class Background
{
public:
    /// Create new background object
    Background() = default;

    /// Modify the incidentRay.
    virtual void hit(Ray& incidentRay) = 0;
};

#endif // BACKGROUND_H