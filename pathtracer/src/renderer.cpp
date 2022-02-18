//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      renderer.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-05-28
/// @brief     Renderer

#include <cmath>
#include <vector>

#include "background.h"
#include "doctest.h"
#include "ray.h"
#include "sensor.h"
#include "renderer.h"
#include "mesh.h"
#include "materialmodel.h"
#include "intersect.h"

/// @brief Unit test for AssetInstance::AssetInstance 
TEST_CASE("Testing AssetInstance::AssetInstance")
{
	std::string fileName = "../objects/cube_gold.gltf";
	#ifdef _WIN32
		std::replace(fileName.begin(), fileName.end(), '/', '\\');
	#endif
	
	AssetGeometry assetGeometry(fileName);
	assetGeometry.BVH();
	Transformation transformation = Transformation::Translation({0,0,0});

	// Checking asset instantiation by ray intersection
	AssetInstance assetInstance(&assetGeometry, transformation);
	Ray ray(0, { 0, 1, -4 }, { 0, 0, 1 });
	Intersection intersection(ray);
	CHECK(assetInstance.intersectRay(ray, intersection));
}

/// @brief Create a new render object
///
/// @param [in] cpAssetInstance pointer to AssetInstance object
/// @param [in] pMaterialModel pointer to MaterialModel object
/// @param [in] pBackground pointer to Background object (may be nullptr)
Renderer::Renderer(const AssetInstance *cpAssetInstance, MaterialModel *pMaterialModel, Background *pBackground)
{
    m_cpAssetInstance = cpAssetInstance;
    m_pMaterialModel = pMaterialModel;
    m_pBackground = pBackground;

    omp_init_lock(&m_ompWritelock);
}

/// Enable/disable verbose information
void Renderer::setVerbose(bool bVerbose)
{
    m_bVerbose = bVerbose;
}

/// Return pointer to background object
const Background *Renderer::getBackground() const
{
    return m_pBackground;
}

/// Return pointer to material model
const MaterialModel *Renderer::getMaterialModel() const
{
    return m_pMaterialModel;
}

/// Return pointer to instance
const AssetInstance *Renderer::getAssetInstance() const
{
    return m_cpAssetInstance;
}

