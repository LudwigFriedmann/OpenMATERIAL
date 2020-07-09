OpenMaterial_asset_info
=======================

This is a proposal for an extension to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) specification. **OpenMaterial_asset_info** defines asset
properties in order to enable semantic classifaction of assets. While the main focus of this extension is the use in the
automotive industry, it is also applicable to other fields.

In order to use this extension, it has to be listed in the ``extensionsUsed`` section of the corresponding asset:
```
"extensionsUsed" : [
    "OpenMaterial_asset_info"
]
```

Properties
----------

Within the following enlistment of properties specified by the proposed extension, items labeled as **required** are
mandatory and must be present. Properties without **required** label are optional and may be omitted:

* **`asset_type`** [string][**required**]
Asset type. Available options comprise:
  - `geometry`
  - `material`
  - `scene`
  - `sensor`
  - `texture`

* **`asset_parent`** [string]
Unique identifier of the parent asset, specified in the same format as the property ``id``.
If this property is set, either ``asset_variation`` or ``asset_version`` must be set (but not both).

* **`asset_variation`** [string]
Human readable description of changes in parent asset referenced in `asset_parent` property.
In other words, this property describes how the corresponding asset differs from the parent asset.
If this property is set, ``asset_parent`` must be set and ``asset_version`` must not be set.

* **`asset_version`** [string]
Version of the asset. `asset_parent` property references previous version of the asset.  
If this property is set, ``asset_parent`` must be set and ``asset_variation`` must not be set.

* **`category`** [string]
Asset category as defined in [cityscapes](https://www.cityscapes-dataset.com) labels. Available options comprise:
  - bicycle
  - bridge
  - building
  - bus
  - car
  - caravan
  - dynamic
  - ego_vehicle
  - fence
  - ground
  - guard_rail
  - license_plate
  - motorcycle
  - out_of_roi
  - parking
  - person
  - pole
  - polegroup
  - rail_track
  - rectification_border
  - rider
  - road
  - sidewalk
  - sky
  - static
  - terrain
  - traffic_light
  - traffic_sign
  - trailer
  - train
  - truck
  - tunnel
  - unlabeled
  - vegetation
  - wall

* **`creation_date`** [string]
Timestamp of the asset creation. Format: "yyyy.mm.dd. hh:mm:ss"

* **`creator`** [string][**required**]
Name of the asset creator.

* **`description`** [string]
Human readable description of the asset.

* **`id`** [string][**required**]
Unique asset identifier. The ``id`` consists of 32 hexadecimal numerals and must not contain additional characters like
dashes or minus signs. Only use lower case characters are allowed. Allowed values are described with regex pattern:
"\^[0-9a-f]{32}$".

* **`sources`** [array of strings]
List of sources that have been used to create the asset. Examples for sources are Digital Object Identifiers (DOI), a link
to a website or the like.

* **`tags`** [string]
Tags for the asset. Tags may comprise a single word, only. Tags have to be separated by whitespace.

* **`title`** [string][**required**]
Human readable title of the asset.

glTF Schema Updates
-------------------
N/A

JSON Schema
-----------
[OpenMaterial_asset_info.schema.json](schema/OpenMaterial_asset_info.schema.json)

Known Implementations
---------------------
N/A