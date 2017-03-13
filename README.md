# Matrix Multiplication in C

We implement a cache-optmized version of standard matrix multiplication (O(n^3)).

We also implement an optimized version of Strassen's matrix multiplication algorithm (O(n^(2+\log_8(7)))). 

We then compare the two algorithms, and note that for small enough $n$, standard matrix multiplication is faster than strassen's. Using this information, we optimize strassen's algorithm to achieve an optimal matrix multiplication algorithm at all dimensions.

## Author
Luis Perez
