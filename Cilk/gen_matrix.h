//
// gen_matrix.h
//   Derek Chiou, Sep. 18, 2005

extern int matrix_dimension_size;

double *gen_sub_matrix(int pid, int test_set, int array_num, double *result, 
		       int x_lo, int x_hi, int x_stride, int y_lo, int y_hi, int y_stride,
		       int row_major_p);

typedef struct {
  int x;
  int y;
} pair_t;

double *gen_sub_matrix_array_spec(int pid, int test_set, int matrix_num, double *result, pair_t *pairs);

int init_gen_sub_matrix(int test_set);   
