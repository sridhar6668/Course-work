#!/bin/bash
#SBATCH -J new_randomised_cc        			# Job Name
#SBATCH -o new_randomised_cc.o%j    			# Output and error file name (%j expands to jobID)
#SBATCH -n 16           						# Total number of mpi tasks requested
#SBATCH -p normal  								# Queue (partition) name -- normal, development, etc.
#SBATCH -t 03:00:00 				    		# Run time (hh:mm:ss) - 3 hours
#SBATCH --mail-user=ksanjeev@cs.stonybrook.edu
#SBATCH --mail-type=begin  						# email me when the job starts
#SBATCH --mail-type=end    						# email me when the job finishes

export PATH=$PATH:$WORK/cilk/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORK/cilk/lib64

./new_randomised_cc >> new_randomised_cc.output
