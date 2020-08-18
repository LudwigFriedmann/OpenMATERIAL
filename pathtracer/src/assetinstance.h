//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      assetinstance.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Support for instancing

#ifndef ASSETINSTANCE_H
#define ASSETINSTANCE_H

#include "assetgeometry.h"
#include "defines.h"
#include "constants.h"
#include "ray.h"
#include "transformation.h"
#include "uuid.h"

/// @brief Support for instances
///
/// Support instancing of AssetGeometry assets.
class AssetInstance
{
private:
    /// Transformation from local to world coordinates
    Transformation m_transformation;

    /// Pointer to AssetGeometry object
    AssetGeometry *m_pAssetGeometry;

    /// Unique id of this instance
    Uuid m_uuid;

public:
    /// Create new instance as pAssetGeometry
    ///
    /// @param [in] pAssetGeometry pointer to AssetGeometry object
    /// @param [in] transformation transformation
    AssetInstance(AssetGeometry *pAssetGeometry, const Transformation& transformation)
    {
        m_transformation = transformation;
        m_pAssetGeometry = pAssetGeometry;
    }

    /// Return pointer to AssetGeometry object
    AssetGeometry *getAssetGeometry() const
    {
        return m_pAssetGeometry;
    }

    /// Return transformation
    Transformation getTransformation() const
    {
        return m_transformation;
    }

    /// Get unique id of instance
    Uuid getUuid() const { return m_uuid; }

    /// @brief Intersect ray with this instance
    ///
    /// Intersect ray with this instance. The distance t from the origin of the
    /// ray to the hit point must satisfy tmin <= 0 <= tmax. If the ray
    /// intersects a triangle of a mesh and tmin <= 0 <= tmax, true is returned
    /// and rIntersection contains information on the hit point.
    ///
    /// Please note that this method overwrites tmax. If the ray intersects geometry,
    /// tmax equals t. If not, tmax should be ignored.
    ///
    /// rIntersection needs to be initialized with the incidentRay in world
    /// coordinates.
    ///
    /// @param [in] incidentRay ray
    /// @param [out] rIntersection reference to intersection object
    /// @param [in] tmin minimum value of t (see text description)
    /// @param [in,out] tmax maximum value of t (see text description)
    bool intersectRay(const Ray& incidentRay, Intersection& rIntersection, Float tmin=0, Float tmax=fInfinity) const
    {
        const Ray rayLocal = m_transformation.transformRayToLocal(incidentRay);
        bool hit = m_pAssetGeometry->intersectRay(rayLocal, rIntersection, tmin, tmax);
        if(hit)
        {
            Transformation transformation = m_transformation.apply(rIntersection.getTransformation());
            rIntersection.setTransformation(transformation);
        }

        return hit;
    }
};

#endif // ASSETINSTANCE_H