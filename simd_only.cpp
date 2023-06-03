#include"lib.h"

void smid_matrixMultiply(int** matrix1,int m1 ,int n1,int** matrix2,int m2,int n2,
                          int** result ) {
    if (n1 != m2) {
        std::cout << "无法进行矩阵乘法，矩阵尺寸不匹配！\n";
        return;
    }
    int** transed = transposeMatrix(matrix2, m2, n2);
   for (int i = 0; i < m1; i++)
        for (int j = 0; j < n2; j++)
        {
            result[i][j] = vec_milti(matrix1[i],m2,transed[j]);
        }
    releaseMatrix(transed,n2);
}

