#include "lib.h"



void smid_matrixMultiply_wb(int** matrix1,int m1 ,int n1,int** transed,int m2,int n2,
                          int** result ) {
   // std::cout << "转置后：" << std::endl;
    //printMatrix(transed, n2, m2);
   for (int i = 0; i < m1; i++)
        for (int j = 0; j < n2; j++)
        {
            result[i][j] = vec_milti(matrix1[i],m2,transed[j]);
        }
   
}



// 使用多线程进行矩阵乘法计算
void multithread_wb(int** matrix1, int m1, int n1, int** matrix2, int m2, int n2,
    int** result)
{
    if (n1 != m2) {
        std::cout << "无法进行矩阵乘法，矩阵尺寸不匹配！\n";
        return;
    }
    int** transed = transposeMatrix(matrix2, m2, n2);
    std::vector<std::thread> threads;
    int thread_rows = m1 / NUMBERS_THREAD;
    for (int i = 0; i < NUMBERS_THREAD-1; i++) {
        threads.emplace_back(smid_matrixMultiply_wb, matrix1 + thread_rows * i,thread_rows,n1 , transed ,m2,n2,result+ thread_rows * i);
    }
    threads.emplace_back(smid_matrixMultiply_wb, matrix1 + thread_rows*(NUMBERS_THREAD-1), m1- thread_rows * (NUMBERS_THREAD - 1), n1, transed, m2, n2, result+thread_rows * (NUMBERS_THREAD - 1));
    for (auto& thread : threads) {
        thread.join();
    }
    releaseMatrix(transed, n2);
}
