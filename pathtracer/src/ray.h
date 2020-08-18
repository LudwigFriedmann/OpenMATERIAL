//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      ray.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-28
/// @brief     Support for rays

#ifndef RAY_H
#define RAY_H

#include <vector>

#include "defines.h"
#include "exceptions.h"
#include "spectrum.h"
#include "vector.h"

class RayTube;

enum e_rayType {
    /// Plain ray, m_pType is nullptr
    RAY_PLAIN,
    /// Unpolarized light, m_pType is of type RayTypePolarized *
    RAY_RGB_UNPOLARIZED,
    /// Polarized light, m_pType is of type RayTypePolarized *
    RAY_RGB_POLARIZED,
    /// Polarized, raytube, m_pType is of type RayTypeTubePolarized *
    RAY_POLARIZED_TUBE
};

/// @brief Ray class
///
/// A ray is an infinite line. Points on the ray are represented as
///      P(t) = O + t*D
/// where O is the origin, the vector D represents the direction of the ray, and
/// t parametrizes points on the ray. t corresponds to the distance from the
/// origin.
///
/// Rays also have ids. However, these ids are different from the uuids of
/// assets. A ray is not considered an asset and the ray id is only used by the
/// sensor to match primaryRays and bouncedRays.
///
/// Additional properties can be set using the setData* methods. These methods
/// allow to save pointers to additional data. The corresponding objects can be
/// retrieved using the getData* methods. Different ray types include polarized
/// and unpolarized RGB light, and ray tubes. The methods getType() returns the
/// corresponding type of the ray.
class Ray
{
protected:
    /// Id of ray
    std::size_t m_uId = 0;

    /// Number of bounces
    int m_iBounces = 0;

    /// Origin of ray
    Vector3 m_vOrigin;

    /// Direction of the ray
    Vector3 m_vDirection;

    /// 1/direction of the ray (used in class BBox)
    Vector3 m_vInvDirection;

    /// Optical path length (in vacuum this coresponds to the distance)
    Float m_fOpticalPathLength = 0;

    /// 1 if corresponding component of m_v3fDirection is negative, otherwise 0 (used in class BBox)
    int m_posneg[3]{};

    /// Type of ray
    e_rayType m_eType = RAY_PLAIN;

    /// Pointer to a class with extra properties
    void *m_pType = nullptr;

    ///Ray intensity
    float m_fIntensity{};

public:
    /// Create uninitalized Ray object
    Ray() = default;

    /// @brief Create a new ray
    ///
    /// @param [in] uId id of ray
    /// @param [in] crvOrigin origin of ray
    /// @param [in] crvDirection direction of ray
    Ray(std::size_t uId, const Vector3& crvOrigin, const Vector3& crvDirection)
    {
        m_uId = uId;
        setOrigin(crvOrigin);
        setDirection(crvDirection);
    }

    /// Return id of ray
    std::size_t getId() const { return m_uId; }

    /// Set origin to crvOrigin
    void setOrigin(const Vector3 &crvOrigin)
    {
        m_vOrigin = crvOrigin;
    }

    /// Set direction to crvDirection
    void setDirection(const Vector3 &crvDirection)
    {
        m_vDirection = crvDirection;
        m_vInvDirection = Vector3(1/crvDirection[0], 1/crvDirection[1], 1/crvDirection[2]);
        for(int j = 0; j < 3; j++)
            m_posneg[j] = crvDirection[j] >= 0 ? 0 : 1;
    }

    /// Set optical path length to fOpticalPathLength
    void setOpticalPathLength(Float fOpticalPathLength)
    {
        m_fOpticalPathLength = fOpticalPathLength;
    }

    /// Get optical path length
    Float getOpticalPathLength() const
    {
        return m_fOpticalPathLength;
    }

    /// Increment optical path length by fDeltaOpticalPathLength
    void addToOpticalPathLength(float fDeltaOpticalPathLength)
    {
        m_fOpticalPathLength += fDeltaOpticalPathLength;
    }

