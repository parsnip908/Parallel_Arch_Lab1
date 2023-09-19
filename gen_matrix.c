#include<stdio.h>
#include<stdlib.h>
#include "gen_matrix.h"

int matrix_dimension_size = -1;
static int num_arg_matrices;
int called_p[8];     // a maximum of 8 arrays per test set

void double_call_check(int matrix_num) {
  static int first_p = 1;

  if (first_p) {
    int a;
    for (a = 0; a < 8; ++a) called_p[a] = 0;
    first_p = 0;
  }

  // double call check
  if (called_p[matrix_num]) {
    printf("tsk, tsk, you called me twice when you should have only called once!\n");
    exit(1);
  } else {
    called_p[matrix_num] = 1;
  }
}

double gen_one_element(int test_set,
		       int matrix_num,
		       int x,
		       int y) {
  switch(test_set) {
  case 0:
    switch(matrix_num) {
    case 0:  // generate from x and y
    case 1:  // 
      return ((double)(x + 1)/(y + 1));
	  
    case 2:  // unity C
      if (x == y) {
	return(1.0);
      } else {
	return(0.0);
      }
      break;
	  
    default:
      printf("error, don't know test_set = %d, matrix_num = %d\n", test_set, matrix_num);
      exit(1);
    }
    break;

  case 1:
    switch(matrix_num) {
    case 0:  // all unity
    case 1:  
    case 2:  
    case 3:  
    case 4:  
    case 5:  
    case 6:  
    case 7:  
      if (x == y) {
	return(1.0);
      } else {
	return(0.0);
      }
      break;
      
    default:
      printf("error, don't know test_set = %d, matrix_num = %d\n", test_set, matrix_num);
      exit(1);

    }
    break;
    
  }
}


double *gen_sub_matrix(int pid,        // the process id of the job calling.  This should be a number between 0 and num processes - 1
		       int test_set,   // the test set command line argument
		       int matrix_num, // the matrix number [0, num_arg_matrices - 1] 
		       double *result, // a pointer to storage for the flattened array of the requested sub matrix
		       int x_lo,       // the starting x value of the requested sub matrix
		       int x_hi,       // the ending x value of the requested sub matrix
		       int x_stride,   // the stride in the x dimension
		       int y_lo,       // the starting y value of the requested sub matrix
		       int y_hi,       // the ending y value of the requested sub matrix
		       int y_stride,   // the stride in the y dimension
		       int row_major_p)// whether the returned sub matrix is in row_major or column_major order
{
  int i = 0;
  int y;
  int x;

  double_call_check(matrix_num);

  // bounds check
  if ((matrix_dimension_size - 1) < x_hi) {
    printf("error: x_hi is too large\n");
    exit(1);
  }

  if ((matrix_dimension_size - 1) < y_hi) {
    printf("error: y_hi is too large\n");
    exit(1);
  }

  if (row_major_p) {
    for (y = y_lo; y <= y_hi; y += y_stride) {
      for (x = x_lo; x <= x_hi; x += x_stride) {
	result[i++] = gen_one_element(test_set, matrix_num, x, y);
      }
    }
  } else { // column major
    for (x = x_lo; x <= x_hi; x += x_stride) {
      for (y = y_lo; y <= y_hi; y += y_stride) {
	result[i++] = gen_one_element(test_set, matrix_num, x, y);
      }
    }
  }

  return(result);
}

double *gen_sub_matrix_array_spec(int pid, int test_set, int matrix_num, double *result, pair_t *pairs) {
  int i = 0;

  double_call_check(matrix_num);

  while (pairs[i].x >= 0) {
    switch(matrix_num) {
    case 0:
    case 1:
    case 2:
      result[i++] = ((pairs[i].x == pairs[i].y) ? 1.0 : 0.0);
      break;
    default:
      return(NULL);
    }
  }
}

// returns num_arrays
int init_gen_sub_matrix(int test_set) {
  // this makes it more convenient to change the number of arrays
  switch(test_set) {
  case 0:
    num_arg_matrices = 3; break;
  case 1:
    num_arg_matrices = 8; break;
  default:
    printf("we only have 3 tests, numbered 0, 1, and 2\n");
    exit(1);
  }
  return(num_arg_matrices);
}
