#include <iostream>
#include <vector>
#include <immintrin.h>
#include <thread>
#include<random>
#include <cstring>
#include<cmath>




int** createMatrix(int rows, int cols) {
    int** matrix = new int* [rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols]();
        for (int j = 0; j < cols; j++)matrix[i][j] = 0;
    }

    return matrix;
}

int** generateRandomMatrix(int rows, int cols) {
    // 创建随机数引擎
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);  // 假设生成范围为 1 到 100

    // 创建二维数组
    int** matrix = new int* [rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new int[cols];
    }

    // 填充随机值
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = dis(gen);
        }
    }

    return matrix;
}

// 打印矩阵
void printMatrix(int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// 释放矩阵内存
void releaseMatrix(int** matrix, int rows) {
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int** transposeMatrix(int** matrix, int rows, int cols) {
    // 创建转置后的矩阵
    int** transposed = new int* [cols];
    for (int i = 0; i < cols; ++i) {
        transposed[i] = new int[rows];
    }

    // 进行矩阵转置
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }

    // 返回转置后的矩阵
    return transposed;
}

bool check(int** a, int** b, int rows, int cols)
{
    for (int i = 0; i < cols; i++)
        for (int j = 0; j < rows; j++)
        {
            if (a[i][j] != b[i][j])
            {
                std::cout << "error" << std::endl;
                return false;
            }
        }
    std::cout << "ok" << std::endl;
    return true;
}
int vec_milti(int* a,int lenth, int*b) {
    //使用simd指令集实现向量点乘
    int iterations = lenth / 8;//每次处理8个int
    __m256i d1, d2, d3, sum;
    sum = _mm256_setzero_si256();
    for (int i = 0; i < iterations; i++) {
        d1 = _mm256_loadu_si256((__m256i*) & a[i * 8]);
        d2 = _mm256_loadu_si256((__m256i*) & b[i * 8]);
        d3 = _mm256_mullo_epi32(d1, d2);
        sum = _mm256_add_epi32(d3, sum);
    }
    int* p = (int*)&sum;
   
    int ret = p[0]+ p[1]+ p[2] + p[3] + p[4] + p[5] + p[6] + p[7] ;
    //处理剩余的
    for (int i = iterations * 8; i < lenth; i++)
    {
        ret += a[i] * b[i];
    }

    return ret;
}

int** matrixMultiplication(int** matrix1, int** matrix2, int rows1, int cols1, int cols2) {

    int** result = createMatrix(rows1, cols2);

    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            for (int k = 0; k < cols1; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}