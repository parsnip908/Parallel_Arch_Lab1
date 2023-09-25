CC = mpicc
FLAGS = -O3 -fno-tree-vectorize -mno-avx -mno-avx2 -mno-fma -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2
CFILES = gen_matrix.c my_malloc.c MPI_mm.c

test_mm: $(CFILES)
	$(CC) $(FLAGS) $(CFILES) -o test_mm

run: test_mm
	rm -f test_mm.o* test_mm.e*
	sbatch MPI_script.sh

run_debug: test_mm
	rm -f test_mm.o* test_mm.e*
	sbatch MPI_debug.sh

run_performance: test_mm
	rm -f test_mm.o* test_mm.e*
	sbatch MPI_performance.sh

clean:
	rm -f test_mm.o* test_mm.e* test_mm
