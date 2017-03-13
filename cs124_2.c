#include "include/matrix.h"

#include <assert.h>
#include <time.h>

int main(void) {
	int n;
	if (scanf("%d", &n) != 1) {
			printf("You must input the dimension of the matrix.\n");
	}

	// Initialze the seed.
	srand((unsigned)time(NULL));

	printf("\nStart...\n");
	Matrix left = generate_random_matrix(n, n);
	Matrix right = generate_random_matrix(n, n);
	printf("\nGenerated matrix...\n");
	printf("\nFirst matrix:\n");
	print_matrix(left);
	printf("\nSecond matrix:\n");
	print_matrix(right);
	
	printf("\nMultplying matrix normally...\n");
	Matrix normal_result = matrix_multiply(left, right);
	printf("\nResult:\n");
	print_matrix(normal_result);

	printf("\nMultplying matrix with strassen...\n");
	Matrix strassen_result = strassen(left, right, 1000);
	printf("\nResult:\n");
	print_matrix(strassen_result);

	assert(equal(normal_result, strassen_result));

	free(normal_result.data);
	free(strassen_result.data);
}