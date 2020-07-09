gltffilename="ballEnriched_aluminium.gltf"
background="../../hdr/green_point_park_4k.hdr"
outputfile="rotation"
rm ${outputfile}*
displacementmap="../../materials/paper_displacement.bmp"
magnitude=0.1
UVmap=sphere
for i in `seq 170 -50 20` ; do ../bin/pathtracer -i ../../objects/$gltffilename -H $background -o ${outputfile}X$i.ppm -n 1,0,0 -p $i -d $displacementmap -m $magnitude -U $UVmap; done
for i in `seq 170 -50 20` ; do ../bin/pathtracer -i ../../objects/$gltffilename -H $background -o ${outputfile}Y$i.ppm -n 0,1,0 -p $i -d $displacementmap -m $magnitude -U $UVmap; done
for i in `seq 170 -50 20` ; do ../bin/pathtracer -i ../../objects/$gltffilename -H $background -o ${outputfile}Z$i.ppm -n 0,0,1 -p $i -d $displacementmap -m $magnitude -U $UVmap; done

