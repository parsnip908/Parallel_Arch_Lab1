# Last revised: October 22, 2021
#
# Notes:
#
#   -- Launch this script by executing
#      "sbatch milan.mpi.slurm" on a Lonestar6 login node.
#
#   -- Use ibrun to launch MPI codes on TACC systems.
#      Do NOT use mpirun or mpiexec.
#
#   -- Max recommended MPI ranks per Milan node: 128
#      (start small, increase gradually).
#
#   -- If you're running out of memory, try running
#      fewer tasks per node to give each task more memory.
#
#----------------------------------------------------

#SBATCH -J MPI_mm           # Job name
#SBATCH -o MPI_mm.o%j       # Name of stdout output file
#SBATCH -e MPI_mm.e%j       # Name of stderr error file
#SBATCH -p normal          # Queue (partition) name
#SBATCH -N 2               # Total # of nodes
#SBATCH -n 4              # Total # of mpi tasks
#SBATCH -t 00:30:00        # Run time (hh:mm:ss)
#SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH --mail-user=username@tacc.utexas.edu

# Any other commands must follow all #SBATCH directives...

# Launch MPI code...
ibrun ./MPI_mm.x 0 0 8        # Use ibrun instead of mpirun or mpiexec
