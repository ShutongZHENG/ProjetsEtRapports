#!/bin/bash
#SBATCH --job-name=ppn
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --time=00:01:00
#SBATCH --output=ppn-%j.out
#SBATCH --error=ppn-%j.out
#SBATCH --hint=nomultithread
#SBATCH --exclusive
#RESERVATION

cd ${SLURM_SUBMIT_DIR}
 
module purge
 
module load intel/19.0.3/gcc-4.8.5
module load intel-mkl/2019.3.199/intel-19.0.3.199
module load intel-mpi/2019.3.199/intel-19.0.3.199
 
set -x
 
srun ./ppn
