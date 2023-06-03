# 矩阵优化

本项目利用avx中simd指令集，多线程、cache访存优化等方法，完成对矩阵乘法算法的优化。

实验环境：

cpu Inter Core i5 1035G1,4核8线程。

gcc 8.1.0版本，O-0，O3优化下，对于两个1024*1024方阵的相乘



优化结果：
相比于一般的矩阵乘法实现，

| 优化方法                                    | 时间（编译器不优化） | 时间（O3优化） |
| ------------------------------------------- | -------------------- | -------------- |
| 不进行任何优化                              | 6000ms               | 1100ms         |
| 仅访存优化                                  | 4200ms               | 200ms          |
| 使用simd指令集（256bit）+访存优化           | 780ms                | 168ms          |
| 单使用多线程(8线程)+cache访存优化           | 400ms                | 80ms           |
| simd+多线程+cache访存优化+循环展开+矩阵分块 | 195ms                | 35ms           |
| python  numpy库实现                         |                      | 20ms           |

下面介绍这几种优化的实现方法：

## 1、不进行任何优化

对于矩阵 乘法的基础实现用到的主要内容是三层嵌套循环,可以参考如下代码

```c++
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
```

对于上述基础实现的代码，虽然逻辑上是正确的，但是可以观察到三层循环的遍历顺序为i，j,k，对于matrix2可以看到是按列遍历，我们知道对于多维数组而言，按行遍历的速度是远高于按列进行遍历的（按行遍历地址空间是连续的，由于程序的局部原理）因此，对于这段代码最简单的优化方式便可以得知了，那便是改变程序i,j,k的遍历顺序。

## 2、cache访存优化

 由上文的分析我们得知，要改变i,j,k的遍历顺序。同样，这也有很多种方法进行改变——(i,k,j)、（k,i,j）、（k,j,i）....... 但是我们要记住我们这样优化的目的是什么，我们为什么要改变这个遍历顺序，为了尽可能让矩阵按行进行连续访问 i在k前，i在j前，k在j前，满足这三个条件，显然最优的遍历规则应该是i,k,j。

因此优化只要将k,j的循环顺序调换即可。

同样，我们还有另一种方法可以达到类型的效果：就是在matrix2进行乘法之前,先将其转置，之后再进行乘法。

```c++

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
```

经过测试，访存优化后在编译器不优化的条件下，提速约1.5倍。

## 3、使用smid指令集+访存优化

SIMD（Single Instruction Multiple Data）指令集，指单指令多数据流技术，可用一组指令对多组数据通进行并行操作。简单来说，可以用一条指令处理多条运算。使用smid指令之前需要查看自己的cpu是否支持。

这次使用了256比特的simd指令集进行优化即一条指令可以完成8个数据乘法运算，下面展示了使用smid指令集完成向量点乘运算，基于该运算便可以完成矩阵乘法。

代码如下：

```c++
int vec_milti(int* a,int lenth, int*b) {
    //使用simd指令集实现向量点乘
    int iterations = lenth / 8;//每次处理8个int
    __m256i d1, d2, d3, sum;
    sum = _mm256_setzero_si256();
    int i = 0;
    for (; i < iterations-1; i+=2) {
        d1 = _mm256_loadu_si256((__m256i*) & a[i * 8]);
        d2 = _mm256_loadu_si256((__m256i*) & b[i * 8]);
        d3 = _mm256_mullo_epi32(d1, d2);
        sum = _mm256_add_epi32(d3, sum);
        //进行二级循环展开
        d1 = _mm256_loadu_si256((__m256i*) & a[(i+1) * 8]);
        d2 = _mm256_loadu_si256((__m256i*) & b[(i+1) * 8]);
        d3 = _mm256_mullo_epi32(d1, d2);
        sum = _mm256_add_epi32(d3, sum);
    }
    for(;i<iterations;i++){
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

```

完整矩阵乘法代码见仅simd优化.cpp。

优化结果：使用smid指令后优化了6倍左右。

## 4、使用多线程优化


多线程优化的本质是利用cpu多核的并行计算，对于矩阵乘法结果中的每一个的计算都是独立的，因此我们基本不需要考虑线程之间的同步问题，我们唯一需要解决的问题是计算任务的分配问题。如何让计算任务均匀的分配，这里我选择了一个比较简单的方法——就是将矩阵1按行进行分块，分块后直接与矩阵二相乘（这样的做法优点是比较简单，缺点是程序的局部性访问较差。）经过此步骤的优化后，在8线程运行的条件下程序的性能提高了大约6倍左右。

## 5、矩阵进行分块运算

分块的目的是为了增加程序的局部性原理，当计算较大的矩阵时，由于矩阵大小超出了cache大小，会导致cache命中率降低，为了优化在矩阵规模较大时的程序速度问题，我们可以将大矩阵划分为小的矩阵块来计算。以此增加运行速度，通过测试，在本主机上分块大小最优为128*128大小的块。

在8线程，矩阵大小为4096*4096大小测试数据如下：

|  分块大小    |   时间/ms   |
| ---- | ---- |
|  8*8  |  11476    |
|   16*16   |   6913   |
|  32*32  |   4896   |
|  64*64  |   3769   |
| 128*128 |  3649    |
|  256*256  |     4503 |
|  512*512  |    6079  |

### 综合对比结果

将numpy实现，不分块的实现，以及分块实现进行对比：

从图中可以看到，分块后的算法在进行大规模的矩阵运算时相比分块前的算法有了较大优化，但是与numpy实现还是有较大差距。

![](https://github.com/hellowoe23/-/blob/main/figure/Figure_1.png)

## 仍可以努力的点

对于我们的矩阵优化策略，已经算是较为完备的，但是还有几个点可以有优化的空间。

1. 使用strassen算法，对矩阵的计算复杂度进行优化。
2. 在进行多线程运算时，采用更均匀的矩阵划分方法。
3. 在进行矩阵存储时，采用一维的向量进行存储，以及运算。
4. 进一步使用512比特的simd指令集。
5. 在算法中使用寄存器存储值进行运算。

..........

总之，还有很多可以努力的空间，希望大家可以在此基础上进一步优化。

​                                                                                                                                           ------SDU amx
