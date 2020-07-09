//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      constants.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-06-24
/// @brief     Define constants

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits>

#include "defines.h"

/// Mathematical constant pi
#ifndef M_PI
#define M_PI 3.141592653589793
#endif

/// Float representation of infinity
#ifndef fInfinity
#define fInfinity std::numeric_limits<Float>::infinity()
#endif

/// Physical constants
namespace PhysicalConstants
{
    /// Reduced Planck constant \f$\hbar\f$ [m² kg / s]
    static const Float hbar = 1.0545718e-34;

    /// Reduced Planck constant \f$\hbar\f$ [eV s/rad]
    static const Float hbar_eV = 6.582119514e-16;

    /// Boltzmann constant \f$k_\mathrm{B}\f$ [m² kg / ( K s² )]
    static const Float kB = 1.38064852e-23;

    /// Speed of light \f$c\f$ in vacuum [m/s]
    static const Float c = 299792458.;

    /// Elementary charge \f$e\f$ [C]
    static const Float e = 1.60217662e-19;
}

#endif // CONSTANTS_H

