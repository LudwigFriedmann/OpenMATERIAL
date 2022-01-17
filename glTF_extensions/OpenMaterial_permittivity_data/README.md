OpenMaterial_permittivity_data
==============================

This is a proposal for an extension to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) specification. The goal of this extension is to
provide a physically accurate description of the relative permittivity at a given temperature, incident angle
and wavelength range in a look-up table. Values within the range which aren't explicitly listed can be
obtained using linear interpolation.

The extension may serve as a blueprint for further material-specific look-up tables referenced from the
``physical_properties`` section of the **OpenMaterial_material_parameters** extension. 

In order to use this extension, it has to be listed in the ``extensionsUsed`` section of a corresponding material
file:

```
"extensionsUsed" : [
    "OpenMaterial_permittivity_data"
]
```

Properties
----------

This extension provides relative permittivity data measured at a certain incident angle and temperature for a certain wavelenght range.

Within the following enlistment of properties specified by the proposed extension, items labeled as **required** are
mandatory and must be present. Properties without **required** label are optional and may be omitted:

* **`temperature`** [string][**required**]
Temperature (numeric value) in [K] at which an conductivity value was measured.
* **`incident_angle`** [string][**required**]
Incident angle (numeric value) of the measurement relative to the local normal vector at the material surface in [rad].
* **`permittivity`** [string][**required**]
Relative permittivity, key to an array of tuples of corresponding wavelengths and permittivity values.

Example
-------

Within the section `physical_properties` of its **OpenMaterial_material_parameters** section, a material file
(e.g. `tarmac.gltf`) provides an URI to a conductivity data file:

```
"materials": [
    {
        "name": "tarmac",
        "extensions": {
            "OpenMaterial_material_parameters": {
                "user_preferences": {
                    ...
                },
                "physical_properties": {
                    ...
                    "permittivity_uri": "data/tarmac_permittivity.gltf", <======					
                    ...
                }
            }
        }
    }
],
````

The permittivity data file (in this case `tarmac_permittivity.gltf`) provides the data to compute permittivity at the geometry: 

````
"extensions": {
    "OpenMaterial_ior_data": {
        "data": [
            {
                "temperature": 300.0,
                "incident_angle": 0.785398,				
                "permittivity": [
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
[OpenMaterial_permittivity_data.schema.json](schema/OpenMaterial_permittivity_data.schema.json)

Known Implementations
---------------------
N/A