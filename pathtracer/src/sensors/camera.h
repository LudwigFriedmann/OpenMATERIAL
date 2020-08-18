//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      sensors/camera.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-25
/// @brief     Camera class implementing thin lens and pinhole camera

#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include "defines.h"
#include "materialmodel.h"
#include "ray.h"
#include "sensor.h"
#include "tonemapping.h"
#include "transformation.h"

/// @brief Thin lens camera model
///
/// The classs implements the thin lens camera model as described in pbrt [1].
/// If the radius of the lens is zero, the thin lens camera becomes a pinhole
/// camera.
///
/// References:
///  - [1] Physically Based Rendering, chapter 6.2.3
class Camera: public Sensor
{
private:
    // Camera parameters

    /// Width resolution
    unsigned int m_uWres = 500;

    /// Height resolution
    unsigned int m_uHres = 500;

    /// Focal length
    Float m_fFocalLength = 0.5;

    /// Lens Radius
    Float m_fLensRadius = 0;

    /// Focal distance (zf)
    Float m_fFocalDist = 4;

    /// Aspect ratio (width/height)
    Float m_fAspectRatio = 1;

    /// Vertical Field of view [Deg]
    Float m_fYFieldOfView = 120;

    /// Number of samples
    int m_iSamples = 1;

    /// Buffer that holds the image data w_mres*m_hres*(r,g,b)
    Float *m_pfBuffer = nullptr;

    /// Flag indicating all primary rays have been generated
    bool m_bDone = false;

public:
    Camera(const Vector3 &pos, const Vector3 &dir, const Vector3 &up, unsigned int wres, unsigned int hres);

    virtual ~Camera();

    void setFocalLength(Float fFocalLength);
    Float getFocalLength() const;

    void setLensRadius(Float fLensRadius);
    Float getLensRadius() const;

    void setNumberOfSamples(int iNumberOfSamples);
    int getNumberOfSamples() const;

    void setFocalDistance(Float fFocalDistance);
    Float getFocalDistance() const;

    Float getYFieldOfView() const;
    void setYFieldOfView(Float fYFielfOfView);

    void setAspectRatio(Float m_fAspectRatio);
    Float getAspectRatio() const;

    void setHres(unsigned int iHres);
    unsigned int getHres() const;

    void setWres(unsigned int iWres);
    unsigned int getWres() const;

    std::vector<Ray> getPrimaryRays() override;
    void reportPrimaryRay(Ray& rvBouncedRay) override;
    virtual void save(const std::string &rsFilename, const ToneMapping &toneMapping);
    virtual void saveRaycaster(const std::string &rsFilename);
    bool isCompatible(const MaterialModel& crMaterialModel) override;
};

#endif // CAMERA_H