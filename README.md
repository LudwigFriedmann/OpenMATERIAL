![Pathtracer Build](https://github.com/LudwigFriedmann/OpenMaterial/workflows/Pathtracer%20Build/badge.svg)

OpenMaterial
============

In virtual development, test and validation of automated and autonomous driving systems, 3D models are used
for the geometric representation of the environment of simulated vehicles as well as for the vehicles themselves.
For a long time, physical correctness in the visual representation of those geometries was not fundamental. Due
to limited computing capacities, implementations were designed for lowest possible memory and computing
time requirements while providing a visually plausible appearance. 

For physical sensor simulation, which is becoming increasingly important in the context mentioned above, this
approach is suitable to a limited extent. Instead of visual plausibility, physically correct modelling of material
properties is fundamental in order to achieve valid results. Besides sensor simulation, modern rendering solutions
also require physical material properties in 3D models in order to be able to reproduce physically correct lighting,
reflections and shadowing.

The specific architecture of a simulation framework used in the above-mentioned context poses further demand
for action. Within the framework, which may be set up distributed over several compute nodes, subsystems such
as an environment simulation, rendering and sensor models are implemented as individual software components.
Internally, these components use non-standardized 3D models to represent the environment and road users,
each for its specific purpose. Examples are visual representation as well as the calculation of wheel contact
points and the propagation of beams or wave fronts. The implementation can be both in the form of a white box
or a black box and may originate from different suppliers.  

<p align="center"><img src="simulation_architecture.png" alt="simulation_architecture" width="730" height="306"></p>

A 3D model exchange format, physical material properties and their annotation in corresponding 3D models are
not yet standardized in this context. This results in ongoing integration efforts as well as in incompatibilities
among software components. Yet, it is not possible to guarantee the consistency of 3D models in environmental
simulation, rendering and sensor simulation. Moreover, the lack of uniform material definitions and annotations
prevents the creation of comprehensive 3D model databases and material databases for virtual development,
test and validation. For the comprehensive use of physical sensor models, which requires extensive, uniformly
defined environmental geometries and associated materials, this standardization measure is indispensable. 

Overall goal of this project is the development of a generic, standardized 3D model exchange format with
physically correct description of materials for rendering and sensor simulation. For this purpose, this repository
contains proposals for extensions to the [Khronos Group glTF 2.0](https://github.com/KhronosGroup/glTF) file format. Usage of the proposed
extensions is demonstrated by a raycaster / pathtracer implementation and a collection of example files for
objects and materials:

| Filepath                              | Description
|:--------------------------------------|:------------------------------------------------------------------------------------------------------------------------------------------|
| [`external`](external/)               | Third-party dependencies                                                                                                                  |
| [`glTF_extensions`](glTF_extensions/) | Proposed glTF extensions defining asset properties, providing physically correct material descriptions and enabling glTF file referencing |
| [`hdr`](hdr/)                         | Examples of HDR (high dynamic range) images                                                                                               |
| [`materials`](materials/)             | Examples of glTF materials using the proposed glTF extensions                                                                             |
| [`objects`](objects/)                 | Examples of glTF 3D objects using the proposed glTF extensions                                                                            |
| [`pathtracer`](pathtracer/)           | Implementation of a raycaster / pathtracer using the proposed glTF extensions                                                           |
