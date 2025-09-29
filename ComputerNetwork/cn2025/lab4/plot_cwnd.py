import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import csv
import matplotlib as mpl

# 配置matplotlib支持中文显示
plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'Arial Unicode MS']  # 优先使用的中文字体
plt.rcParams['axes.unicode_minus'] = False  # 解决负号显示问题

# 读取CSV文件
df = pd.read_csv('tcp_cwnd.csv')

# 创建图形
plt.figure(figsize=(12, 6))

# 设置图表样式
plt.rcParams.update({'font.size': 12})

# 绘制cwnd变化的折线图(蓝色)
plt.plot(df['time_ms'], df['cwnd_mss'], 'b-', linewidth=2, label='cwnd')

# 绘制ssthresh的变化(红色虚线)
plt.plot(df['time_ms'], df['ssthresh_mss'], 'r--', linewidth=1, alpha=0.7, label='ssthresh')

# 设置坐标轴为黑色
for spine in plt.gca().spines.values():
    spine.set_color('black')
plt.gca().tick_params(axis='both', colors='black')

# 添加标题和标签
plt.title('TCP拥塞窗口随时间变化', color='black', fontweight='bold', fontsize=14)
plt.xlabel('时间 (ms)', color='black', fontweight='bold')
plt.ylabel('窗口大小 (MSS)', color='black', fontweight='bold')
plt.grid(True, linestyle='--', alpha=0.3)
plt.legend(loc='upper right')

# 设置坐标轴范围
y_min = df['cwnd_mss'].min() * 0.8
y_max = df['cwnd_mss'].max() * 1.2
plt.ylim(y_min, y_max)

# 保存图片
plt.savefig('tcp_cwnd_plot.png', dpi=300, bbox_inches='tight')

# 显示图形
plt.show()