OpenMaterial_ior_data
=====================

This is a proposal for an extension to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) specification. The goal of this extension is to
provide a physically accurate description of the complex index of refraction (IOR) at a given temperature and
wavelength range in a look-up table. Values within the range which aren't explicitly listed can be obtained using
linear interpolation.

The extension may serve as a blueprint for further material-specific look-up tables referenced from the
``physical_properties`` section of the **OpenMaterial_material_parameters** extension. 

In order to use this extension, it has to be listed in the ``extensionsUsed`` section of a corresponding material
file:

```
"extensionsUsed" : [
    "OpenMaterial_ior_data"
]
```

Properties
----------

This extension provides complex IOR data measured at a certain temperature for a certain wavelenght range.
The IOR data is assigned to either section "``n``" (which represents the real part of the IOR) or section "``k``"
(representing the imaginary part of the IOR) depending on their affiliation. "``n``" and "``k``" are keys to arrays of
tuples [wavelength, IOR data]. 

Within the following enlistment of properties specified by the proposed extension, items labeled as **required** are
mandatory and must be present. Properties without **required** label are optional and may be omitted:

* **`temperature`** [number][**required**]
Temperature [K] at which the IOR values were measured.
* **`n`** [array][**required**]
Real part of the IOR, key to an array of tuples of corresponding wavelengths [m] and IOR values [-].
* **`k`** [array][**required**]
Imaginary part of the IOR, key to an array of tuples of corresponding wavelengths [m] and IOR values [-].

Example
-------

Within the section `physical_properties` of its **OpenMaterial_material_parameters** section, a material file
(e.g. `aluminium.gltf`) provides an URI to a IOR data file:

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
                    "refractive_index_uri": "data/aluminium_ior.gltf", <======
                    ...
                }
            }
        }
    }
],
````

The IOR data file (in this case `aluminium_ior.gltf`) provides the data to compute reflections of rays at the
geometry: 

````
"extensions": {
	"OpenMaterial_ior_data": {
		"data": [
			{
				"temperature": 300.0,
				"n": [
					[200e-9, 0.110803374],
					...
					[1700e-9, 1.584018511]
				],
				"k": [
					[200e-9, 1.908606137],
					 ...
					[1700e-9,15.55632073]
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
[OpenMaterial_ior_data.schema.json](schema/OpenMaterial_ior_data.schema.json)

Known Implementations
---------------------
N/A
