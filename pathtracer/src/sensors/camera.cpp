//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      sensors/camera.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-17
/// @brief     Support for Thin Film Camera

#include <cmath>
#include <fstream>
#include <vector>

#include "doctest.h"
#include "exceptions.h"
#include "materialmodels/specular.h"
#include "matrix.h"
#include "random.h"
#include "sensors/camera.h"
#include "tonemapping.h"
#include "transformation.h"
#include "vector.h"

/// @brief Unit test for Camera::Camera 
TEST_CASE("Testing Camera::Camera")
{
	Camera camera({0,1,-4}, {0,0,1}, {0,1,0}, 1200,1200);

	// Check if primary rays were created
	std::vector<Ray> primaryRays = camera.getPrimaryRays();
	CHECK(primaryRays.size() == 1200*1200);
}

/// @brief Create new camera object
///
/// The implemented camera model is a thin lens camera as described in pbrt [1].
/// If the radius of the lens is zero, the thin lens camera becomes a pinhole
/// camera.
///
/// References:
///   [1] Physically Based Rendering, chapter 6.2.3
///
/// @param [in] pos position of camera (position of the center of the lens)
/// @param [in] dir direction in which the camera is looking
/// @param [in] up vector that the camera considers as "up"
/// @param [in] wres resolution for width
/// @param [in] hres resolution for height
Camera::Camera(const Vector3& pos, const Vector3& dir, const Vector3& up, unsigned int wres, unsigned int hres)
    : Sensor(pos, dir, up)
{
    m_uHres = hres;
    m_uWres = wres;

    // allocate memory for image buffer and initialize it with zeros
    m_pfBuffer = new Float[3*wres*hres];
    for(size_t j = 0; j < 3*wres*hres; j++)
        m_pfBuffer[j] = 0;
}

///  @brief Destructor that frees allocated memory for image buffer
Camera::~Camera()
{
    // free image buffer
    delete[] m_pfBuffer;
}

/// @brief Return if sensor is compatible with material model
///
/// @retval true if pinhole camera is compatible with crMaterialModel
/// @retval false otherwise
bool Camera::isCompatible(const MaterialModel& crMaterialModel)
{
    return typeid(crMaterialModel) == typeid(MaterialModelSpecular);
}

/// @brief Set the focal length of the camera
///
/// @param [in] fFocalLength focal length (fFocalLength>0)
void Camera::setFocalLength(Float fFocalLength)
{
    m_fFocalLength = fFocalLength;
}

/// @brief Get the focal length of the camera
///
/// @retval fFocalLength focal length
Float Camera::getFocalLength() const
{
    return m_fFocalLength;
}

/// @brief Set radius of the lens
///
/// For fLensRadius=0 the camera becomes a pinhole camera. If flensRadius=0,
/// the number of samples are set to 1 by this method, see also
/// \ref setNumberOfSamples for more information.
///
/// @param [in] fLensRadius radius of the lens (fLensRadius>=0)
void Camera::setLensRadius(Float fLensRadius)
{
    m_fLensRadius = fLensRadius;
    if(fLensRadius == 0)
        m_iSamples = 1;
}

/// @brief Get radius of the lens
///
/// @retval fLensRadius radius of lens
Float Camera::getLensRadius() const
{
    return m_fLensRadius;
}

/// @brief Set number of samples
///
/// If the radius of the lens is greater than zero, multiple initial rays per
/// pixel are needed to create a non-noisy image. The number of initial rays
/// (the number of samples per pixel) are set to iNumberOfSamples.
///
/// @param [in] iNumberOfSamples number of samples per pixel (iNumberOfSamples>0)
void Camera::setNumberOfSamples(int iNumberOfSamples)
{
    m_iSamples = iNumberOfSamples;
}

/// @brief Get number of samples
///
/// @retval iNumberOfSamples number of samples
int Camera::getNumberOfSamples() const
{
    return m_iSamples;
}

/// @brief Set focal distance
///
/// Set focal distance to fFocalDistance. Points at a separation fFocalDistance
/// from the camera are in focus.
///
/// @param [in] fFocalDistance focal distance (fFocalDistance>0)
void Camera::setFocalDistance(Float fFocalDistance)
{
    m_fFocalDist = fFocalDistance;
}

