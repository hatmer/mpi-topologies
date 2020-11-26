#!/bin/bash

# usage: ./run_mapreduce.sh <maxNum> <layerSize> <numLayers>
N=$(($2*$3 + 1))
/usr/lib64/openmpi/bin/mpiexec -n $N mapreduce $1 $2 $3
