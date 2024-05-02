#!/bin/bash
#SBATCH -J EP3-776852               # Job name
#SBATCH -p fast                     # Job partition
#SBATCH -n 1                        # Number of processes
#SBATCH -t 01:30:00                 # Run time (hh:mm:ss)
#SBATCH --cpus-per-task=40          # Number of CPUs per process
#SBATCH --output=%x.%j.out          # Name of stdout output file - %j expands to jobId and %x to jobName
#SBATCH --error=%x.%j.err           # Name of stderr output file

echo "*** GRID 2500 x 2500 ***"

echo "*** SEQUENTIAL ***"
srun singularity run container.sif laplace_seq 2500

echo "*** OPENMP ***"
echo "*** 1 THREAD - OPENMP ***"
export OMP_NUM_THREADS=1
srun singularity run container.sif laplace_omp 2500
echo "*** 2 THREADS - OPENMP ***"
export OMP_NUM_THREADS=2
srun singularity run container.sif laplace_omp 2500
echo "*** 5 THREADS - OPENMP ***"
export OMP_NUM_THREADS=5
srun singularity run container.sif laplace_omp 2500
echo "*** 10 THREADS - OPENMP ***"
export OMP_NUM_THREADS=10
srun singularity run container.sif laplace_omp 2500
echo "*** 20 THREADS - OPENMP ***"
export OMP_NUM_THREADS=20
srun singularity run container.sif laplace_omp 2500
echo "*** 40 THREADS - OPENMP ***"
export OMP_NUM_THREADS=40
srun singularity run container.sif laplace_omp 2500

echo "*** GRID 5000 x 5000 ***"

echo "*** SEQUENTIAL ***"
srun singularity run container.sif laplace_seq 5000

echo "*** OPENMP ***"
echo "*** 1 THREAD - OPENMP ***"
export OMP_NUM_THREADS=1
srun singularity run container.sif laplace_omp 5000
echo "*** 2 THREADS - OPENMP ***"
export OMP_NUM_THREADS=2
srun singularity run container.sif laplace_omp 5000
echo "*** 5 THREADS - OPENMP ***"
export OMP_NUM_THREADS=5
srun singularity run container.sif laplace_omp 5000
echo "*** 10 THREADS - OPENMP ***"
export OMP_NUM_THREADS=10
srun singularity run container.sif laplace_omp 5000
echo "*** 20 THREADS - OPENMP ***"
export OMP_NUM_THREADS=20
srun singularity run container.sif laplace_omp 5000
echo "*** 40 THREADS - OPENMP ***"
export OMP_NUM_THREADS=40
srun singularity run container.sif laplace_omp 5000