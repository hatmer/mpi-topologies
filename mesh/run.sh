#!/bin/bash

# usage: ./run_mesh.sh <maxNum> <layerSize> <numLayers>
N=$(($2*$3))
/usr/lib64/openmpi/bin/mpiexec -n $N mesh $1 $2 $3
