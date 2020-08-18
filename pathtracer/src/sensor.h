//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      sensor.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-28
/// @brief     Abstract definition of the interface of a sensor

#ifndef SENSOR_H
#define SENSOR_H

#include <vector>

#include "intersect.h"
#include "uuid.h"
#include "ray.h"
#include "transformation.h"
#include "vector.h"

class Renderer;
class MaterialModel;

/// Abstract definition of Sensor class
class Sensor
{
protected:
    /// Name of sensor
    std::string m_name = "";

    /// UUID of sensor
    const Uuid m_uuid = Uuid();

    /// Position of the sensor
    Vector3 m_pos;

    /// Direction the sensor is looking at
    Vector3 m_dir;

    /// Vector that the sensor considers as "up"
    Vector3 m_up;

    /// Camera to world transformation
    Transformation m_transformCameraToWorld = Transformation::Identity();

public:
    /// Create uninitialized sensor object
    Sensor();

    /// Create new sensor object
    ///
    /// The name will be given by crsName. The sensor is located at pos, looking
    /// in the direction dir, and the sensor considers the vector up as "up".
    /// Resolution is given by wres, hres.
    ///
    /// @param [in] pos position of sensor
    /// @param [in] dir direction the sensor is looking at
    /// @param [in] up vector that the sensor considers as "up"
    Sensor(const Vector3& pos, const Vector3& dir, const Vector3& up);


    /// @brief Return vector of primary rays
    ///
    /// The renderer will call this method until the returned vector is empty
    /// (has zero elements). The renderer will report back all primary rays
    /// (call \ref reportPrimaryRay) before calling this method again.
    ///
    /// @retval rvPrimaryRays vector of primary rays
    virtual std::vector<Ray> getPrimaryRays() = 0;


    /// @brief Method called after all rays from getPrimaryRays have been processed
    ///
    /// This method will be called after all rays from getPrimaryRays() have
    /// been processed and before getPrimaryRays() is called again. While most
    /// sensors will not need to implement this method, it provides a simple way
    /// to free memory allocated in getPrimaryRays().
    ///
    /// @param [in] vRays vector of primary rays originially returned from getPrimaryRays()
    virtual void deletePrimaryRays(const std::vector<Ray> &) {}

    /// @brief Report the results for the primary rays
    ///
    /// This method will be called by the renderer and hands back a bounced ray
    /// for a primary ray. If there are multiple bounced ray for one primary
    /// ray, this method will be called several times. The ray rBouncedRay
    /// contains the information what the corresponding primary "sees". For
    /// example, in the case of light the information are the RGB values of the
    /// radiance. The sensor can then use this information to create the sensor
    /// impression.
    ///
    /// @param [in] rBouncedRay bounced ray
    virtual void reportPrimaryRay(Ray& rBouncedRay) = 0;

    /// @brief Report the result for one primary ray
    ///
    /// This method is a wrapper around \ref reportPrimaryRays in the case
    /// there is only one ray.
    ///
    /// @param [in] rvBouncedRays vector of bounced ray
    virtual void reportPrimaryRays(std::vector<Ray>& rvBouncedRays)
    {
        for(Ray& ray : rvBouncedRays)
            reportPrimaryRay(ray);
    }

    /// @brief Check is sensor is compatible with material model
    ///
    /// @param [in] crMaterialModel material model
    /// @retval true if material model and sensor are compatible
    /// @retval false if material model and sensor are incompatible
    virtual bool isCompatible(const MaterialModel& crMaterialModel) = 0;

    /// @brief Return flag indicating if the sensor should be called continuously
    ///
    /// If this method calls true, the method \ref Renderer::trace will call the
    /// method \ref hook. While most sensors will not use this capibility,
    /// sensors like the RCS sensor need it to determine if the aperture was
    /// hit. Otherwise, the sensor is not able to get such information.
    ///
    /// @retval true if trace should call hook
    /// @retval false otherwise
    virtual bool callSensor() const { return false; }

    /// @brief Method called by \ref Renderer::trace
    ///
    /// If \ref callSensor returns true, Renderer::trace will call this method.
    /// The information if the ray hit geometry can be retrieved calling
    /// crIntersection.hit().
    ///
    /// @param [in] crRenderer reference to the renderer
    /// @param [in] crIncidentRay incident ray
    /// @param [in] crIntersection information about intersection point if hit is true
    /// @retval true if renderer should continue
    /// @retval false if renderer should not continue
    virtual bool hook(const Renderer& crRenderer, Ray& crIncidentRay, Intersection& crIntersection)
    {
        // suppress warnings about unused variables
        (void)crRenderer;
        (void)crIncidentRay;
        (void)crIntersection;

        return true;
    }

    /// Get the UUID of the sensor
    const Uuid &getUuid() const;

    /// Set resolution of the sensor to (wres, hres)
    //void setResolution(unsigned int wres, unsigned int hres);

    /// Set the name of the sensor to crsName
    void setName(const std::string &crsName);

    /// Get the resolution of the sensor
    //void getResolution(unsigned int& wres, unsigned int& hres) const;

    /// Get the position of the sensor
    virtual const Vector3& getPosition() const;

    /// Get the direction the sensor is looking at
    virtual const Vector3& getDirection() const;

    /// Get the vector that the sensor is considering as "up"
    virtual const Vector3& getUpwards() const;
};

#endif // SENSOR_H