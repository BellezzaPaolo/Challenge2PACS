# Challenge2PACS

This library, in the namespace algebra, implements the sparse matrix class storing in 2 ways:
- **Dynamic or uncompressed**: uses a map storing the elements in row-wise ordering or column-wise ordering.
- **compressed**:uses compressed sparse row () or compressed sparse column().
To switch from row-wise and column-wise ordering go in the 8th row of main.cpp.

## Usage
To use the code simply write int he terminal:
```bash
make
```
and then:
```bash
./main
```

The makefile also has this 2 targets:
- one to clean all the object and executable:
```bash
make clean
```
- one to optimize the code compilation:
```bash
make optimize
```

## Performance
the use of compressed form of sparse matrix for storing matrixes makes the computation faster. Here the results in microseconds obtained in 10 tests made performing a matrix-vecto product where the matrix is initialized as [here](https://math.nist.gov/MatrixMarket/data/Harwell-Boeing/lns/lnsp_131.html) (131x131 matrix with 536 entries non-zeros) and multiplied by a vector of ones.

| Test | Compressed | Uncompressed |
|:----:|:----------:|:------------:|
| 1    | 18         | 439          |
| 2    | 19         | 482          |
| 3    | 18         | 582          |
| 4    | 19         | 436          |
| 5    | 18         | 446          |
| 6    | 19         | 537          |
| 7    | 29         | 479          |
| 8    | 20         | 511          |
| 9    | 37         | 537          |
| 10   | 19         | 438

Getting an average:

| Matrix | mean time (microsecond) |
|:------:|:-----------------------:|
| Compressed | 21,6        |
| Uncompressed | 498,7       |


## Features
The class has the following public methods:
- 2 constructors one takes in input the number of rows and the number of columns, the other a file to initialize the matrix
- compress() a method that pass the matrix from uncompressed to compressed
- uncompress() a method that pass from compressed form to the uncompressed one
- is_compressed() that checks if the matrix is compressed or not
- the overload of the call operator (const and non-const version) to acces the element
- resize(size_t R,size_t C) changes the MAtrix dimensions
- display() that show how is tored the matrix
- norm() a method that computes the norm of the matrix (One norm, Infinity norm or Frobenius norm)

and 4 friend functions:
- operator*(Matrix<T,S>,vector<T>) that performs the matrix-vector multiplication
- print(Matrix<T,S>) that prints at the terminal the matrix
- read(string,Matrix<T,S>) reads the matrix from the file passed as string