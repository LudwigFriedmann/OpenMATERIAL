OpenMaterial_reference_link
===========================

This is a proposal for an extension to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) specification. **OpenMaterial_reference_link** enables
referencing of other files from glTF (including glTF itself). Thus, it enables modularity and therefore improves exchangeability
of assets and materials.

In order to use this extension, it has to be listed in the ``extensionsUsed`` section of the corresponding asset:
```
"extensionsUsed" : [
    "OpenMaterial_reference_link"
]
```

Properties
----------

Within the following enlistment of properties specified by the proposed extension, items labeled as **required** are
mandatory and must be present. Properties without **required** label are optional and may be omitted:

* **`title`** [string]
Human-readable title of the referenced asset. This property is intended to help humans get an idea what the referenced
asset is.

* **`uri`** [string][**required**]
Uniform Resource Identifier (URI) that identifies the external referenced asset. The URI must conform to the [RFC 3986](https://tools.ietf.org/html/rfc3986)
specification.

> **Note:** An implementation of this extension should at least support the ``file://`` scheme and relative references. Relative
paths are relative to the corresponding asset. In addition to the schemes defined by RFC 3986, implementation may
support the scheme ``id://`` to allow referencing other assets by the ``id`` property specified in ``OpenMaterial_asset_info``.


Example: Referencing materials
------------------------------
The asset file `cube.gltf` contains geometry and references material from `aluminium.gltf`:
  
  ```
  {
      "asset": {...},
      "scenes": [...],
      "nodes": [...],
      "meshes": [...],
      "materials": [
          {
              "extensions": {
                  "OpenMaterial_reference_link": {
                      "title": "aluminium",
                      "uri": "aluminium.gltf"
                  }
              }
          }
      ],
      "accessors" : [...],
      "bufferViews" : [...],
      "buffers" : [...],
      "extensionsUsed": [
          "OpenMaterial_reference_link"
      ]
  }
  ```

The material file `aluminium.gltf` contains material properties:
  ```
  {
      "asset": {...},
      "materials": [
          {
              "name": "aluminium",
              "extensions": {
                  "OpenMaterial_material_parameters": {
                      "user_preferences": {
                          "geometrical_optics": true,
                          "include_diffraction": false,
                          "include_numerical_simulation": false,
                          "material_scheme": "surface",
                          "material_classification": "solid-metal",
                          "material_type": {
                              "isotropic": true,
                              "homogeneous": true,
                              "magnetic": false
                          },
                          "temperature": 300.0,
                          "surface_displacement_uri": "",
                          "surface_roughness": {
                              "surface_height": 0.0,
                              "surface_correlation_length": 0.0
                          },
                          "coating_materials": [],
                          "lambert_emission": 0.0,
                          "subsurface": {
                            "subsurface": false,
                            "subsurface_thickness": 0.0
                          },
                          "ingredients": []
                      },
                      "physical_properties": {
                          "refractive_index_uri": "data/aluminium_ior.gltf",
                          "mean_free_path": 0.0,
                          "particle_density": 0.0,
                          "particle_cross_section": 0.0,
                          "emissive_coefficient_uri": "",
                          "applicable_sensors": [
                              "camera",
                              "lidar",
                              "radar"
                          ]
                      }
                  }
              }
          }
      ]
  }
  ```

After resolving reference to material, `cube.gltf` would look like this:
  ```
  {
      "asset": {...},
      "scenes": [...],
      "nodes": [...],
      "meshes": [...],
      "materials": [
          {
              "name": "aluminium",
              "extensions": {
                  "OpenMaterial_material_parameters": {
                      "user_preferences": {
                          "geometrical_optics": true,
                          "include_diffraction": false,
                          "include_numerical_simulation": false,
                          "material_scheme": "surface",
                          "material_classification": "solid-metal",
                          "material_type": {
                              "isotropic": true,
                              "homogeneous": true,
                              "magnetic": false
                          },
                          "temperature": 300.0,
                          "surface_displacement_uri": "",
                          "surface_roughness": {
                              "surface_height": 0.0,
                              "surface_correlation_length": 0.0
                          },
                          "coating_materials": [],
                          "lambert_emission": 0.0,
                          "subsurface": {
                            "subsurface": false,
                            "subsurface_thickness": 0.0
                          },
                          "ingredients": []
                      },
                      "physical_properties": {
                          "refractive_index_uri": "data/aluminium_ior.gltf",
                          "mean_free_path": 0.0,
                          "particle_density": 0.0,
                          "particle_cross_section": 0.0,
                          "emissive_coefficient_uri": "",
                          "applicable_sensors": [
                              "camera",
                              "lidar",
                              "radar"
                          ]
                      }
                  }
              }
          }
      ],
      "accessors" : [...],
      "bufferViews" : [...],
      "buffers" : [...],
      "extensionsUsed": [
          "OpenMaterial_material_parameters"
      ]
  }
  ```

glTF Schema Updates
-------------------
N/A

JSON Schema
-----------
[OpenMaterial_reference_link.schema.json](schema/OpenMaterial_reference_link.schema.json)

Known Implementations
---------------------
N/A