/// @brief Trace a ray and generate a ray path
///
/// First, the closest hit point of the incident ray with geometry is computed.
/// If the sensor's method callSensor returns true, the sensor's method hook
/// is called with the information on the intersection. For more details, refer
/// to the documentation of the Sensor interface.
///
/// If the ray hits geometry, the method bounce of the given material model is
/// called that generates zero or more bounced rays that will be traced through
/// the scene. If the ray does not hit geometry and a background object was
/// given, the hit method of the background object is called and the returned
/// ray is reported back to the sensor, i.e., reportPrimaryRay is called.
///
/// @param [in] rSensor reference to sensor
/// @param [in] incidentRay incident ray
void Renderer::trace(Sensor& rSensor, Ray& incidentRay)
{
    Intersection intersection(incidentRay);
    bool hit = m_cpAssetInstance->intersectRay(incidentRay, intersection);
    float totalradiance=1;
    // If the method callSensors returns true, call the hook function of the
    // sensor. See Sensor::callSensor and Sensor::hook for more information.
    if(rSensor.callSensor())
    {
        bool bCont = rSensor.hook(*this, incidentRay, intersection);
        if(!bCont)
            return;
    }
    else
    {
        SpectrumRGB *pSpectrum = incidentRay.getDataRGBUnpolarized();
        totalradiance=static_cast<float>((*pSpectrum)[0].second+(*pSpectrum)[1].second+(*pSpectrum)[2].second)/3.0f;
    }
    if(hit and incidentRay.getNumberOfBounces()<m_thbounces and totalradiance>m_minradiance)
    {
        if(m_bVerbose &&  incidentRay.getNumberOfBounces() == 0)
        {
            omp_set_lock(&m_ompWritelock);
	    std::cout<< " =========== Information on hit point ==========="<<std::endl;
	    std::cout<< "----Geometric Info----"<<std::endl;
            std::cout<< "    intersection point "<<intersection.getIntersectionPoint()<<std::endl;
	    std::cout<< "    ray origin = "<<incidentRay.getOrigin()<<std::endl;
	    std::cout<< "    ray direction =  "<<incidentRay.getDirection()<<std::endl;
	    std::cout<< "    ray Id "<<incidentRay.getId()<<std::endl;

	    std::cout<< "----Material Physical Properties----"<<std::endl;
	    std::cout<< "       *General Properties----"<<std::endl;
	    std::cout<< "            classification  = "<< intersection.getMaterial()->getName()<<std::endl;
	    std::cout<< "            temperature =  "<< intersection.getMaterial()->getTemperature()<<std::endl;
	    std::cout<< "            mean free path =  "<<intersection.getMaterial()->getMeanFreePath()<<std::endl;
	    std::cout<< "            lambert emission =  "<<intersection.getMaterial()->getLambertEmission()<<std::endl;
	    std::cout<< "            particle density =  "<< intersection.getMaterial()->getParticleDensity()<<std::endl;
	    std::cout<< "            particle cross section =  "<<intersection.getMaterial()->getParticleCrossSection()<<std::endl;
	    std::cout<< "       *Camera/Lidar Related Properties----"<<std::endl;
	    std::cout<< "            index of refraction uri =  "<<  intersection.getMaterial()->getIndexOfRefractionUri()<<std::endl;
	    std::cout<< "            emissivity coefficient uri =  "<<  intersection.getMaterial()->getEmissivityCoefficientUri()<<std::endl;                        
	    std::cout<< "       *Radar Related Properties----"<<std::endl;
	    std::cout<< "            effective particle area =  "<<  intersection.getMaterial()->getEffectiveParticleArea()<<std::endl;
	    std::cout<< "            relative permittivity uri =  "<<  intersection.getMaterial()->getRelativePermittivityUri()<<std::endl;
	    std::cout<< "            relative permeability uri =  "<<  intersection.getMaterial()->getRelativePermeabilityUri()<<std::endl;
	    std::cout<< "            electrical resistivity =  "<<  intersection.getMaterial()->getElectricalResistivity()<<std::endl;
	    std::cout<< "       *Ultrasound Related Properties----"<<std::endl;
	    std::cout<< "            acoustic impedance =  "<<  intersection.getMaterial()->getAcousticImpedance()<<std::endl;
	    std::cout<< "            shear velocity =  "<<  intersection.getMaterial()->getShearVelocity()<<std::endl;

	    std::cout<< "----Material User Properties----"<<std::endl;	    
	    std::cout<< "    material scheme =  "<<intersection.getMaterial()->getMaterialSchemeAsString()<<std::endl;
	    std::cout<< "    geometric optics =  "<<intersection.getMaterial()->getGeometricalOptics()<<std::endl;
	    std::cout<< "    include diffraction =  "<<  intersection.getMaterial()->getIncludeDiffraction()<<std::endl;
	    std::cout<< "    include numerical simulation =  "<<  intersection.getMaterial()->getIncludeNumericalSimulation()<<std::endl;
	    std::cout<< "    material classification =  "<<  intersection.getMaterial()->getMaterialClassification()<<std::endl;
	    std::cout<< "    surface displacement uri =  "<<  intersection.getMaterial()->getSurfaceDisplacementUri()<<std::endl;
	    std::vector<st_CoatingMaterial> v = intersection.getMaterial()->getCoatingMaterials(); 
	    std::cout<< "    coating material.material_ref =  "<<  v[0].sMaterialRef<<std::endl;
	    std::cout<< "    coating material.fLayerThickness =  "<<  v[0].fLayerThickness<<std::endl;
	    std::vector<st_Ingredient> I = intersection.getMaterial()->getIngredients();
	    std::cout<< "    ingredient.material_ref =  "<<  I[0].sMaterialRef<<std::endl;
	    std::cout<< "    ingredient.order =  "<<  I[0].sDistributionPatternUri<<std::endl;	 
	    std::cout<< "    subsurface_subsurface =  "<<  intersection.getMaterial()->getSubsurface().bSubsurface<<std::endl;
	    std::cout<< "    subsurface_thickness =  "<<  intersection.getMaterial()->getSubsurface().fSubsurfaceThickness<<std::endl;
	    std::cout<< "    material_type.isotropic =  "<<  intersection.getMaterial()->getMaterialType().bIsotropic<<std::endl;
	    std::cout<< "    material_type.homogeneous =  "<<  intersection.getMaterial()->getMaterialType().bHomogeneous<<std::endl;
	    std::cout<< "    material_type.magnetic =  "<<  intersection.getMaterial()->getMaterialType().bMagnetic<<std::endl;
	    std::cout<< "    surface roughness.surface_height =  "<<  intersection.getMaterial()->getSurfaceRoughness().fSurfaceHeight<<std::endl;
	    std::cout<< "    surface roughness.surface_correlation_lenght =  "<<  intersection.getMaterial()->getSurfaceRoughness().fSurfaceCorrelationLength<<std::endl;
            omp_unset_lock(&m_ompWritelock);
        }
        // Call trace for each bounced ray again
        for(Ray& bouncedRay : m_pMaterialModel->bounce(incidentRay, intersection, rSensor, *this)){
            trace(rSensor, bouncedRay);
           if (m_raytracer>0) rSensor.reportPrimaryRay(bouncedRay);
        }
    }
    // If we don't hit geometry but we were given a background...
    else if(m_pBackground && m_raytracer <1)
    {
        // Modify the incident ray, i.e., include the background
        m_pBackground->hit(incidentRay);
        // and report back the ray to the sensor
        rSensor.reportPrimaryRay(incidentRay);
    }
}

/// @brief Render scene for given sensor
///
/// The renderer will first check if the given sensor is compatible with the
/// material model and throw an exception if not. Then, the sensor's method
/// getPrimaryRays is called to generate the primary rays. The primary rays
/// are traced through the scene. If supported by the sensor, the sensor's
/// method hook is called while tracing the ray. After all primary rays have
/// been traced, the sensor's method deletePrimaryRays is called.
///
/// @param [in] rSensor sensor object
void Renderer::render(Sensor& rSensor)
{
    // Check if sensor and material model are compatible
    if(!rSensor.isCompatible(*m_pMaterialModel))
        throw RuntimeError("Sensor incompatible to material model");

    while(true)
    {
        std::vector<Ray> primaryRays = rSensor.getPrimaryRays();
        size_t uElems = primaryRays.size();
        if(uElems == 0)
            break;

        #pragma omp parallel for default(none) shared (uElems,rSensor,primaryRays)
        for(int i = 0; i < (int)uElems; i++)trace(rSensor, primaryRays[i]);
		
        // call deletePrimaryRays for cleanup
        rSensor.deletePrimaryRays(primaryRays);
    };
}