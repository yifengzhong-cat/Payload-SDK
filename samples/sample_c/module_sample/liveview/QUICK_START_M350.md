# M350无人机视频流快速入门指南

## 概述

本示例代码为M350无人机配合RK3588处理器(Manifold 3平台)提供完整的视频流拉取解决方案。

## 核心功能

✅ **自动识别M350无人机**  
✅ **支持FPV和载荷摄像头**  
✅ **支持多种DJI云台相机** (H20/H20T/H20N等)  
✅ **H264格式视频流**  
✅ **详细的中文注释**  
✅ **多种集成方式**

## 三步快速使用

### 第1步：配置应用信息

编辑 `samples/sample_c/platform/linux/manifold3/application/dji_sdk_app_info.h`:

```c
// 将以下信息替换为您在DJI开发者网站注册的应用信息
#define USER_APP_NAME "M350VideoStream"
#define USER_APP_ID "your_app_id_here"
#define USER_APP_KEY "your_app_key_here"
#define USER_APP_LICENSE "your_app_license_here"
#define USER_DEVELOPER_ACCOUNT "your_email@example.com"
#define USER_BAUD_RATE "921600"
```

> 💡 如何获取应用信息：访问 https://developer.dji.com/ 注册并创建应用

### 第2步：集成到程序

在 `samples/sample_c/platform/linux/manifold3/application/main.c` 中添加：

```c
// 文件顶部添加头文件
#include <liveview/test_m350_liveview_rk3588.h>

// 在 DjiCore_ApplicationStart() 之后添加
int main(int argc, char **argv)
{
    // ... 现有初始化代码 ...
    
    returnCode = DjiCore_ApplicationStart();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("start sdk application error");
    }
    
    // 添加以下代码
    sleep(3);  // 等待系统稳定
    USER_LOG_INFO("Starting M350 video stream example...");
    returnCode = DjiTest_M350LiveviewForRK3588();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("M350 video stream failed with code: 0x%08X", returnCode);
    }
    
    // 程序继续运行...
    while (1) {
        sleep(1);
    }
}
```

### 第3步：编译和运行

```bash
# 在Manifold 3上执行以下命令
cd /path/to/Payload-SDK
mkdir build && cd build
cmake ..
make

# 运行程序
cd samples/sample_c/platform/linux/manifold3/
sudo ./dji_sdk_demo_on_manifold3
```

## 程序输出

运行成功后，你会看到：

```
=================================================
M350 视频流拉取示例启动 (RK3588/Manifold 3)
=================================================

步骤 1: 获取无人机信息
确认无人机型号: M350
确认平台: Manifold 3 (RK3588)

步骤 2: 初始化视频流模块
视频流模块初始化成功

步骤 3: 开始拉取FPV摄像头H264视频流
FPV视频流启动成功，保存到文件: m350_fpv_stream_20240120_143025.h264

步骤 4: 开始拉取载荷摄像头H264视频流
载荷摄像头视频流启动成功，保存到文件: m350_payload_stream_20240120_143025.h264

步骤 5: 接收视频流数据 (持续 30 秒)
正在录制视频流... 1/30 秒
正在录制视频流... 2/30 秒
...

步骤 6: 停止视频流
视频流已停止

保存的文件:
  - FPV视频流: m350_fpv_stream_20240120_143025.h264
  - 载荷摄像头视频流: m350_payload_stream_20240120_143025.h264
```

## 播放视频

### 方法1: 使用VLC播放器

```bash
vlc m350_payload_stream_20240120_143025.h264
```

### 方法2: 使用FFmpeg播放

```bash
ffplay m350_payload_stream_20240120_143025.h264
```

### 方法3: 转换为MP4格式

```bash
ffmpeg -i m350_payload_stream_20240120_143025.h264 -c:v copy output.mp4
```

## 高级用法示例

更多集成示例请参考 `integration_example.c.txt` 文件，该文件包含4种不同的集成方式：
1. 在main函数中直接调用
2. 作为独立线程运行
3. 按需调用特定摄像头
4. 循环录制视频

> **注意**: `integration_example.c.txt` 是参考代码片段集合，不会被编译。请根据需要复制相应代码到您的项目中。

### 示例1: 只录制红外摄像头

