#include <iostream>
#include <vector>
#include <immintrin.h>
#include <thread>
#include<random>
#include <cstring>
#include<cmath>
#define NUMBERS_THREAD  8

int vec_milti(int* a,int lenth, int*b);
int** createMatrix(int rows, int cols);
int** generateRandomMatrix(int rows, int cols);
void printMatrix(int** matrix, int rows, int cols);
void releaseMatrix(int** matrix, int rows);
int** transposeMatrix(int** matrix, int rows, int cols);
bool check(int** a, int** b, int rows, int cols);
int** matrixMultiplication(int** matrix1, int** matrix2, int rows1,  int cols1, int cols2);