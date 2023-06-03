import numpy as np
import time
 
i = 128
# 生成两个随机矩阵
while i<=8192:
    matrix1 = np.random.randint(i, i)
    matrix2 = np.random.randint(i, i)

    # 计时开始
    start_time = time.time()

    # 执行矩阵乘法
    result = np.dot(matrix1, matrix2)

    # 计时结束
    end_time = time.time()

    # 输出结果和计算时间
    print(f"{i}*{i}: ", (end_time - start_time)*1000, "ms")
    i*=2