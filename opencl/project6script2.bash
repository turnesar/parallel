#!/bin/bash
#SBATCH -J Project6code2
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o project6_2.out
#SBATCH -e project6_2.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=turnesar@oregonstate.edu

# number of local work size is b and array size is t
for b in 32 64 96 128 256 
do 
    for t in 1024 64*1024 256*1024 512*1024 1024*1024 4*1024*1024 8*1024*1024 
    do
        g++ -DLOCAL_SIZE=$b -DNUM_ELEMENTS=$t -o second second.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
        ./second   
    done    
done    