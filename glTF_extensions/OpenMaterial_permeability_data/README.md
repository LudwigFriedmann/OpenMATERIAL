OpenMaterial_permeability_data
==============================

This is a proposal for an extension to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) specification. The goal of this extension is to
provide a physically accurate description of the relative permeability at a given temperature, incident angle
and wavelength range in a look-up table. Values within the range which aren't explicitly listed can be
obtained using linear interpolation.

The extension may serve as a blueprint for further material-specific look-up tables referenced from the
``physical_properties`` section of the **OpenMaterial_material_parameters** extension. 

In order to use this extension, it has to be listed in the ``extensionsUsed`` section of a corresponding material
file:

```
"extensionsUsed" : [
    "OpenMaterial_permeability_data"
]
```

Properties
----------

This extension provides relative permeability data measured at a certain incident angle and temperature for a certain wavelenght range.

Within the following enlistment of properties specified by the proposed extension, items labeled as **required** are
mandatory and must be present. Properties without **required** label are optional and may be omitted:

* **`temperature`** [string][**required**]
Temperature (numeric value) in [K] at which an conductivity value was measured.
* **`incident_angle`** [string][**required**]
Incident angle (numeric value) of the measurement relative to the local normal vector at the material surface in [rad].
* **`permeability`** [string][**required**]
Relative permeability, key to an array of tuples of corresponding wavelengths and permeability values.

Example
-------

Within the section `physical_properties` of its **OpenMaterial_material_parameters** section, a material file
(e.g. `iron.gltf`) provides an URI to a conductivity data file:

```
"materials": [
    {
        "name": "iron",
        "extensions": {
            "OpenMaterial_material_parameters": {
                "user_preferences": {
                    ...
                },
                "physical_properties": {
                    ...
                    "permeability_uri": "data/tarmac_permeability.gltf", <======					
                    ...
                }
            }
        }
    }
],
````

The permeability data file (in this case `tarmac_permeability.gltf`) provides the data to compute permeability at the geometry: 

````
"extensions": {
    "OpenMaterial_ior_data": {
        "data": [
            {
                "temperature": 300.0,
                "incident_angle": 0.785398,				
                "permeability": [
                    [3.89341e-03, xxx]
				]
            }
        ]
    }
},
````

glTF Schema Updates
-------------------
N/A

JSON Schema
-----------
[OpenMaterial_permeability_data.schema.json](schema/OpenMaterial_permeability_data.schema.json)

Known Implementations
---------------------
N/A