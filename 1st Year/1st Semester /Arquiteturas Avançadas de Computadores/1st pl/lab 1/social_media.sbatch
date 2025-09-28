#!/bin/bash
#SBATCH --job-name=social_media_simulation
#SBATCH --output=output.txt
#SBATCH --error=error.txt
#SBATCH --ntasks=1             # 1 process (we use threads internally)
#SBATCH --cpus-per-task=8      # adjust threads (matches -t argument in program)
#SBATCH --time=00:05:00        # 5 minutes
#SBATCH --partition=standard   # partition name

# Load necessary modules if any (example: GCC)
module load gcc

# Compile program (if needed)
make clean
make

# Run program with arguments:
#  - number of users
#  - number of threads
./build/social_media 1000 8   # example: 1000 users, 8 threads
