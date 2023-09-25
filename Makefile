
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

MPI_mm.x: $(MPIOBJDIR) $(MPIOBJS) ./mpi_obj/MPI_mm.o
	$(MPICC) $(MPIFLAGS) $(MPIOBJS) ./mpi_obj/MPI_mm.o -o MPI_mm.x

test_mm.x: $(OBJDIR) $(OBJS) ./obj/test_mm.o
	$(CC) $(CFLAGS) $(OBJS) ./obj/test_mm.o -o test_mm.x

both: MPI_mm.x test_mm.x

run_mpi: MPI_mm.x
	rm -f MPI_mm.o* MPI_mm.e*
	sbatch MPI_script.sh

run_debug: test_mm.x
	./test_mm.x 0 0 100

run_performance: test_mm.x
	./test_mm.x 1 0 100

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
