Unzip file and then 
for openCL:

	go into the opencl directory

	chmod +x project7bclscript.bash	to make it executable.  

	On DGX:

		sbatch project7bclscript.bash

	project7b.out contains the printed results

for openMP:

	This automatically runs for 1 and 8 threads with SIMD and no SIMD (so 4 times). 
	The array output is commented out right now.

	go into the openmp_simd directory
	
	chmod +x project7bopenscript.bash
	
	On flip:
	
		project7bopenscript.bash


graphs_project7b.xslx has the graphs/tables 


