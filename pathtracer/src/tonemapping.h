//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      tonemapping.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-28
/// @brief     implementation of a very simple tone mapping operator

#ifndef TONEMAPPING_H
#define TONEMAPPING_H

#include "defines.h"

/// @brief Simple tone mapping operator. Its function is to map HDR image into LDR,
/// reducing the info according to a user defined parameter.

class ToneMapping
{
private:
    /// parameter for tone mapping operator
    Float m_fAlpha=1;

public:
    /// Create empty tone mapping object
    ToneMapping() {}

    /// @brief Simple tone mapping operator
    ///
    /// This operator maps every value to the interval [0,1) using the formula
    ///     f(x) = x/(alpha+x)
    /// where alpha is a user defined parameter.
    ///
    /// This operator allows to map a HDR image to a LDR image.
    ///
    /// @param [in] fAlpha parameter alpha
    ToneMapping(Float fAlpha=1)
    {
        m_fAlpha = fAlpha;
    }

    /// overload function call
    Float operator()(Float fIn) const
    {
       return fIn/(m_fAlpha+fIn);
    }
};

#endif // TONEMAPPING_H

