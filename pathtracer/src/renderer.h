//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG) 
//

/// @file      renderer.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-06-19
/// @brief     Rendering

#ifndef RENDERER_H
#define RENDERER_H

#include <omp.h>

class Sensor;

#include "assetinstance.h"
#include "background.h"
#include "materialmodel.h"

/// @brief Renderer class. It combines the sensor and material part through the assetInstance class.
//The primary rays are generated, traced and bounced until they hit the background. 

class Renderer
{
private:
    /// Pointer to instance
    const AssetInstance *m_cpAssetInstance = nullptr;

    /// Pointer to material model object
    MaterialModel *m_pMaterialModel = nullptr;

    /// Pointer to background
    Background *m_pBackground = nullptr;

    /// Lock for omp
    omp_lock_t m_ompWritelock;

    /// Flag indicating whether to output debugging information while rendering
    bool m_bVerbose = false;

    /// Threshold on number of bouncings allowed
    int m_thbounces = 50;

    /// Threshold on min. Radiance for reflected ray
    float m_minradiance = 0.1; //Initial il 1.0

    void trace(Sensor& crSensor, Ray &incidentRay);

    /// Raytracer mode
    int m_raytracer = 0;

public:
    Renderer(const AssetInstance *cpAssetInstance, MaterialModel *pMaterialModel, Background *pBackground = nullptr);
    void setVerbose(bool bVerbose);

    const Background *getBackground() const;
    const MaterialModel *getMaterialModel() const;
    const AssetInstance *getAssetInstance() const;
    /// Set function to stop the ray tracing after a given number of bounces
    void setNumberofAllowedBounces(int a){m_thbounces=a;}
    /// Set function to stop the ray tracing when a minimum radiance level is reached
    void setThresholdOnRadiance(float a){m_minradiance=a;}
    /// Set function to enable raycaster mode in pathtracer
    void setRaytracer(int a){m_raytracer=a;}

    void render(Sensor& rSensor);
};

#endif // RENDERER_H
