//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      backgrounds/fatal.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Background class that throws an exception

#ifndef BACKGROUND_FATAL_H
#define BACKGROUND_FATAL_H

#include "background.h"
#include "exceptions.h"
#include "ray.h"
#include "spectrum.h"

/// @brief Background class that throws an exception
class BackgroundFatal : public Background
{
public:
    /// Create new background object
    BackgroundFatal() = default;

    /// Throw a RuntimeError
    void hit(Ray& incidentRay) override
    {
        (void)incidentRay;
        throw RuntimeError("Hit background for ray id " + std::to_string(incidentRay.getId()));
    }
};

#endif // BACKGROUND_FATAL_H