```c
#include <liveview/test_m350_liveview_rk3588.h>

int main(void)
{
    // ... 初始化 ...
    
    DjiLiveview_Init();
    
    // 录制30秒H20T红外视频
    DjiTest_M350GetCameraStream(
        DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
        DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR,
        30,
        "infrared.h264"
    );
    
    DjiLiveview_Deinit();
}
```

### 示例2: 更多集成方式

详见 `integration_example.c.txt` 文件中的完整示例。

## 支持的摄像头和源

| 云台相机 | 摄像头源常量 | 说明 |
|---------|------------|------|
| 通用 | `DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT` | 默认源（推荐） |
| H20 | `DJI_LIVEVIEW_CAMERA_SOURCE_H20_WIDE` | 广角镜头 |
| H20 | `DJI_LIVEVIEW_CAMERA_SOURCE_H20_ZOOM` | 变焦镜头 |
| H20T | `DJI_LIVEVIEW_CAMERA_SOURCE_H20T_WIDE` | 广角镜头 |
| H20T | `DJI_LIVEVIEW_CAMERA_SOURCE_H20T_ZOOM` | 变焦镜头 |
| H20T | `DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR` | 红外镜头 |
| H20N | `DJI_LIVEVIEW_CAMERA_SOURCE_H20N_WIDE` | 广角镜头 |
| H20N | `DJI_LIVEVIEW_CAMERA_SOURCE_H20N_ZOOM` | 变焦镜头 |
| H20N | `DJI_LIVEVIEW_CAMERA_SOURCE_H20N_IR` | 红外镜头 |

## 常见问题排查

### Q1: 编译错误 "undefined reference to DjiTest_M350LiveviewForRK3588"

**原因**: 新文件没有被包含到编译中

**解决方案**: 
```bash
cd build
rm -rf *
cmake ..
make
```

### Q2: 运行时错误 "获取无人机信息失败"

**可能原因**:
- Manifold 3与M350未正确连接
- E-Port连接松动
- 无人机未开机

**解决方案**:
1. 检查物理连接
2. 确保无人机已开机
3. 检查E-Port连接是否牢固

### Q3: 视频流启动失败 (错误代码: 0x...)

**可能原因**:
- 应用信息配置错误
- 云台相机未安装或未开机
- 遥控器未连接

**解决方案**:
1. 确认应用信息正确
2. 检查云台相机是否正常工作
3. 确保遥控器已连接到无人机

### Q4: .h264文件无法播放

**可能原因**:
- 录制时间太短
- 文件大小为0（没有接收到数据）

**解决方案**:
1. 确保至少录制5秒以上
2. 检查文件大小：`ls -lh *.h264`
3. 使用VLC或FFmpeg播放，不要用系统默认播放器

### Q5: Manifold 3限制问题

**问题**: FPV和主摄像头DEFAULT源不能同时订阅

**解决方案**: 
- 先录制FPV，停止后再录制载荷摄像头
- 或使用不同的摄像头源（如H20T的IR+ZOOM可以同时使用）

## 性能参数

| 参数 | 值 |
|-----|---|
| 视频格式 | H.264 |
| 典型码率 | 2-8 Mbps |
| 分辨率 | 取决于云台相机型号 |
| 延迟 | < 200ms |
| 支持同时流数 | 1-2路（取决于平台） |

## 文件结构

```
samples/sample_c/module_sample/liveview/
├── test_m350_liveview_rk3588.c      # 主实现文件
├── test_m350_liveview_rk3588.h      # 头文件
├── integration_example.c             # 集成示例代码
├── README_M350_RK3588.md            # 详细说明文档
└── QUICK_START_M350.md              # 本快速入门指南
```

## 技术支持

- **官方文档**: https://developer.dji.com/doc/payload-sdk-tutorial/cn/
- **API参考**: https://developer.dji.com/doc/payload-sdk-api-reference/cn/
- **论坛支持**: https://djisdksupport.zendesk.com/hc/zh-cn

## 下一步

1. ✅ 尝试运行基础示例
2. ✅ 了解不同摄像头源的使用
3. ✅ 学习如何实时处理视频流（查看integration_example.c）
4. ✅ 根据需求定制录制参数

---

**提示**: 本指南针对M350+RK3588(Manifold 3)平台，使用DJI Payload SDK 3.15.0版本。
