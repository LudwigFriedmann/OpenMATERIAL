//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      spectrum.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Spectrum class

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <utility>

#include "defines.h"
#include "exceptions.h"

/// Wavelength in meters of red light
#define WAVELENGTH_RED   650e-9

/// Wavelength in meters of green light
#define WAVELENGTH_GREEN 510e-9

/// Wavelength in meters of blue light
#define WAVELENGTH_BLUE  440e-9

const Float pfRGBWavelengths[3] = {
    (Float)WAVELENGTH_RED, (Float)WAVELENGTH_GREEN, (Float)WAVELENGTH_BLUE
};

/// @brief This class is used to calculate and return the wavelenght of scattered rays, whether the object or background is hit 
class SpectrumRGB
{
private:
    /// RGB light spectrum (red, green, blue)
    Float m_pfRGB[3];

public:
    /// @brief Create a new SpectrumRGB object
    ///
    /// Create a new RGB spectrum. An RGB spectrum consists of one value at
    /// red (wavelength 650nm), green (wavelength 510nm), and blue (wavelength
    /// 440nm). Typically, the value corresponds to radiance.
    ///
    /// @param [in] fRed value for red
    /// @param [in] fGreen value for green
    /// @param [in] fBlue value for blue
    SpectrumRGB(Float fRed, Float fGreen, Float fBlue)
    {
        m_pfRGB[0] = fRed;
        m_pfRGB[1] = fGreen;
        m_pfRGB[2] = fBlue;
    }

    /// Create a copy of a SpectrumRGB object
    SpectrumRGB(const SpectrumRGB &crSpectrum)
    {
        m_pfRGB[0] = crSpectrum.m_pfRGB[0];
        m_pfRGB[1] = crSpectrum.m_pfRGB[1];
        m_pfRGB[2] = crSpectrum.m_pfRGB[2];
    }

    /// Multiply RGB spectrum to RGB values (red, green, blue)
    void multiply(Float fRed, Float fGreen, Float fBlue)
    {
        m_pfRGB[0] *= fRed;
        m_pfRGB[1] *= fGreen;
        m_pfRGB[2] *= fBlue;
    }

    /// Convert spectrum to RGB values (red, green, blue)
    void toRGB(Float& fRed, Float& fGreen, Float& fBlue) const
    {
        fRed   = m_pfRGB[0];
        fGreen = m_pfRGB[1];
        fBlue  = m_pfRGB[2];
    }

    /// Return elements of spectrum
    std::size_t size() const { return sizeof(m_pfRGB)/sizeof(Float); }

    /// Overload [] operator and return wavelength,value pair
    std::pair<Float,Float&> operator[](int i)
    {
        return std::pair<Float,Float&>(pfRGBWavelengths[i], m_pfRGB[i]);
    }
};

#endif // SPECTRUM_H

