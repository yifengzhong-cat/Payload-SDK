# M350无人机视频流解决方案总结

## 问题描述

用户需要在M350无人机上通过RK3588处理器（Manifold 3平台）拉取摄像头的视频流。

## 解决方案概述

本解决方案提供了完整的代码实现和文档，使用DJI Payload SDK的LiveView接口实现M350无人机摄像头视频流的获取和保存。

## 提供的文件

### 1. 核心实现代码

#### `test_m350_liveview_rk3588.c` 和 `.h`
- **功能**: 完整的M350视频流拉取实现
- **特点**: 
  - 自动识别M350无人机型号
  - 支持FPV和载荷摄像头
  - 支持多种云台相机（H20/H20T/H20N等）
  - 详细的中文注释
  - 错误处理和日志输出
  - 自动保存为.h264文件

#### 主要函数

1. **`DjiTest_M350LiveviewForRK3588()`**
   - 完整演示示例
   - 自动执行所有步骤：初始化→启动视频流→录制→停止→清理
   - 默认录制30秒
   - 适合快速测试和学习

2. **`DjiTest_M350GetCameraStream()`**
   - 通用接口函数
   - 可指定摄像头位置、源、录制时长和文件名
   - 适合集成到自己的应用中

### 2. 集成示例代码

#### `integration_example.c.txt`
提供4种不同的集成方式（参考代码片段，不编译）：
1. 在main函数中直接调用
2. 作为独立线程运行
3. 按需调用特定摄像头
4. 循环录制视频

> **注意**: 此文件是代码参考示例，不会被编译。请根据需要复制相应代码到您的项目中。

### 3. 文档说明

#### `QUICK_START_M350.md` - 快速入门指南
- 三步快速使用流程
- 常见问题排查
- 支持的摄像头和源列表

#### `README_M350_RK3588.md` - 详细技术文档
- 完整的功能特性说明
- 硬件和软件要求
- 详细的API接口说明
- 高级使用示例
- 性能参数和技术支持信息

## 核心技术要点

### 1. 视频流接口

使用DJI Payload SDK提供的LiveView接口：

```c
// 初始化
DjiLiveview_Init();

// 启动H264视频流
DjiLiveview_StartH264Stream(
    position,  // 摄像头位置
    source,    // 摄像头源
    callback   // 回调函数
);

// 停止视频流
DjiLiveview_StopH264Stream(position, source);

// 清理
DjiLiveview_Deinit();
```

### 2. 回调机制

视频数据通过回调函数异步接收：

```c
void CameraCallback(E_DjiLiveViewCameraPosition position, 
                   const uint8_t *buf, uint32_t bufLen)
{
    // buf包含H264编码的视频数据
    // 可以直接保存到文件或进行实时处理
    fwrite(buf, 1, bufLen, file);
}
```

### 3. 摄像头位置

```c
DJI_LIVEVIEW_CAMERA_POSITION_NO_1  // 载荷位置1
DJI_LIVEVIEW_CAMERA_POSITION_NO_2  // 载荷位置2  
DJI_LIVEVIEW_CAMERA_POSITION_NO_3  // 载荷位置3
DJI_LIVEVIEW_CAMERA_POSITION_FPV   // FPV摄像头
```

### 4. 摄像头源（根据云台型号）

```c
// 通用
DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT

// H20T云台
DJI_LIVEVIEW_CAMERA_SOURCE_H20T_WIDE  // 广角
DJI_LIVEVIEW_CAMERA_SOURCE_H20T_ZOOM  // 变焦
DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR    // 红外

// H20N云台
DJI_LIVEVIEW_CAMERA_SOURCE_H20N_WIDE
DJI_LIVEVIEW_CAMERA_SOURCE_H20N_ZOOM
DJI_LIVEVIEW_CAMERA_SOURCE_H20N_IR
```

## 使用流程

### 最简单的使用方式

```c
#include <liveview/test_m350_liveview_rk3588.h>

int main(void)
{
    // 1. 初始化DJI SDK
    DjiCore_Init(&userInfo);
    DjiCore_ApplicationStart();
    
    // 2. 等待系统稳定
    sleep(3);
    
    // 3. 运行视频流示例（自动完成所有操作）
    DjiTest_M350LiveviewForRK3588();
    
    // 4. 查看保存的.h264文件
    return 0;
}
```

### 自定义使用方式

```c
// 获取H20T红外摄像头60秒视频
DjiLiveview_Init();

DjiTest_M350GetCameraStream(
    DJI_LIVEVIEW_CAMERA_POSITION_NO_1,   // 位置1
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR,  // 红外源
    60,                                   // 60秒
    "my_infrared_video.h264"             // 文件名
);

DjiLiveview_Deinit();
```

