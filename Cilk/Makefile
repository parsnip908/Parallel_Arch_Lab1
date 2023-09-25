CC = /work/08382/mengtian/ls6/cilk/bin/clang 
CFLAGS = -fopencilk -O3 -fno-tree-vectorize -mno-avx -mno-avx2 -mno-mmx -mno-fma -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2
CFILES = gen_matrix.c my_malloc.c CILK_mm.c

test_mm: $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) -o test_mm

run: test_mm
	rm -f test_mm.o* test_mm.e*
	sbatch CILK_script.sh

run_debug: test_mm
	rm -f test_mm.o* test_mm.e*
	sbatch CILK_debug.sh

run_performance: test_mm
	rm -f test_mm.o* test_mm.e*
	sbatch CILK_performance.sh

clean:
	rm -f test_mm.o* test_mm.e* test_mm
