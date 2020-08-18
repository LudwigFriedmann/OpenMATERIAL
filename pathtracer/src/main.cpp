//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      main.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-09-30
/// @brief     Main function of the path tracer

#include <iostream>
#include <vector>

#include "argparse.h"
#include "doctest.h"

#include "assetgeometry.h"
#include "assetinstance.h"
#include "backgrounds/hdr.h"
#include "backgrounds/fatal.h"
#include "materialmodels/specular.h"
#include "renderer.h"
#include "sensors/camera.h"
#include "tonemapping.h"
#include "vector.h"

/// Entry point of the program
/// @param [in] arguments from command line
/// @param [out] status code

int main(int argc, const char *argv[])
{
    using std::cout;
    using std::cerr;
    using std::endl;

    int iTesting = 0, iVerbose = 0, iCentering = 0, iRaycaster = 0;
    int iResolution = 800;
    Vector3 axisRotation(0,1,0);
    Quaternion q;
    float fAlpha = 0.1, fPhiDeg = 0, fDisplacementmap = 0.1, fScaling=1.0;
    const char *sGltfFile = "", *sOutputFile = "out.ppm", *sHdrFile = "", *sAxisRotation = "", *sDispMap = "", *sUVmaptype = "random", *sEulerRotation = "";

    // Parse command line options
    static const char *const usage[] = {
        "pathtracer [options] [[--] args]",
        "pathtracer [options]",
        nullptr,
    };

    struct argparse_option options[] = {
        OPT_HELP(),

        OPT_GROUP("Required arguments"),
        OPT_STRING('i', "input", &sGltfFile, "path to glTF file", nullptr, 0, 0),

        OPT_GROUP("Optional arguments"),
        OPT_STRING('H', "hdr", &sHdrFile, "path to HDR file", nullptr, 0, 0),
        OPT_FLOAT('a', "alpha", &fAlpha, "alpha value for tone mapping (default: 0.1)", nullptr, 0, 0),
        OPT_INTEGER('r', "resolution", &iResolution, "resolution (default: 800)", nullptr, 0, 0),
        OPT_STRING('n', "axis", &sAxisRotation, "axis of rotation (default: 0,1,0)", nullptr, 0, 0),
        OPT_FLOAT('p', "phi", &fPhiDeg, "rotation around axis in degrees (default: 0)", nullptr, 0, 0),
        OPT_STRING('e', "Euler angles", &sEulerRotation, "Euler angles of rotation in a right-handed system (z axis pointing towards the user). Please type the three angles (in Degrees) separated by a comma (example: 45,0,45). First value is the rotation around the z-axis (Roll->bank angle), second around the y-axis (Yaw->bearing) and third around x-axis (Pitch->elevation)", NULL, 0, 0),
        OPT_FLOAT('s', "scaling factor", &fScaling, "scaling factor, works only in combination with -e (default: 1)", nullptr, 0, 0),
        OPT_STRING('o', "output", &sOutputFile, "output file", nullptr, 0, 0),
        OPT_BOOLEAN('v', "verbose", &iVerbose, "print debugging output", nullptr, 0, 0),
        OPT_BOOLEAN('t', "test", &iTesting, "run unit tests and exit", nullptr, 0, 0),
        OPT_BOOLEAN('R', "raycaster", &iRaycaster, "calls raycaster (no bounces, no background)", nullptr, 0, 0),
        OPT_BOOLEAN('c', "automaticcentering", &iCentering, "automatic scene centering", nullptr, 0, 0),
        OPT_STRING('d', "displacementmap", &sDispMap, "input displacement map path", nullptr, 0, 0),
        OPT_FLOAT('m', "m_displacementmap", &fDisplacementmap, "input displacement map overall magnification (default is 0.1)", nullptr, 0, 0),
        OPT_STRING('U', "m_UVmaptype", &sUVmaptype, "input displacement UV mapping type (random,sphere,cube,cylinder). (Default is random)", nullptr, 0, 0),
        OPT_END(),
    };

    struct argparse argparse{};
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nRender a glTF object with or without HDR background", nullptr);
    argc = argparse_parse(&argparse, argc, argv);

    // Run unit tests
    if(iTesting)
    {
        doctest::Context ctx;
        // Stop after 5 failed asserts
        ctx.setOption("abort-after", 5);
        ctx.run();
        return 0;
    }

    if(!strlen(sGltfFile))
    {
        cerr << "Missing parameter --input" << endl << endl;
        argparse_usage(&argparse);
        return 1;
    }

    if(!strlen(sHdrFile) && !iRaycaster)
    {
        cout << "Parameter --hdr is not specified,  raycaster mode is enabled" << endl;
        iRaycaster=1;
    }

    if(!utils::path::fileExists(sGltfFile))
    {
        cerr << "File " << sGltfFile << " does not exist." << endl;
        return 1;
    }

    if(!utils::path::fileExists(sHdrFile)&& !iRaycaster)
    {
        cerr << "File " << sHdrFile << " does not exist." << endl;
        return 1;
    }

    if(strlen(sAxisRotation))
    {
        auto junks = utils::string::split(sAxisRotation, ",");

        try
        {
            float x = std::stof(junks.at(0));
            float y = std::stof(junks.at(1));
            float z = std::stof(junks.at(2));
            axisRotation = Vector3(x,y,z).normalize();
        }
        catch(...)
        {
            cerr << "Invalid format for axis." << endl;
            return 1;
        }
    }

    if(strlen(sEulerRotation))
    {
        auto junks = utils::string::split(sEulerRotation, ",");
        try
        {
            double yaw = -std::stod(junks.at(0))*M_PI/180; //z
            double pitch = std::stod(junks.at(1))*M_PI/180; //X
            double roll = -std::stod(junks.at(2))*M_PI/180; //Y

            // Abbreviations for the various angular functions
            double cy = cos(yaw * 0.5);
            double sy = sin(yaw * 0.5);
            double cp = cos(pitch * 0.5);
            double sp = sin(pitch * 0.5);
            double cr = cos(roll * 0.5);
            double sr = sin(roll * 0.5);

            /// Create a quaternion q = w + x*i + y*j + z*k
            auto w = static_cast<float>(cy * cp * cr + sy * sp * sr);
            auto x = static_cast<float>(cy * cp * sr - sy * sp * cr);
            auto y = static_cast<float>(sy * cp * sr + cy * sp * cr);
            auto z = static_cast<float>(sy * cp * cr - cy * sp * sr);
            Quaternion swap(x, y, z, w);
            q=swap;
        }
        catch(...)
        {
            cerr << "Invalid format for Euler angles." << endl;
            return 1;
        }
    }

    // Load glTF file
    cout << "Loading " << sGltfFile << endl;
    AssetGeometry assetGeometry(sGltfFile);

    // Load displacement map
	if (std::strcmp(sDispMap, "") != 0)
	{
		assetGeometry.loadDisplacementMap(sDispMap, fDisplacementmap, sUVmaptype);
	}
    
    // Create Bounding Volume Hierarchy (BVH)
    assetGeometry.BVH();

    cout << "Loaded " << assetGeometry.getTitle() << endl;

    // Create instance with rotation of fPhi around axisRotation
    double fPhi = fPhiDeg*M_PI/180.0;

	// Create instance with Euler's rotation and traslation
    Vector3 S(fScaling,fScaling,fScaling);

	Vector3 O(0, 0, 0);
	Vector3 T = O;

    // Change the center of rotation to the geometrical center of the object (i.e. the center of the objects bounding box)
    if (iCentering)
	{
		std::vector<int> nd =assetGeometry.getviSceneNodes();
		if (nd.size()>1) cout<<"Warning, the scene contains more than one object. Automatic centering is disabled"<<endl;
		else
		{
			BBox bbox = assetGeometry.getBBox();
			Float x = 0.5*(bbox.getMax()[0]+bbox.getMin()[0]);
			Float y = 0.5*(bbox.getMax()[1]+bbox.getMin()[1]);
			Float z = 0.5*(bbox.getMax()[2]+bbox.getMin()[2]);
			Vector3 T2(-x,-y,-z);
			T=T2;
			std::cout<<"Automatic centering is enabled"<<std::endl;
		}
    }

	Transformation transformation = Transformation::Translation(T);
    Transformation transformation2 = Transformation::Rotation(axisRotation, fPhi);
    if (strlen(sEulerRotation)){transformation2 = Transformation::TRS(O,q,S);}
    Transformation transformation3 = transformation2.apply(transformation);
    
	// Instantiate asset
    AssetInstance assetInstance(&assetGeometry, transformation3);

    // Set up the camera
    const Vector3 vCameraPosition(0,1,-4);
    const Vector3 vCameraDirection = Vector3(0,0,1);
    const Vector3 vCameraUp(0,1,0);
    Camera camera(vCameraPosition, vCameraDirection, vCameraUp, iResolution, iResolution);

    // Define material model
    MaterialModelSpecular materialModel;

	// Render
    cout << "Rendering the scene..." << endl;
    if (iRaycaster) {
        // Define background that throws exceptions
        BackgroundFatal backgroundfatal;
        Renderer renderer(&assetInstance, &materialModel, &backgroundfatal);
        renderer.setVerbose(iVerbose);
        renderer.setThresholdOnRadiance(0.0);
        renderer.setNumberofAllowedBounces(1);
        renderer.setRaytracer(1);
        renderer.render(camera);
    }
    else {
        // background
        BackgroundHDR background(sHdrFile);
        Renderer renderer(&assetInstance, &materialModel, &background);
        renderer.setVerbose(iVerbose);
        renderer.render(camera);
    }

	// Save output
    cout << "Saving image to " << sOutputFile << endl;
    ToneMapping toneMapping(fAlpha);
    if (iRaycaster)
        camera.saveRaycaster(sOutputFile);
    else
        camera.save(sOutputFile, toneMapping);

    return 0;
}