#include "../include/matrix.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Values in our input matrices must be in [-BOUND, BOUND].
#define BOUND (1 << 2) // Must be a power of 2 to preserve randomness.

// Generates a random int in [-(2^31-1), 2^31 -1].
// Assumes generator has been seeded.
int generate_random_int32() {
	int res = rand() % BOUND;
	float p = ((float)rand()/(float)(RAND_MAX));
	return (p < 0.5) ? res : -res;
}

/***** Internal element-wise ops to be performed on a matrix. *****/
uint64_t add(uint64_t a, uint64_t b){
	return a + b;
}
uint64_t sub(uint64_t a, uint64_t b) {
	return a - b;
}
uint64_t sub_reverse(uint64_t a, uint64_t b) {
	return b - a;
}

/***** Internal padding functions *****/
// Pading for odd-dimension matrix in both dimensions. Only one copy.
Matrix pad_both(Matrix input) {
	assert(input.nrows % 2 == 1);
	assert(input.ncols % 2 == 1);
	Matrix output = create_matrix(input.nrows + 1, input.ncols + 1);
	for (int i = 0; i < input.nrows; ++i) {
		memcpy(&INDEX(output, i, 0),
					 &INDEX(input, i ,0),
			     sizeof(uint64_t) * input.ncols);
		INDEX(output, i, input.ncols) = 0;
	}
	memset(&INDEX(output, output.nrows - 1, 0), 0, sizeof(uint64_t) * output.ncols);
	return output;
}
// Padding for odd column.
Matrix pad_column(Matrix input) {
	assert(input.ncols % 2 == 1);
	Matrix output = create_matrix(input.nrows, input.ncols + 1);
	for (int i = 0; i < input.nrows; ++i) {
		memcpy(&INDEX(output, i, 0),
					 &INDEX(input, i ,0),
			     sizeof(uint64_t) * input.ncols);
		INDEX(output, i, input.ncols) = 0;
	}
	return output;
}
// Padding for odd row.
Matrix pad_row(Matrix input) {
	assert(input.nrows % 2 == 1);
	Matrix output = create_matrix(input.nrows + 1, input.ncols);
	memcpy(&INDEX(output, 0, 0),
				 &INDEX(input, 0, 0),
				 sizeof(uint64_t) * input.nrows * input.ncols);
	memset(&INDEX(output, output.nrows - 1, 0), 0, sizeof(uint64_t) * output.ncols);
	return output;
}

/**** Internal `efficient` methods ****/
// Efficient copy into a location in-memory.
void efficient_copy(Matrix source, Matrix dest) {
	assert(source.ncols == dest.ncols);
	assert(source.nrows == dest.nrows);
	memcpy(&INDEX(dest, 0, 0),
				 &INDEX(source, 0, 0),
				 sizeof(uint64_t) * dest.nrows * dest.ncols);
}
// Perform binary `op` on matrices.
// Results are stored in the `output` matrix to reuse space.
// WARNING: Do not pass same matrix for both. Make a copy first.
void efficient_perform_op(elementOp op, Matrix unchanged, Matrix output) {
	assert(unchanged.nrows == output.nrows);
	assert(unchanged.ncols == output.ncols);
	for (int i = 0; i < output.ncols; ++i){
		for (int j = 0; j < output.ncols; ++j){
			INDEX(output, i, j) = op(INDEX(output, i, j), INDEX(unchanged, i, j));
		}
	}
}

/**** Other internal methods *****/
// Creates a half-sized view of `input` as a matrix starting at INDEX(input, i, j).
// Assumes input matrix is even dimensions. Fails if i, j would lead to a smaller matrix.
Matrix view(Matrix input, int i, int j) {
	assert(input.nrows % 2 == 0);
	assert(input.ncols % 2 == 0);
	assert(i <= input.nrows / 2);
	assert(j <= input.ncols / 2);
	Matrix view;

	view.nrows = input.nrows / 2;
	view.ncols = input.ncols / 2;
	view.data = &INDEX(input, i, j);

	return view;
}

