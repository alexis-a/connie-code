#!/bin/bash
#SBATCH --job-name=TEST
#SBATCH --ntasks=1
#SBATCH --time=2:00:00

echo​ ​ -e​ ​ "\n##​ ​ Job $SLURM_JOB_NAME​ \n"

srun hostname
srun sleep 60
