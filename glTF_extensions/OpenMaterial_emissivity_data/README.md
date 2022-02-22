OpenMaterial_emissivity_data
============================

This is a proposal for an extension to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) specification. The goal of this extension is to
provide a physically accurate description of the emissivity coefficient at a given temperature in a look-up table. Values within the range which aren't
explicitly listed can be obtained using linear interpolation.

The extension may serve as a blueprint for further material-specific look-up tables referenced from the
``physical_properties`` section of the **OpenMaterial_material_parameters** extension. 

In order to use this extension, it has to be listed in the ``extensionsUsed`` section of a corresponding material
file:

```
"extensionsUsed" : [
    "OpenMaterial_emissivity_data"
]
```

Properties
----------

This extension provides emissivity coefficient data measured at a certain temperature.

Within the following enlistment of properties specified by the proposed extension, items labeled as **required** are
mandatory and must be present. Properties without **required** label are optional and may be omitted:

* **`temperature`** [number][**required**]
Temperature [K] at which an emissivity coefficient value was measured.
* **`emissivity`** [number][**required**]
Emissivity coefficient [-].

Example
-------

Within the section `physical_properties` of its **OpenMaterial_material_parameters** section, a material file
(e.g. `aluminium.gltf`) provides an URI to a emissivity coefficient data file:

```
"materials": [
    {
        "name": "aluminium",
        "extensions": {
            "OpenMaterial_material_parameters": {
                "user_preferences": {
                    ...
                },
                "physical_properties": {
                    ...
                    "emissive_coefficient_uri": "data/aluminium_emissivity.gltf", <======					
                    ...
                }
            }
        }
    }
],
````

The emissivity coefficient data file (in this case `aluminium_emissivity.gltf`) provides the data to compute emissivity at the geometry: 

````
"extensions": {
	"OpenMaterial_emissivity_data": {
		"data": [
			{
				"temperature": 300.0,
				"emissivity": 0.048
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
[OpenMaterial_emissivity_data.schema.json](schema/OpenMaterial_emissivity_data.schema.json)

Known Implementations
---------------------
N/A
