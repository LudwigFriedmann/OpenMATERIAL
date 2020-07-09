//
//Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      sensor.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-03
/// @brief     Generic sensor class

#include "sensor.h"
#include "matrix.h"
#include "transformation.h"

/// Create not initialized sensor object
Sensor::Sensor() {}

/// @brief Create new sensor object
///
/// Each sensor has a name, a position, a direction it is looking at, a vector
/// that the sensor considers as "up", and a resolution each for width and
/// height.
///
/// @param [in] pos position of the sensor
/// @param [in] dir direction the sensor is looking at
/// @param [in] up vector that the sensor considers as "up"
Sensor::Sensor(const Vector3 &pos, const Vector3 &dir, const Vector3 &up)
    : m_pos(pos), m_dir(dir), m_up(up)
{
    Matrix3x3 M = Matrix3x3((-m_dir.cross(m_up)),m_up,-m_dir);
    Vector3 o = pos;

    m_transformCameraToWorld = Transformation(M,o);
}

/// Set name of sensor to crsName
void Sensor::setName(const std::string &crsName)
{
    m_name = crsName;
}

/// Get position of sensor
const Vector3 &Sensor::getPosition() const
{
    return m_pos;
}

/// Get direction sensor is looking at
const Vector3 &Sensor::getDirection() const
{
    return m_dir;
}

/// Get vector that the sensor considers as "up"
const Vector3 &Sensor::getUpwards() const
{
    return m_up;
}

/// Get UUID of sensor
const Uuid &Sensor::getUuid() const
{
    return m_uuid;
}
