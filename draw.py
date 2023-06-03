import pandas as pd
import matplotlib.pyplot as plt
import math
# 读取CSV文件
data = pd.read_csv('cmp.csv')

# 提取需要绘制曲线的列
column_names = ['FINAL', 'without_blcoking','numpy']  # 将'column1'和'column2'替换为您实际的列名
columns = data[column_names]

# 平滑曲线处理
smoothed_data = columns.rolling(window=1).mean()  # 使用窗口大小为5的移动平均线平滑数据
i=128
x=[]
while i<8913:
    x.append(f"{i}*{i}")
    i*=2
# 绘制曲线对比图
plt.figure(figsize=(10, 6))  # 设置图形大小
for column in smoothed_data.columns:
    print(smoothed_data[column])
    plt.plot(x,smoothed_data[column], label=column)  # 绘制曲线
plt.legend()  # 显示图例
plt.xlabel('Matrix size')  # 替换为实际的X轴标签
plt.ylabel('time')  # 替换为实际的Y轴标签
plt.title('comparasion')  # 替换为实际的标题
plt.grid(True)  # 显示网格线
plt.show()  # 显示图形
