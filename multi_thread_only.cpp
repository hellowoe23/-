#include <iostream>
#include <vector>
#include <immintrin.h>
#include <thread>
#include<random>
#define NUMBERS_THREAD 8
void matrixMultiplication(int** matrix1, int** matrix2, int rows1, int cols1, int cols2,int** result) {
    for (int i = 0; i < rows1; i++) {
        for (int k = 0; k < cols1; k++) {
            for (int j = 0; j < cols2; j++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
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


// 使用多线程进行矩阵乘法计算
void multithread_only(int** matrix1, int m1, int n1, int** matrix2, int m2, int n2,
    int** result)
{
    if (n1 != m2) {
        std::cout << "无法进行矩阵乘法，矩阵尺寸不匹配！\n";
        return;
    }
    std::vector<std::thread> threads;
    int thread_rows = m1 / NUMBERS_THREAD;
    for (int i = 0; i < NUMBERS_THREAD-1; i++) {
        threads.emplace_back(matrixMultiplication, matrix1 + thread_rows * i,matrix2,thread_rows,n1,n2,result+ thread_rows * i);
    }
    threads.emplace_back(matrixMultiplication, matrix1 + thread_rows*(NUMBERS_THREAD-1), matrix2,m1- thread_rows * (NUMBERS_THREAD - 1), n1, n2, result+thread_rows * (NUMBERS_THREAD - 1));
    for (auto& thread : threads) {
        thread.join();
    }
}

