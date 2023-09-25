
CC = gcc
CFLAGS = -g -DDEBUG
OBJDIR=./obj/

MPICC = mpicc
MPIFLAGS = -O3 -fno-tree-vectorize -mno-avx -mno-avx2 -mno-fma -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2
MPIOBJDIR=./mpi_obj/
# MPIOBJ= gen_matrix.o my_malloc.o


OBJ= gen_matrix.o my_malloc.o
OBJS = $(addprefix $(OBJDIR), $(OBJ))
MPIOBJS = $(addprefix $(MPIOBJDIR), $(OBJ))

test_mm: $(MPIOBJDIR) $(MPIOBJS) ./mpi_obj/MPI_mm.o
	$(MPICC) $(MPIFLAGS) $(MPIOBJS) ./mpi_obj/MPI_mm.o -o test_mm

original_test_mm: $(OBJDIR) $(OBJS) ./obj/test_mm.o
	$(CC) $(CFLAGS) $(OBJS) ./obj/test_mm.o -o original_test_mm

both: test_mm original_test_mm

run_mpi: test_mm
	rm -f test_mm.o* test_mm.e*
	sbatch MPI_script.sh

run_debug: original_test_mm
	./original_test_mm 0 0 100

run_performance: original_test_mm
	./original_test_mm 1 0 100

$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MPIOBJDIR)%.o: %.c
	$(MPICC) $(MPIFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(MPIOBJDIR):
	mkdir -p $(MPIOBJDIR)

clean:
	rm -f $(MPIOBJDIR)* $(OBJDIR)* *.x