/***** Implementation of public methods *****/
bool equal(Matrix a, Matrix b) {
	if (a.nrows != b.nrows) return false;
	if (a.ncols != b.ncols) return false;
	for (int i = 0; i < a.nrows; ++i){
		for (int j = 0; j < a.ncols; ++j) {
			if (INDEX(a, i, j) != INDEX(b, i, j)) {
				return false;
			}
		}
	}
	return true;
}
Matrix create_matrix(int rows, int cols) {
	Matrix m;
	m.nrows = rows;
	m.ncols = cols;
	m.data = malloc(sizeof(uint64_t) * rows * cols);
	return m;
}
Matrix create_zero_matrix(int rows, int cols) {
	Matrix m;
	m.nrows = rows;
	m.ncols = cols;
	m.data = calloc(rows * cols, sizeof(uint64_t));
	return m;
}
void print_matrix(Matrix m){
	printf("\n===============================\n");
	printf("columns: %d\n", m.ncols);
	printf("rows: %d\n", m.nrows);
	for (int i = 0; i < m.nrows; ++i) {
		for (int j = 0; j < m.ncols - 1; ++j) {
			printf("%lld,", INDEX(m, i, j));
		}
		printf("%lld\n", INDEX(m, i, m.ncols - 1));
	}
	printf("==================================\n");
}
Matrix generate_random_matrix(int rows, int cols) {
	Matrix m = create_matrix(rows, cols);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			INDEX(m,i,j) = generate_random_int32();
		}
	}
	return m;
}
// TODO(nautilik): Can we reuse space?
Matrix matrix_multiply(Matrix m, Matrix n) {
	assert(m.ncols == n.nrows);
	Matrix prod = create_zero_matrix(m.nrows, n.ncols);
	for (int i = 0; i < prod.nrows; ++i){
		for (int k = 0; k < m.ncols; ++k) {
			for (int j = 0; j < prod.ncols; ++j) {
				INDEX(prod, i, j) += (INDEX(m, i, k) * INDEX(n, k, j));
			}
		}
	}
	return prod;
}
Matrix copy(Matrix a){
	Matrix copy = create_matrix(a.nrows, a.ncols);
	efficient_copy(a, copy);
	return copy;
}
Matrix perform_op(elementOp op, Matrix a, Matrix b) {
	Matrix res = copy(b);
	efficient_perform_op(op, a, res);
	return res;
}
// TODO(nautilik): Re-implement to re-use space instead of copy on recursive calls.
Matrix strassen(Matrix left, Matrix right, int cutoff) {
	// TODO(nautilik): Re-implement to accomodate dimensions that are not the same.
	assert(left.nrows == left.ncols);
	assert(left.nrows == right.nrows);
	assert(left.ncols == right.ncols);

	int n = left.nrows;
	// Fallback to simpler algorithm.
	if (n <= cutoff) {;
		return matrix_multiply(left, right);
	}
	// If we need to pad both dimensions, it's more efficient to do so in one go.
	if (left.nrows % 2 != 0 && left.ncols %2 != 0) {
		Matrix m_padded = pad_both(left);
		Matrix result = strassen(m_padded, right, cutoff);
		free(m_padded.data);
		return result;
	} else if (left.nrows % 2 != 0) {
		Matrix m_padded = pad_row(left);
		Matrix result = strassen(m_padded, right, cutoff);
		free(m_padded.data);
		return result;
	} else if (left.ncols % 2 != 0) {
		Matrix m_padded = pad_column(left);
		Matrix result = strassen(m_padded, right, cutoff);
		free(m_padded.data);
		return result;
	} else if (right.nrows % 2 != 0 || right.ncols % 2 != 0) {
		// Swap since we need to pad n now.
		return strassen(right, left, cutoff);
	}

	// These matrices are simply views. Memory still belongs to input.
	Matrix a = view(left, 0, 0);
	Matrix b = view(left, 0, n / 2);
	Matrix c = view(left, n / 2, 0);
	Matrix d = view(left, n / 2, n / 2);

	Matrix e = view(right, 0, 0);
	Matrix f = view(right, 0, n / 2);
	Matrix g = view(right, n / 2, 0);
	Matrix h = view(right, n / 2, n / 2);

	// These matrices own their own memory, an so do intermediates, so we must free.
	Matrix tmp = perform_op(sub, f, h);
	Matrix p1 = strassen(a, tmp, cutoff);
	free(tmp.data);
	tmp = perform_op(add, a, b);
	Matrix p2 = strassen(tmp, h, cutoff);
	free(tmp.data);
	tmp = perform_op(add, c, d);
	Matrix p3 = strassen(tmp, e, cutoff);
	free(tmp.data);
	tmp = perform_op(sub, g, e);
	Matrix p4 = strassen(d, tmp, cutoff);
	free(tmp.data);
	Matrix tmp1 = perform_op(add, a, d);
	Matrix tmp2 = perform_op(add, e, h);
	Matrix p5 = strassen(tmp1, tmp2, cutoff);
	free(tmp1.data);
	free(tmp2.data);
	tmp1 = perform_op(sub, b ,d);
	tmp2 = perform_op(add, g, h);
	Matrix p6 = strassen(tmp1, tmp2, cutoff);
	free(tmp1.data);
	free(tmp2.data);
	tmp1 = perform_op(sub, a, c);
	tmp2 = perform_op(add, e, f);
	Matrix p7 = strassen(tmp1, tmp2, cutoff);
	free(tmp1.data);
	free(tmp2.data);

	// Make output matrix. We add directly onto the output.
	Matrix output = create_matrix(n, n);
	
	Matrix out_view = view(output, 0, 0);
	efficient_copy(p5, out_view);
	efficient_perform_op(add, p4, out_view);
	efficient_perform_op(sub_reverse, p2, out_view);
	efficient_perform_op(add, p6, out_view);

	out_view = view(output, 0, n / 2);
	efficient_copy(p1, out_view);
	efficient_perform_op(add, p2, out_view);

	out_view = view(output, n / 2, 0);
	efficient_copy(p3, out_view);
	efficient_perform_op(add, p4, out_view);

	out_view = view(output, n / 2, n / 2);
	efficient_copy(p1, out_view);
	efficient_perform_op(add, p5, out_view);
	efficient_perform_op(sub_reverse, p3, out_view);
	efficient_perform_op(add, p7, out_view);

	return output;
}
