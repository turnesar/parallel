#!/bin/bash
#SBATCH -J Project67ode
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o project7b.out
#SBATCH -e project7b.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=turnesar@oregonstate.edu

# local size is b 
for b in 64
do 
    g++ -DLOCAL_SIZE=$b  -o project7b project7bopencl.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
    ./project7b     
done    