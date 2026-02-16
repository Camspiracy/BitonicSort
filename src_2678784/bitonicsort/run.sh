#!/bin/bash

pe=8
size=33554432
echo ---------serial bitonic sort has started----------
./bitonic $size |& tee -a terminal_output.out
echo ---------serial bitonic sort is done-------------
echo

echo ---------omp bitonic sort has started-----
./bitonic_omp $size $pe |& tee -a terminal_output.out
echo ---------omp bitonic sort is done--------
echo

echo -----------mpi bitonic sort has started----------------
mpiexec -n $pe ./bitonic_mpi $size |& tee -a terminal_output.out
echo -----------mpi bitonic sort is done--------------------
echo
