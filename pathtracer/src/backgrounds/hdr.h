//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      backgrounds/hdr.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-17
/// @brief     HDR background

#ifndef BACKGROUND_HDR_H
#define BACKGROUND_HDR_H

#include <string>

#include <cstdlib>
#include "stb_image.h"

#include "background.h"
#include "constants.h" // definition of M_PI
#include "matrix.h"
#include "ray.h"
#include "spectrum.h"

/// @brief Support for HDR backgrounds
class BackgroundHDR : public Background
{
private:
    /// Pointer to HDR buffer
    float *m_pfHdr = nullptr;

    /// Transformation matrix for the HDR background
    Matrix3x3 m_M = Matrix3x3::RotationX(M_PI/2);

    /// Width resolution of HDR image (pixels in x-direction)
    int m_iXres;

    /// Height resolution of HDR image (pixels in y-direction)
    int m_iYres;

public:
    explicit BackgroundHDR(const std::string &rsHdr);
    ~BackgroundHDR();

    void hit(Ray& incidentRay) override;
};

#endif // BACKGROUND_HDR_H