## 技术特点

### ✅ 优势

1. **完全中文注释** - 所有代码都有详细的中文说明
2. **开箱即用** - 提供完整示例，可直接运行
3. **灵活集成** - 提供多种集成方式供选择
4. **错误处理** - 完善的错误检查和日志输出
5. **文档完整** - 包含快速入门和详细技术文档
6. **标准H264** - 输出标准H264格式，可用任何播放器播放

### ⚠️ 注意事项

1. **Manifold 3限制**: FPV和主摄像头DEFAULT源不能同时订阅
2. **应用配置**: 需要在DJI开发者网站注册应用并配置信息
3. **硬件连接**: 确保Manifold 3与M350通过E-Port正确连接
4. **存储空间**: H264视频流通常2-8Mbps，需要足够存储空间

## 编译和部署

### 编译
```bash
cd /path/to/Payload-SDK
mkdir build && cd build
cmake ..
make
```

生成的可执行文件：
- `build/bin/dji_sdk_demo_on_manifold3`

### 部署到Manifold 3
```bash
# 将可执行文件复制到Manifold 3
scp build/bin/dji_sdk_demo_on_manifold3 dji@manifold3:/home/dji/

# SSH登录Manifold 3并运行
ssh dji@manifold3
sudo ./dji_sdk_demo_on_manifold3
```

## 视频处理

### 播放
```bash
# VLC
vlc m350_payload_stream_*.h264

# FFmpeg
ffplay m350_payload_stream_*.h264
```

### 转换为MP4
```bash
ffmpeg -i m350_payload_stream_*.h264 -c:v copy output.mp4
```

### 实时流媒体
可以结合FFmpeg将H264流转为RTSP/RTMP流：
```bash
ffmpeg -i m350_payload_stream.h264 -f rtsp rtsp://localhost:8554/live
```

## 扩展开发建议

### 1. 实时图像处理
在回调函数中集成OpenCV进行实时处理：
```c
void ProcessCallback(E_DjiLiveViewCameraPosition position,
                    const uint8_t *buf, uint32_t bufLen)
{
    // 解码H264
    AVFrame *frame = DecodeH264(buf, bufLen);
    
    // OpenCV处理
    cv::Mat image = ConvertToMat(frame);
    // ... 目标检测、识别等 ...
}
```

### 2. 多路流同步
如果需要同时获取多路视频：
```c
// 使用不同的摄像头源避免冲突
DjiLiveview_StartH264Stream(
    DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_ZOOM,
    ZoomCallback
);

DjiLiveview_StartH264Stream(
    DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR,
    IRCallback
);
```

### 3. 网络传输
将视频流通过网络传输：
```c
void NetworkCallback(E_DjiLiveViewCameraPosition position,
                     const uint8_t *buf, uint32_t bufLen)
{
    // 通过UDP/TCP发送
    send(socket, buf, bufLen, 0);
}
```

## 性能优化建议

1. **缓冲区管理**: 使用环形缓冲区提高性能
2. **多线程**: 将视频处理放在独立线程
3. **文件IO**: 使用缓冲写入减少磁盘IO
4. **内存管理**: 及时释放不用的缓冲区

## 故障排查

| 问题 | 可能原因 | 解决方案 |
|-----|---------|---------|
| 编译失败 | 缺少头文件 | 清理build目录重新编译 |
| 无法连接无人机 | 物理连接问题 | 检查E-Port连接 |
| 视频流启动失败 | 应用配置错误 | 检查app_info配置 |
| 文件无法播放 | 录制时间太短 | 至少录制5秒以上 |
| 同时订阅失败 | Manifold 3限制 | 使用不同源或分开订阅 |

## 技术支持

- **官方文档**: https://developer.dji.com/
- **开发者论坛**: https://djisdksupport.zendesk.com/hc/zh-cn
- **GitHub Issues**: 在本仓库提交问题

## 版本信息

- **DJI Payload SDK**: 3.15.0
- **支持平台**: Manifold 3 (RK3588)
- **支持无人机**: M350 RTK
- **支持云台**: H20/H20T/H20N及其他DJI企业级云台

## 总结

本解决方案提供了一套完整的M350无人机视频流获取实现，包括：
- ✅ 可直接使用的示例代码
- ✅ 详细的中文注释和文档
- ✅ 多种集成方式
- ✅ 完善的错误处理
- ✅ 技术支持和故障排查指南

用户可以直接使用提供的示例代码，也可以根据自己的需求进行定制开发。

---

**开发日期**: 2024-01-20  
**SDK版本**: DJI Payload SDK 3.15.0  
**目标平台**: M350 RTK + Manifold 3 (RK3588)
