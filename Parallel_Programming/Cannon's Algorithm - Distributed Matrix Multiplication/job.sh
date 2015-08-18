#!/bin/bash
#SBATCH -J can1_2a_m        			# Job Name
#SBATCH -o cannon.o%j    			# Output and error file name (%j expands to jobID)
#SBATCH -n 1        						# Total number of mpi tasks requested
#SBATCH -N 1
#SBATCH -p normal  								# Queue (partition) name -- normal, development, etc.
#SBATCH -t 2:00:00 				    		# Run time (hh:mm:ss) - 1.5 hours
#SBATCH --mail-user=sridhar.periasami@stonybrook.edu
#SBATCH --mail-type=begin  						# email me when the job starts
#SBATCH --mail-type=end    						# email me when the job finishes
export PATH=$PATH:$WORK/cilk/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORK/cilk/lib64
export CILK_NWORKERS=16
ibrun tacc_affinity ./cannon >> cannon_output_1
