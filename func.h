
void smid_matrixMultiply_wb(int** matrix1,int m1 ,int n1,int** transed,int m2,int n2,int** result );
void multithread_wb(int** matrix1, int m1, int n1, int** matrix2, int m2, int n2,int** result);
int** padMatrix(int** matrix, int &rows, int cols, int block_size);
void smid_matrixMultiply_block(int** matrix1, int m1, int n1, int** transed, int m2, int n2,int** result,int BLOCK_SIZE);
void multithread_block(int** a, int m1, int n1, int** b, int m2, int n2,int** result,int BLOCK_SIZE);
void smid_matrixMultiply(int** matrix1,int m1 ,int n1,int** matrix2,int m2,int n2,int** result );
