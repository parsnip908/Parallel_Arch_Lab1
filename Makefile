
CC = gcc
CFLAGS = -g -DDEBUG
OBJDIR=./obj/

OBJ= gen_matrix.o my_malloc.o
OBJS = $(addprefix $(OBJDIR), $(OBJ))


test_mm: $(OBJDIR) $(OBJS) ./obj/test_mm.o
	$(CC) $(CFLAGS) $(OBJS) ./obj/test_mm.o -o test_mm


run_debug: test_mm
	./test_mm 0 0 100

run_performance: test_mm
	./test_mm 1 0 100

$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)* test_mm
