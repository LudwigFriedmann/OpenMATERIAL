//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      interpolation.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-08-02
/// @brief     Support for interpolation

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <algorithm>
#include <utility>
#include <vector>

#include "exceptions.h"

/// Available types for interpolation
enum e_interpolationType {
    /// Use left point for interpolation
    INTERPOLATION_STEP,
    /// Use closest point for interpolation
    INTERPOLATION_NEAREST,
    /// Use linear interpolation using left and right point
    INTERPOLATION_LINEAR
};

/// @brief Interpolation of one-dimensional functions
///
/// The class allows interpolation of a function y=f(x) from data points
/// (x_j,y_j).
template <class T1, class T2>
class Interpolation
{
private:
    /// Vector of data points (x,y)
    std::vector<std::pair<T1,T2>> m_data;

    /// Type of interpolation; by default linear
    e_interpolationType m_interpolationType = INTERPOLATION_LINEAR;

    /// Internal function for interpolation
    T2 _get(const T1 x) const
    {
        int indexLeft = 0, indexRight = size()-1;

        // Find left and right index using binary search
        while((indexRight-indexLeft) > 1)
        {
            const int indexMiddle = (indexLeft+indexRight)/2;

            if(x < m_data[indexMiddle].first)
                indexRight = indexMiddle;
            else
                indexLeft = indexMiddle;
        }

        if(m_interpolationType == INTERPOLATION_LINEAR)
        {
            T1 delta_x = m_data[indexRight].first-m_data[indexLeft].first;
            T2 delta_y = m_data[indexRight].second-m_data[indexLeft].second;

            return m_data[indexLeft].second + (x-m_data[indexLeft].first)*delta_y/delta_x;
        }
        else if(m_interpolationType == INTERPOLATION_STEP)
            return m_data[indexLeft].second;
        else if(m_interpolationType == INTERPOLATION_NEAREST)
        {
            if((m_data[indexRight].first-x) < (x-m_data[indexLeft].first))
                return m_data[indexRight].second;
            else
                return m_data[indexLeft].second;
        }
        
        // If we reach here, the interpolation type has not been implemented
        throw NotImplementedError("Interpolation type not implemented");
    }

public:
    /// Create a new interpolation object
    Interpolation() = default;

    /// Set type of interpolation to interpolationType
    ///
    /// @param [in] interpolationType type of interpolation
    void setInterpolationType(e_interpolationType interpolationType)
    {
        m_interpolationType = interpolationType;
    }

    /// @brief Add point (x,y).
    ///
    /// Note that you have to call sort() before you can use get().
    ///
    /// @param [in] x value of x
    /// @param [in] y value of y
    void addPoint(const T1 x, const T2 y)
    {
        m_data.push_back(std::make_pair(x,y));
    }

    /// @brief Add several points
    ///
    /// The x values are given by x, the y elements are given by y. This method
    /// will add elems of points.
    ///
    /// Note that you have to call sort() before you can use get().
    ///
    /// @param [in] x array of x values
    /// @param [in] y corresponding array with y values
    /// @param [in] uElems number of (x,y) pairs
    void addPoints(const T1 *x, const T2 *y, std::size_t uElems)
    {
        m_data.reserve(uElems);
        for(std::size_t i = 0; i < uElems; i++)
            m_data.push_back(std::make_pair(x[i],y[i]));
    }

    /// @brief Add several points
    ///
    /// The x values are given by the vector x, the y values are given by the
    /// vector y. If the vectors x and y do not have the same length, N points
    /// will be added where N is the length of the vector with less elements.
    ///
    /// Note that you have to call sort() before you can use get().
    ///
    /// @param [in] x vector of x values
    /// @param [in] y corresponding vector with y values
    void addPoints(const std::vector<T1> x, const std::vector<T2> y)
    {
        addPoints(x.data(), y.data(), std::min(x.size(), y.size()));
    }

    /// Returns the number of points
    ///
    /// @retval numberOfPoints number of points
    size_t size() const { return m_data.size(); }

    /// @brief Sort the data points
    ///
    /// This method must be called before you can access the data.
    void sort()
    {
        std::sort(m_data.begin(), m_data.end());
    }

    /// Return minimum x value
    ///
    /// @retval xmin minimum value of x
    T1 xMin() const
    {
        return m_data.front().first;
    }

    /// Return maximum x value
    ///
    /// @retval xmax maximum value of x
    T1 xMax() const
    {
        return m_data.back().first;
    }

    /// @brief Get interpolated value at x
    ///
    /// If x is outside of the interpolation domain a ValueError is thrown.
    ///
    /// @param [in] x value of x
    /// @retval y interpolated value y=f(x)
    T2 get(const T1 x) const
    {
        if(m_data.size() == 0)
            throw ValueError("No points have been added");
        if(m_data.size() == 1 && m_interpolationType == INTERPOLATION_LINEAR)
            throw ValueError("At least two points needed for linear interpolation");
        if(x < m_data.front().first)
            throw ValueError("Value is below interpolation range");
        if(x > m_data.back().first)
            throw ValueError("Value is above interpolation range");

        return _get(x);
    }

    /// Return true if x is in the interpolation domain, otherwise return false
    ///
    /// @param [in] x value of x
    /// @retval true if x is in the interpolation domain
    /// @retval false if x is not in the interpolation domain
    bool contains(const T1 x) const
    {
        return !(m_data.size() == 0 || x < m_data.front().first || x > m_data.back().first);
    }

    /// @brief Get interpolated value at x
    ///
    /// If x is outside the inteprolation domain, notFoundItem will be
    /// returned.
    ///
    /// @param [in] x value of x
    /// @param [in] notFoundItem item that should be returned if x is not in the interpolation domain
    /// @retval y interplated value of f(x) or notFoundItem
    T2 get(const T1 x, const T2 notFoundItem) const
    {
        if(m_data.size() == 0 || x < m_data.front().first || x > m_data.back().first)
            return notFoundItem;

        return _get(x);
    }
};

#endif // INTERPOLATION_H