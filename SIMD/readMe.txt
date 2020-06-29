Unzip file and then 

chmod +x project4script

to make it executable.
Running the script gives results to STDOUT.  Redirect to file with

project4script > file.txt

For multithreading I made a separate file and script

chmod +x project4scriptextra

to make it executable.
Running the script gives results to STDOUT.  Redirect to file with

project4scriptextra > file.txt

Of note you do have to manually change the SIMD flag in this file. If you want results appended 
to the same file, change the SIMDFLAG and rerun:

project4scriptextra >> file.txt

results.txt are results used in graphs on pdf
resultsextra.txt are results for multiple threads
graphs_project4.xslx has the graphs/tables and formulas used


