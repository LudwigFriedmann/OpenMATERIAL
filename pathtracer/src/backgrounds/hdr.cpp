//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      backgrounds/hdr.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-17
/// @brief     Support for HDR backgrounds

#include <cassert>
#include <algorithm>

#include "backgrounds/hdr.h"
#include "defines.h"
#include "doctest.h"
#include "exceptions.h"

/// @brief Unit test for BackgroundHDR::BackgroundHDR 
TEST_CASE("Testing BackgroundHDR::BackgroundHDR")
{
	std::string fileName = "../hdr/green_point_park_4k.hdr";
	#ifdef _WIN32
		std::replace(fileName.begin(), fileName.end(), '/', '\\');
	#endif
	
	// Check for errors thrown if hdr isn't loaded
	BackgroundHDR background(fileName); 
	CHECK(true);
}

/// @brief Create background using HDR file
///
/// @param [in] rsHdr path to HDR file
BackgroundHDR::BackgroundHDR(const std::string &rsHdr) : m_iXres(0), m_iYres(0)
{
    int n;
    m_pfHdr = stbi_loadf(rsHdr.c_str(), &m_iXres, &m_iYres, &n, 0);
    if(m_pfHdr == nullptr)
        throw OSError("Couldn't read HDR file " + rsHdr);
}

///  @brief Free allocated memory for HDR background
BackgroundHDR::~BackgroundHDR()
{
    free(m_pfHdr);
}

///  @brief Set correct RGB values of incidentRay
///  @param [in] incidentRay which has not scattered
void BackgroundHDR::hit(Ray& incidentRay)
{
    Float r,theta,phi;
    m_M.apply(incidentRay.getDirection()).toSpherical(r,theta,phi);

    // Convert to pixel values
    const int x = static_cast<int>(std::round((phi+M_PI)/(2*M_PI)*(m_iXres-1)));
    const int y = static_cast<int>(std::round((theta)/M_PI*(m_iYres-1)));

    assert(x >= 0 && x < m_iXres);
    assert(y >= 0 && y < m_iYres);

    const Float fRed   = m_pfHdr[3*(y*m_iXres+x)+0];
    const Float fGreen = m_pfHdr[3*(y*m_iXres+x)+1];
    const Float fBlue  = m_pfHdr[3*(y*m_iXres+x)+2];

    SpectrumRGB *pSpectrum = incidentRay.getDataRGBUnpolarized();
    pSpectrum->multiply(fRed, fGreen, fBlue);
}