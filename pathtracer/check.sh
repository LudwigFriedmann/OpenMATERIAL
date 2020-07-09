# This script renders all glTF objects. This script is used for fast and simple
# testing.

# remove old rendered pictures if they still exist
rm -f *.ppm

bin/pathtracer -i ../objects/bunny_gold.gltf  --hdr ../hdr/green_point_park_4k.hdr -o bunny.ppm

bin/pathtracer -i ../objects/cone_gold.gltf --hdr ../hdr/green_point_park_4k.hdr -o cone_gold.ppm

bin/pathtracer -i ../objects/cube_aluminium.gltf --hdr ../hdr/green_point_park_4k.hdr -o cube_aluminium.ppm
bin/pathtracer -i ../objects/cube_gold.gltf --hdr ../hdr/green_point_park_4k.hdr -o cube_gold.ppm

bin/pathtracer -i ../objects/lucy_gold.gltf --hdr ../hdr/green_point_park_4k.hdr -o lucy_gold.ppm

bin/pathtracer -i ../objects/monkey_gold.gltf --hdr ../hdr/green_point_park_4k.hdr -o monkey_gold.ppm

bin/pathtracer -i ../objects/sphere_aluminium.gltf --hdr ../hdr/green_point_park_4k.hdr -o sphere_aluminium.ppm
bin/pathtracer -i ../objects/sphere_gold.gltf --hdr ../hdr/green_point_park_4k.hdr -o sphere_gold.ppm

bin/pathtracer -i ../objects/teapot_aluminium.gltf --hdr ../hdr/green_point_park_4k.hdr -o teapot_aluminium.ppm
bin/pathtracer -i ../objects/teapot_gold.gltf --hdr ../hdr/green_point_park_4k.hdr -o teapot_gold.ppm
bin/pathtracer -i ../objects/teapot_iron.gltf --hdr ../hdr/green_point_park_4k.hdr -o teapot_iron.ppm

bin/pathtracer -i ../objects/torus_aluminium.gltf --hdr ../hdr/green_point_park_4k.hdr -o torus_aluminium.ppm
bin/pathtracer -i ../objects/torus_gold.gltf --hdr ../hdr/green_point_park_4k.hdr -o torus_gold.ppm
