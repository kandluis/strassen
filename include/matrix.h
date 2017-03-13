#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INDEX(MAT,ROW,COL) (MAT.data[(int64_t)MAT.ncols * (int64_t)ROW + (int64_t)COL])

// A square matrix of integers owned.
// .data[ncols * i + j] is the A[i][j]'th entry of the matrix.
typedef struct {
	int nrows;
	int ncols;
	int64_t* data;  // Points to the data owned by this matrix.
} Matrix;
// Function pointer to generalized code.
typedef uint64_t (*elementOp)(uint64_t, uint64_t);

// Generates a random integer matrix with values in [-(2^31-1), 2^31-1].
// Bounded by RAND_MAX.
Matrix generate_random_matrix(int rows, int cols);
// Allocates new memory for a matrix of dimension rows * cols.
// The memory is not guaranteed to be initialized.
Matrix create_matrix(int rows, int cols);
// Same as below but returns a matrix with all zero elements.
Matrix create_zero_matrix(int rows, int cols);
// Prints a matrix to std out.
void print_matrix(Matrix m);
// Creates a copy of a.
Matrix copy(Matrix a);
// True if a and b are element-wise equal.
bool equal(Matrix a, Matrix b);
// Perform binary `op` on matrices.
Matrix perform_op(elementOp op, Matrix a, Matrix b);
// Computes m*n in a new matrix.
Matrix matrix_multiply(Matrix m, Matrix n);
// Matrix multiplication using strassen's algorithm.
Matrix strassen(Matrix left, Matrix right, int cutoff);