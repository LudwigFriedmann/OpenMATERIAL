//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      bbox.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-28
/// @brief     Axis-aligned minimum bounding boxes

#ifndef BBOX_H
#define BBOX_H

#include <cassert>

#include "constants.h"
#include "defines.h"
#include "ray.h"
#include "vector.h"

/// @brief Class for axis-aligned minimum bounding boxes
class BBox
{
private:
    /// Axis-aligned minimum bounding box: m_pp saves the two edges of the
    /// bounding box. m_pp[0] is the minimum point, m_pp[1] is the maximum point
    /// such that m_pp[0] < m_pp[1] (the comparison is meant to be interpreted
    /// component-wise)
    Vector3 m_pp[2];

public:
    /// Create an unitialized bbox object
    BBox() {
        m_pp[0] = Vector3(0,0,0);
        m_pp[1] = Vector3(0,0,0);
    }

    /// @brief Create bounding box
    ///
    /// Create an axis-aligned boundary box with the two edges given by vMin
    /// and vMax.
    ///
    /// Requirement: vMin[j] <= vMax[j] for j=0,1,2
    ///
    /// @param [in] vMin edge of bounding box (minimum)
    /// @param [in] vMax edge of bounding box (maximum)
    BBox(const Vector3& vMin, const Vector3& vMax)
    {
        // check that vMin_j <= vMax_j for j=0,1,2.
        assert(vMin[0] <= vMax[0]);
        assert(vMin[1] <= vMax[1]);
        assert(vMin[2] <= vMax[2]);

        m_pp[0] = vMin;
        m_pp[1] = vMax;
    }

    /// @brief Create new bounding box from vertices
    ///
    /// Create new bounding box object from vertices. The vertex buffer
    /// rvVertexBuffer is a C++ vector containing 3*N Float elements where N is
    /// the number of vertices and three consecutive Floats correspond to a
    /// vertex.
    ///
    /// @param [in] rvVertexBuffer reference to vertex buffer
    explicit BBox(const std::vector<Float>& rvVertexBuffer)
    {
        Float xmin = fInfinity, xmax=-fInfinity;
        Float ymin = fInfinity, ymax=-fInfinity;
        Float zmin = fInfinity, zmax=-fInfinity;

        const Float *pVertexBuffer = rvVertexBuffer.data();

        for(std::size_t i = 0; i < rvVertexBuffer.size()/3; i++)
        {
            Float x = pVertexBuffer[3*i+0];
            xmin = std::min(x,xmin);
            xmax = std::max(x,xmax);

            Float y = pVertexBuffer[3*i+1];
            ymin = std::min(y,ymin);
            ymax = std::max(y,ymax);

            Float z = pVertexBuffer[3*i+2];
            zmin = std::min(z,zmin);
            zmax = std::max(z,zmax);
        }

        m_pp[0] = Vector3(xmin,ymin,zmin);
        m_pp[1] = Vector3(xmax,ymax,zmax);
    }

    /// Return bounding box surrounding the bounding box with bbox2
    BBox surrround(const BBox& bbox2) const
    {
        Float afMin[3], afMax[3];

        for(int j = 0; j < 3; j++)
        {
            Float min1_j = m_pp[0][j], min2_j = bbox2.m_pp[0][j];
            Float max1_j = m_pp[1][j], max2_j = bbox2.m_pp[1][j];

            afMin[j] = min1_j < min2_j ? min1_j : min2_j;
            afMax[j] = max1_j > max2_j ? max1_j : max2_j;
        }

        return BBox(Vector3(afMin), Vector3(afMax));
    }

    /// Copy bounding box object
    BBox(const BBox& bbox)
    {
        m_pp[0] = bbox.m_pp[0];
        m_pp[1] = bbox.m_pp[1];
    }

    /// Return minimum edge of bounding box
    const Vector3& getMin() const { return m_pp[0]; }

    /// Return maximum edge of bounding box
    const Vector3& getMax() const { return m_pp[1]; }

    /// Ray box intersection
    ///
    /// Return true if the ray hits the bounding box and the distance t
    /// satisfies tmin <= t <= tmax.
    ///
    /// @param [in] ray ray to test
    /// @param [in] tmin minimum value of t
    /// @param [in] tmax maximum value of t
    /// @retval true if ray hits box and tmin <= t <= tmax
    /// @retval false otherwise
    bool intersectRay(const Ray &ray, Float tmin=0, Float tmax=fInfinity) const
    {
        Float interval_min = tmin, interval_max = tmax;

        for(int j = 0; j < 3; j++)
        {
            int posneg = ray.m_posneg[j];
            Float t0 = (m_pp[  posneg][j] - ray.m_vOrigin[j])*ray.m_vInvDirection[j];
            Float t1 = (m_pp[1-posneg][j] - ray.m_vOrigin[j])*ray.m_vInvDirection[j];
            if(t0 > interval_min)
                interval_min = t0;
            if(t1 < interval_max)
                interval_max = t1;
            if(interval_min > interval_max)
                return false;
        }

        return true;
    }

    /// Check if bounding boxes are overlapping
    bool overlap(const BBox &bbox2) const
    {
        if(m_pp[1][0] < bbox2.m_pp[0][0])
        {
            // if we are here, then x^(1)_max < x^(2)_min
            return m_pp[1][1] < bbox2.m_pp[0][1] && m_pp[1][2] < bbox2.m_pp[0][2];
        }
        else
        {
            // if we are here, then x^(1)_max > x^(2)_min
            return m_pp[1][1] > bbox2.m_pp[0][1] && m_pp[1][2] > bbox2.m_pp[0][2];
        }
    }
};

#endif // BBOX_H