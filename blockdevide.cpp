#include "lib.h"

void smid_matrixMultiply_block(int** matrix1, int m1, int n1, int** transed, int m2, int n2,
    int** result,int BLOCK_SIZE) {
    int  iterations = BLOCK_SIZE / 8;
    __m256i d1, d2, sum;
    int* a, * b, * ret;
    int** tempres = createMatrix(BLOCK_SIZE, BLOCK_SIZE);
    for (int r1 = 0; r1 < m1; r1+=BLOCK_SIZE)
        for (int r2 = 0; r2 < n2; r2+=BLOCK_SIZE)
            for (int col = 0; col < n1; col+=BLOCK_SIZE)
            {  
                for (int i = r1; i < std::min(r1+BLOCK_SIZE, m1); i++)
                    for (int j = r2; j < std::min(r2 + BLOCK_SIZE, n2); j++)
                    {
                        //tempres[i%BLOCK_SIZE][j%BLOCK_SIZE] = vec_milti(, BLOCK_SIZE, transed[j]+col);
                         __m256i d1, d2, d3, sum;
                        a = matrix1[i]+col;
                        b = transed[j]+col;
                        sum = _mm256_setzero_si256();
                        for (int ii = 0; ii < iterations; ii++) {
                            d1 = _mm256_loadu_si256((__m256i*) & a[ii * 8]);
                            d2 = _mm256_loadu_si256((__m256i*) & b[ii * 8]);
                            d3 = _mm256_mullo_epi32(d1, d2);
                            sum = _mm256_add_epi32(d3, sum);
                        }
                        int* p = (int*)&sum;
                        tempres[i%BLOCK_SIZE][j%BLOCK_SIZE] = p[0] + p[1] + p[2] + p[3] + p[4] + p[5] + p[6] + p[7];
                    }
                for (int k = 0; k < BLOCK_SIZE; k++)
                {
                    a = result[r1 + k] + r2;
                    b = tempres[k];
                    ret = result[r1 + k] + r2;
                    sum = _mm256_setzero_si256();
                    for (int ii = 0; ii < iterations; ii++) {
                        d1 = _mm256_loadu_si256((__m256i*) & a[ii * 8]);
                        d2 = _mm256_loadu_si256((__m256i*) & b[ii * 8]);
                        sum = _mm256_add_epi32(d1, d2);
                        _mm256_storeu_si256((__m256i*)(ret + ii * 8), sum);
                    }
                    //vec_add(result[r1 + k] + r2, tempres[k],BLOCK_SIZE,result[r1 + k] +  r2);
                }
            }
}



// 使用多线程进行矩阵乘法计算
void multithread_block(int** matrix1, int m1, int n1, int** b, int m2, int n2,
    int** result,int BLOCK_SIZE)
{
    if (n1 != m2) {
        std::cout << "无法进行矩阵乘法，矩阵尺寸不匹配！\n";
        return;
    }

    int** transed = transposeMatrix(b, m2, n2);
    std::vector<std::thread> threads;
    int thread_num = 0;
    if (NUMBERS_THREAD * BLOCK_SIZE > m1) thread_num = m1/BLOCK_SIZE;
    else thread_num = NUMBERS_THREAD;
        
    int thread_rows = m1 / thread_num;
 
    for (int i = 0; i < thread_num; i++) {
        threads.emplace_back(smid_matrixMultiply_block, matrix1 + thread_rows * i, thread_rows, n1, transed, m2, n2, result + thread_rows * i,BLOCK_SIZE);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    releaseMatrix(transed, n2);
}


