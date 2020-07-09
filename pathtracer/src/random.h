//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      random.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-08-30
/// @brief     Support for creating random numbers

#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>
#include <random>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

namespace Random
{
    uint64_t getSeed();

    std::mt19937 *getMersenneGenerator();
    std::mt19937_64 *getMersenne64Generator();

    uint64_t getRandomU64();

    /// @brief Return a random real number between min and max
    ///
    /// The generated random number is in the interval [min,max).
    ///
    /// This is a tiny wrapper around std::uniform_real_distribution.
    ///
    /// @param [in] min minimum value
    /// @param [in] max maximum value
    /// @retval r random real number
    template<class T>
    T uniformRealDistribution(T min, T max)
    {
        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(*(getMersenne64Generator()));
    }

    /// @brief Return an integer number between min and max
    ///
    /// The generated random number is in the closed interval [min,max],
    /// i.e., [min,min+1,...,max-1,max].
    ///
    /// This is a tiny wrapper around std::uniform_int_distribution.
    ///
    /// @param [in] min minimum value
    /// @param [in] max maximum value
    /// @retval r random integer
    template<class T>
    T uniformIntDistribution(T min, T max)
    {
        std::uniform_int_distribution<T> distribution(min, max);
        return distribution(*(getMersenne64Generator()));
    }

    /// @brief Return a real number according to a normal distribution
    ///
    /// The variable mean and stddev correspond to the mean value and the
    /// standard deviation of the normal distribution, respectively.
    ///
    /// This is a tiny wrapper around std::normal_distribution.
    ///
    /// @param [in] mean mean value of normal distribution
    /// @param [in] stddev standard deviation of normal distribution
    /// @retval r random number
    template<class T>
    T normalDistribution(T mean, T stddev)
    {
        std::normal_distribution<T> distribution(mean, stddev);
        return distribution(*(getMersenne64Generator()));
    }

    /// @brief Random point inside a two dimensional disk
    ///
    /// Generate a random point inside the unit disk and write the x-coordinate
    /// to the variable x, and the y-coordinate to the variable y.
    ///
    /// See function ConcentricSampleDisk in pbrt (pbrt-book.org, 13.6.2
    /// Sampling a Unit Disk).
    ///
    /// @param [out] x x-coordinate of random point inside unit disk
    /// @param [out] y y-coordinate of random point inside unit disk
    template<class T>
    void sampleConcentricDisk(T& x, T& y)
    {
        T ux = uniformRealDistribution<T>(-1,1);
        T uy = uniformRealDistribution<T>(-1,1);

        if(ux == 0 && uy == 0)
        {
            x = y = 0;
            return;
        }

        T r,theta;
        if(std::abs(x) > std::abs(y))
        {
            r = ux;
            theta = (M_PI/4)*uy/ux;
        }
        else
        {
            r = uy;
            theta = (M_PI/4)*ux/uy;
        }

        x = r*std::cos(theta);
        y = r*std::sin(theta);
    }
}

#endif // RANDOM_H

