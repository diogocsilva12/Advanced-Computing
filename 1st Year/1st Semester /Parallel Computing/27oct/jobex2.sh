#!/bin/sh
#SBATCH --nodes=1
#SBATCH --ntasks=10
#SBATCH --exclusive
#SBATCH --time=00:04:00
#SBATCH --partition=cpar

# Consider using SBATCH --exclusive option outside of the class
# It ensures that no other user pollutes your measurements

module load gcc/11.2.0

export OMP_NUM_THREADS=2
export OMP_PROC_BIND=true
export OMP_PLACES=threads


perf stat ./default 
perf stat ./private 
perf stat ./fstprivate 
perf stat ./lstprivate
perf stat ./reduce 


echo "Finished"
