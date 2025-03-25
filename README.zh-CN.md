## CarrotFFT 库

🌐 Language: [English](README.md) | [中文](README.zh-CN.md)

![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)

---

CarrotFFT 是一个高效的FFT（快速傅里叶变换）库，支持信号分析和频谱计算。该库提供了FFT实现、窗函数应用以及谐波分析功能。

## 功能特性

- 支持实数FFT（RFFT）
- 提供多种窗函数（如HFT144D）
- 谐波失真分析（THD、SNR计算）
- 跨平台支持（Windows、Linux、STM32）
- 高性能实现，适合嵌入式系统

## 目录结构

```
CarrotFFT/
├── analysis.c           # 信号分析实现
├── analysis.h           # 信号分析头文件
├── fft_impl.c          # FFT核心算法
├── fft_impl.h          # FFT头文件
├── load_data.cpp       # 数据加载工具
├── main.cpp            # 测试主程序
├── main.h              # 主头文件
├── timer_wrapper.c     # 跨平台计时器
└── timer_wrapper.h     # 计时器头文件
```

## 构建说明

### Visual Studio
1. 打开Visual Studio
2. 创建新项目或打开现有解决方案
3. 添加所有源文件到项目
4. 配置项目属性为C++17标准
5. 构建并运行

### Keil MDK
1. 创建新项目
2. 添加所有源文件
3. 配置目标设备选项
4. 设置C/C++选项：
   - 启用C99支持
   - 添加必要的包含路径
5. 构建并下载到目标设备

## 使用示例

```c
#include "fft_impl.h"
#include "analysis.h"

// 初始化FFT
fft_t* fft = fft_init(1024);
double* real = malloc(1024 * sizeof(double));
double* img = malloc(1024 * sizeof(double));

// 执行FFT计算
fft_calc(fft, real, img);

// 信号分析
window_t* win = get_window_by_name("HFT144D");
analysis_t* analysis = analysis_init(1024, 44100.0, win, 5);
analysis_run(analysis, real, img);
```

## 许可证

Apache License 2.0

## 贡献

欢迎提交问题和拉取请求。

---

*本README由DeepSeek生成*