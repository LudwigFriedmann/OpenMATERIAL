OpenMaterial_conductivity_data
==============================

This is a proposal for an extension to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) specification. The goal of this extension is to
provide a physically accurate description of the conductivity at a given temperature and wavelength range
in a look-up table. Values within the range which aren't explicitly listed can be obtained using
linear interpolation.

The extension may serve as a blueprint for further material-specific look-up tables referenced from the
``physical_properties`` section of the **OpenMaterial_material_parameters** extension. 

In order to use this extension, it has to be listed in the ``extensionsUsed`` section of a corresponding material
file:

```
"extensionsUsed" : [
    "OpenMaterial_conductivity_data"
]
```

Properties
----------

This extension provides conductivity data measured at a certain temperature for a certain wavelenght range.

Within the following enlistment of properties specified by the proposed extension, items labeled as **required** are
mandatory and must be present. Properties without **required** label are optional and may be omitted:

* **`temperature`** [number][**required**]
Temperature [K] at which an conductivity value was measured.
* **`conductivity`** [number][**required**]
Conductivity, key to an array of tuples of corresponding wavelengths [m] and conductivity [S/m] values.

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
                    "conductivity_uri": "data/iron_conductivity.gltf", <======					
                    ...
                }
            }
        }
    }
],
````

The conductivity data file (in this case `iron_conductivity.gltf`) provides the data to compute conductivity at the geometry: 

````
"extensions": {
    "OpenMaterial_ior_data": {
        "data": [
            {
                "temperature": 300.0,
                "conductivity": [
                    [3.89341e-03, 1.0e-07]
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
[OpenMaterial_conductivity_data.schema.json](schema/OpenMaterial_conductivity_data.schema.json)

Known Implementations
---------------------
N/A