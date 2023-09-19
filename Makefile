test_mm: test_mm.c gen_matrix.c my_malloc.c gen_matrix.h my_malloc.h
	gcc -g -DDEBUG test_mm.c gen_matrix.c my_malloc.c -o test_mm

run_debug:
	./test_mm 0 0 100

run_performance:
	./test_mm 1 0 100

clean:
	rm *~; rm *.exe