    /// Set intensity
    void setIntensity(const float &ray_intensity)
    {
        m_fIntensity=ray_intensity;
    }


    /// Get intensity
    float getIntensity() const
    {
        return m_fIntensity;
    }


    /// Create a copy of ray
    Ray(const Ray& ray)
    {
        m_uId = ray.m_uId;
        m_vOrigin = ray.m_vOrigin;
        m_vDirection = ray.m_vDirection;
        m_vInvDirection = ray.m_vInvDirection;
        m_iBounces = ray.m_iBounces;
        m_fOpticalPathLength = ray.m_fOpticalPathLength;
        m_eType = ray.m_eType;
        m_pType = ray.m_pType;

        // copy m_posneg
        for(std::size_t j = 0; j < sizeof(m_posneg)/sizeof(m_posneg[0]); j++)
            m_posneg[j] = ray.m_posneg[j];
    }

    /// @brief Create a bounced ray
    ///
    /// The origin of the ray is given by rv3fOrigin, the direction is given by
    /// rv3fDirection. The numberOfBounces of the returned ray is one higher
    /// than of the current ray.
    ///
    /// @param [in] rv3fOrigin origin of ray
    /// @param [in] rv3fDirection direction of ray
    Ray bounce(const Vector3 &rv3fOrigin, const Vector3 &rv3fDirection, Float fDeltaOpticalPathLength) const
    {
        Ray bouncedRay(m_uId, rv3fOrigin, rv3fDirection);
        bouncedRay.m_iBounces = m_iBounces+1;
        bouncedRay.m_fOpticalPathLength = m_fOpticalPathLength+fDeltaOpticalPathLength;
        return bouncedRay;
    }

    /// Get number of bounces
    int getBounces() const
    {
        return m_iBounces;
    }

    /// Get direction of ray
    const Vector3 &getDirection() const { return m_vDirection; }

    /// Get inverse direction (1/direction_x, 1/direction_y, 1/direction_z)
    const Vector3 &getInvDirection() const { return m_vInvDirection; }

    /// Get origin of ray
    const Vector3 &getOrigin() const { return m_vOrigin; }

    /// Return point P(t)=origin+t*direction at position t
    Vector3 at(Float t) const
    {
        return m_vOrigin+t*m_vDirection;
    }

    /// Get number of bounces
    int getNumberOfBounces() const { return m_iBounces; }

    /// Set number of bounces to iBounces
    void setNumberOfBounces(int iBounces) { m_iBounces = iBounces; }

    /// Increase the number of bounces by one
    void increaseNumberOfBounces() { m_iBounces++; }

    /// Get the type of the ray
    e_rayType getType() const
    {
        return m_eType;
    }

    /// Get pointer to SpectrumRGB data
    SpectrumRGB *getDataRGBUnpolarized() const
    {
        if(m_eType != RAY_RGB_UNPOLARIZED)
            throw RuntimeError("Ray is not of type RAY_RGB_UNPOLARIZED");

        return static_cast<SpectrumRGB *>(m_pType);
    }

    /// Set pointer to SpectrumRGB data
    void setDataRGBUnpolarized(SpectrumRGB *pSpectrum)
    {
        m_eType = RAY_RGB_UNPOLARIZED;
        m_pType = pSpectrum;
    }

    /// Return pointer to RayTube object
    ///
    /// If the ray does not have a RayTube object, an exception is thrown.
    ///
    /// @retval pRayTube pointer to RayTube object
    RayTube *getDataRayTube() const
    {
        if(m_eType != RAY_POLARIZED_TUBE)
            throw RuntimeError("Ray is not of type RAY_POLARIZED_TUBE");

        return static_cast<RayTube *>(m_pType);
    }

    /// Set pointer to RayTube object
    void setDataRayTube(RayTube *pRayTube)
    {
        m_eType = RAY_POLARIZED_TUBE;
        m_pType = pRayTube;
    }

    /// Class for axis-aligned minimum bounding boxes
    friend class BBox;
};

#endif // RAY_H