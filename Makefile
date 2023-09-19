
CC = gcc
CFLAGS = -g -DDEBUG
OBJDIR=./obj/
OBJ= test_mm.o gen_matrix.o my_malloc.o

OBJS = $(addprefix $(OBJDIR), $(OBJ))


test_mm: $(OBJDIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o test_mm.exe

run_debug:
	./test_mm 0 0 100

run_performance:
	./test_mm 1 0 100

$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm $(OBJDIR)*; rm *.exe
