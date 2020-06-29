#!/bin/bash
#SBATCH -J Project6code
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o project6.out
#SBATCH -e project6.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=turnesar@oregonstate.edu

# local size is b and array size is t
for b in 8 32 64 256 512
do 
    for t in 1024 64*1024 256*1024 512*1024 1024*1024 4*1024*1024 8*1024*1024 
    do
        g++ -DLOCAL_SIZE=$b -DNUM_ELEMENTS=$t -o first first.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
        ./first   
    done    
done    