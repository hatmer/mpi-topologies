#!/bin/bash

# usage: ./run_sieve.sh <maxNum> <numProcesses>

#export OMPI_CXX='g++ -std=c++0x'
#mpic++ sieve.cpp -o sieve
#/usr/lib64/openmpi/bin/mpiexec -n $2 --hostfile hosts sieve $1 $2
/usr/lib64/openmpi/bin/mpiexec -n $2 sieve $1 $2