/// @brief Get focal distance
///
/// @retval fFocalDist focal distance
Float Camera::getFocalDistance() const
{
    return m_fFocalDist;
}

/// @brief Set y field of view
///
/// Set the field of view for the y-direction (up-down, vertical) to
/// yFieldOfView. fYfieldOfView is an opening angle. The value is expected
/// in degrees.
///
/// @param [in] fYFieldOfView vertical field of view in degrees (fYFieldOfFiew
void Camera::setYFieldOfView(Float fYFieldOfView)
{
    m_fYFieldOfView = fYFieldOfView;
}

/// @brief Get y field of view
///
/// @retval YFieldOfView y field of view
Float Camera::getYFieldOfView() const
{
    return m_fYFieldOfView;
}

/// @brief Set aspect ratio
///
/// Set the aspect ratio (width/height) to fAspectRatio.
///
/// @param [in] fAspectRatio aspect ratio (fAspectRaio>0)
void Camera::setAspectRatio(Float fAspectRatio)
{
    m_fAspectRatio = fAspectRatio;
}

/// @brief Get aspect ratio width/height
///
/// @retval fAspectRatio aspect ratio width/height
Float Camera::getAspectRatio() const
{
    return m_fAspectRatio;
}

/// @brief Set height resolution
///
/// @param [in] uHres height resolution (uHres>0)
void Camera::setHres(unsigned int uHres)
{
    m_uHres = uHres;
}

/// @brief Get height resolution
///
/// @retval uHres height resolution
unsigned int Camera::getHres() const
{
    return m_uHres;
}

/// @brief Set width resolution
///
/// @param [in] uWres height resolution (uWres>0)
void Camera::setWres(unsigned int uWres)
{
    m_uWres = uWres;
}

/// @brief Get width resolution
///
/// @retval uWres width resolution
unsigned int Camera::getWres() const
{
    return m_uWres;
}

/// @brief Return vector of primary rays
///
/// Append the primary rays to the vector rvPrimaryRays. The vector
/// rvPrimaryRays may be empty or already contain entries. The method is
/// only appends primary rays to rvPrimaryRays.
///
/// Once this method returns false, there are no more primary rays.
///
/// @retval primaryRays vector of primary rays
std::vector<Ray> Camera::getPrimaryRays()
{
    std::vector<Ray> primaryRays;
    if(m_bDone)
        return primaryRays;

    // Local coordinates of camera
    const Vector3 pos_c(0,0,0);  // camera position
    const Vector3 dir_c(0,0,-1); // camera direction
    const Vector3 up_c(0,1,0);   // camera up vector
    const Vector3 right_c = dir_c.cross(up_c).normalize(); // (1,0,0)

    // Derived parameters
    const Float zf_prime = (m_fFocalLength*m_fFocalDist)/(m_fFocalLength+m_fFocalDist);

    // Height of the film
    const Float alpha = m_fYFieldOfView*M_PI/180; // FOV in radians
    const Float height = 2*zf_prime*std::tan(alpha/2);

    // Width
    const Float width = height*m_fAspectRatio;

    // Bottom left corner of the film
    const Vector3 origin(-width/2,-height/2,m_fFocalLength);

    // Step in x direction
    const Vector3 dw = right_c*width*(1.f/ static_cast<float>(m_uWres-1));

    // Step in y direction
    const Vector3 dh = up_c*height*(1.f/static_cast<float>(m_uHres-1));

    // Reserve space in the vector
    primaryRays.reserve(m_uWres*m_uHres*static_cast<size_t>(m_iSamples));

    for(size_t j = 0; j < m_uHres; j++)
    {
        for(size_t i = 0; i < m_uWres; i++)
        {
            // Ray id
            const size_t id = j*m_uWres+i;

            // Point on the film
            const Vector3 pFilm = origin+i*dw+dh*j;

            // Ray in camera space
            Ray ray_local = Ray(id, pos_c, (pos_c-pFilm).normalize());

            // Ray in world coordinates
            Ray ray_world;

            for(int k = 0; k < m_iSamples; k++)
            {
                if(m_fLensRadius == 0)
                {
                    // Pinhole camera
                    ray_world = m_transformCameraToWorld.transformRayToWorld(ray_local);
                }
                else
                {
                    // Random numbers between [0,1]
                    auto rx = Random::uniformRealDistribution<Float>(0,1);
                    auto ry = Random::uniformRealDistribution<Float>(0,1);
                    Vector3 r(rx,ry,0.0);

                    // Sample point on lens
                    Random::sampleConcentricDisk<Float>(rx,ry);
                    const Vector3 pLens = r*m_fLensRadius;

                    // Point on plane of focus
                    Float ft = m_fFocalDist/-ray_local.getDirection().get(2);
                    const Vector3 pFocal = ray_local.at(ft);

                    // Ray in camera space
                    ray_local = Ray(id, pLens,(pFocal-pLens).normalize());

                    // Transformation of the Ray from camera space to world space
                    ray_world = m_transformCameraToWorld.transformRayToWorld(ray_local);
                }

                // SpectrumRGB object will be deleted in reportPrimaryRay
                ray_world.setDataRGBUnpolarized(new SpectrumRGB(1,1,1));
                primaryRays.push_back(ray_world);
            }
        }
    }

    m_bDone = true;

    return primaryRays;
}

