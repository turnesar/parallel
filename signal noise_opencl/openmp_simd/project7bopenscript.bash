#!/bin/bash

# number of threads is t and number of threads is t
for t in 1 8 
do 
    echo THREADS = $t
    for f in 0 1 
    do
        echo SIMDFLAG = $f
        g++ -DSIMDFLAG=$f -DNUMT=$t project7bopenmp.cpp -o project7bopenmp -lm -fopenmp
        ./project7bopenmp
    done    
done    
