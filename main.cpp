#include "func.h"
#include "lib.h"
#include <cstdlib>
#include <fstream>  // 包含文件流头文件
#include <string>   // 包含字符串处理头文件
int M1 = 2048;
int M2 = 2048;
int BLOCK_SIZE = 128;

void writeCSV_test_blocksize(const std::string& filename,int block_size) {
    std::ofstream file(filename, std::ios::app);
    int** m1 = generateRandomMatrix(M1, M2);
    int** m2 = generateRandomMatrix(M2, M2);
    int** res = createMatrix(M1, M2);
    if (file.is_open()) {
        auto startTime = std::chrono::steady_clock::now();
        multithread_block(m1, M1, M2, m2, M2, M2,res,block_size);
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        file << std::to_string(duration.count()) << std::endl;

        file.close();

        std::cout << "CSV file '" << filename << "' has been written." << std::endl;
    } else {
        std::cout << "Failed to open the file '" << filename << "'." << std::endl;
    }
}

void writeCSV_cmp(const std::string& filename){
    std::ofstream file(filename);
    if (file.is_open()) {
        for(int M1=128;M1<=8192;M1*=2){
        M2 = M1;
        int** m1 = generateRandomMatrix(M1, M1);
        int** m2 = generateRandomMatrix(M2, M2);
        int** res = createMatrix(M1, M2);
        int** res2 = createMatrix(M1, M2);
        auto startTime = std::chrono::steady_clock::now();
        multithread_block(m1, M1, M2, m2, M2, M2,res,128);
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        file << std::to_string(duration.count()) << ",";
        
        auto startTime2 = std::chrono::high_resolution_clock::now();
        multithread_wb(m1, M1, M2, m2, M2, M2, res2);
        auto endTime2 = std::chrono::high_resolution_clock::now();
        auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(endTime2 - startTime2);
        file << std::to_string(duration2.count()) << std::endl;
        std::cout << M1<<"*"<<M2 << "  has been written." << std::endl;
        releaseMatrix(m1, M1);
        releaseMatrix(m2,M2);
        releaseMatrix(res, M1);
        releaseMatrix(res2,M1);
        }
        file.close();
    }
}
int main_mmm() {

    int** m1 = generateRandomMatrix(M1, M2);
    int** m2 = generateRandomMatrix(M2, M2);
    int** res = createMatrix(M1, M2);
    int** res2 = createMatrix(M1, M2);
    int** res3 = createMatrix(M1, M2);
    std::cout<<" Block "<<std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    multithread_block(m1, M1, M2, m2, M2, M2, res,BLOCK_SIZE);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "time: " << duration.count() << " ms\n";

    std::cout<<"without Block "<<std::endl;
    startTime = std::chrono::high_resolution_clock::now();
    multithread_wb(m1, M1, M2, m2, M2, M2, res2);
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "time: " << duration.count() << " ms\n";

    std::cout<<"only_simd "<<std::endl;
    startTime = std::chrono::high_resolution_clock::now();
    smid_matrixMultiply(m1,M1, M2, m2,M2,M2,res3);
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "time: " << duration.count() << " ms\n";
    
    system("python np.py");
    return 0;
}
int main(){
    //for(int i=8;i<=512;i+=8)
      //writeCSV_test_blocksize("test_blocksize",i);
    writeCSV_cmp("cmp2.csv");
}