/// @brief Report back results for incidentRay
///
/// This method will be called by the renderer and hands back for each
/// primary ray the bounced rays.
///
/// @param [in] rBouncedRay bounced ray
void Camera::reportPrimaryRay(Ray& rBouncedRay)
{
    size_t uId = rBouncedRay.getId();

    Float fRed, fGreen, fBlue;

    SpectrumRGB *pSpectrum = rBouncedRay.getDataRGBUnpolarized();
    pSpectrum->toRGB(fRed, fGreen, fBlue);
    delete pSpectrum;

    m_pfBuffer[3*uId+0] += fRed;
    m_pfBuffer[3*uId+1] += fGreen;
    m_pfBuffer[3*uId+2] += fBlue;
}

/// @brief Save image to file
///
/// Save image as PPM file. The tone mapping operator given by toneMapping
/// will be used.
///
/// @param [in] rsFilename filename of the PPM file
/// @param [in] toneMapping tone mapping operator
void Camera::save(const std::string &rsFilename,  const ToneMapping &toneMapping)
{
    using std::endl;
    std::ofstream file;

    file.open(rsFilename);

    if(!file.is_open())
        throw OSError("Cannot open file: " + rsFilename);

    // Number of colors
    const int maxv = 65535;

    // Write PPM header
    file << "P3" << endl;
    file << m_uWres << " " << m_uHres << endl;
    file << maxv << endl;

    for(unsigned int i = 0; i < 3*m_uWres*m_uHres; i++)
    {
        const Float v = m_pfBuffer[i]/static_cast<float>(m_iSamples);
        if(i && i % (3*m_uWres) == 0)
            file << endl;

        file << (int)(maxv*toneMapping(v)) << " ";
    }
}

void Camera::saveRaycaster(const std::string &rsFilename)
{
    using std::endl;
    std::ofstream file;

    file.open(rsFilename);

    if(!file.is_open())
        throw std::runtime_error("Cannot open file: " + rsFilename);

    float *buffer = m_pfBuffer;

    // Number of colors
    const int maxv = 255;

    // Write PPM header
    file << "P3" << endl;
    file << m_uWres << " " << m_uHres << endl;
    file << maxv << endl;

    // Find maximum
    float maximum = 0;
    for(unsigned int i = 0; i < 3*m_uWres*m_uHres; i++)
        if(buffer[i] > maximum)
            maximum = buffer[i];

    // Factor to scale brightness
    const float factor = maxv/maximum;

    for(unsigned int i = 0; i < 3*m_uWres*m_uHres; i++)
    {
        const float v = buffer[i];
        if(i && i % (3*m_uWres) == 0)
            file << endl;

        file << (int)(factor*v) << " ";
    }
}