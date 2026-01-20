# M350无人机视频流拉取示例 (RK3588/Manifold 3平台)

本示例代码演示如何在RK3588处理器的Manifold 3平台上，通过DJI Payload SDK从M350无人机拉取摄像头视频流。

## 功能特性

- ✅ 支持M350无人机
- ✅ 支持RK3588处理器 (Manifold 3平台)
- ✅ 获取FPV摄像头H264视频流
- ✅ 获取载荷摄像头H264视频流
- ✅ 支持多种云台相机（H20/H20T/H20N等）
- ✅ 自动保存视频流到.h264文件
- ✅ 详细的中文注释

## 文件说明

- `test_m350_liveview_rk3588.c` - 主要实现代码
- `test_m350_liveview_rk3588.h` - 头文件定义
- `README_M350_RK3588.md` - 本说明文档

## 硬件要求

1. **无人机**: DJI M350 RTK
2. **机载计算机**: DJI Manifold 3 (基于RK3588处理器)
3. **云台相机**: 任意DJI企业级云台相机（如H20/H20T/H20N等）
4. **连接方式**: Manifold 3通过E-Port连接到M350

## 软件要求

1. DJI Payload SDK 3.15.0或更高版本
2. Ubuntu 20.04/22.04 (Manifold 3预装系统)
3. GCC编译器
4. CMake 3.10或更高版本

## 快速开始

### 步骤1: 配置DJI开发者信息

编辑文件 `samples/sample_c/platform/linux/manifold3/application/dji_sdk_app_info.h`，填入您的应用信息：

```c
#define USER_APP_NAME "your_app_name"
#define USER_APP_ID "your_app_id"
#define USER_APP_KEY "your_app_key"
#define USER_APP_LICENSE "your_app_license"
#define USER_DEVELOPER_ACCOUNT "your_developer_account"
#define USER_BAUD_RATE "921600"
```

> 📌 **如何获取这些信息**: 访问 [DJI开发者网站](https://developer.dji.com/) 注册应用

### 步骤2: 添加示例代码到编译系统

将示例代码集成到Manifold 3平台的main函数中：

编辑 `samples/sample_c/platform/linux/manifold3/application/main.c`：

```c
// 在文件顶部添加头文件
#include <liveview/test_m350_liveview_rk3588.h>

// 在main函数的适当位置调用
int main(int argc, char **argv)
{
    // ... 现有的初始化代码 ...
    
    // 启动应用后，添加以下代码
    returnCode = DjiCore_ApplicationStart();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("start sdk application error");
    }
    
    // 添加M350视频流示例
    sleep(3);  // 等待系统稳定
    USER_LOG_INFO("启动M350视频流拉取示例...");
    returnCode = DjiTest_M350LiveviewForRK3588();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("M350视频流示例执行失败");
    }
    
    // ... 其余代码 ...
}
```

### 步骤3: 编译项目

```bash
cd /path/to/Payload-SDK
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### 步骤4: 运行程序

```bash
cd samples/sample_c/platform/linux/manifold3/
sudo ./psdk_demo
```

### 步骤5: 查看保存的视频

程序运行完成后，视频流将保存在当前目录：

- `m350_fpv_stream_YYYYMMDD_HHMMSS.h264` - FPV摄像头视频
- `m350_payload_stream_YYYYMMDD_HHMMSS.h264` - 载荷摄像头视频

播放视频：

```bash
# 使用VLC播放器
vlc m350_payload_stream_*.h264

# 或使用FFmpeg播放
ffplay m350_payload_stream_*.h264

# 转换为MP4格式
ffmpeg -i m350_payload_stream_*.h264 -c:v copy output.mp4
```

## 高级使用

### 获取特定摄像头源的视频流

如果您的M350配备了H20T云台相机，可以单独获取红外或变焦镜头的视频流：

```c
#include <liveview/test_m350_liveview_rk3588.h>

// 初始化
DjiLiveview_Init();

// 获取H20T红外镜头视频流（30秒）
DjiTest_M350GetCameraStream(
    DJI_LIVEVIEW_CAMERA_POSITION_NO_1,        // 载荷位置1
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR,       // H20T红外源
    30,                                        // 30秒
    "h20t_ir_stream.h264"                     // 文件名
);

// 获取H20T变焦镜头视频流（30秒）
DjiTest_M350GetCameraStream(
    DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_ZOOM,     // H20T变焦源
    30,
    "h20t_zoom_stream.h264"
);

// 清理
DjiLiveview_Deinit();
```

### 支持的摄像头源

根据您安装的云台相机型号，可以使用以下摄像头源：

#### H20云台
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20_WIDE` - 广角镜头
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20_ZOOM` - 变焦镜头

#### H20T云台
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20T_WIDE` - 广角镜头
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20T_ZOOM` - 变焦镜头
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR` - 红外镜头

#### H20N云台
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20N_WIDE` - 广角镜头
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20N_ZOOM` - 变焦镜头
- `DJI_LIVEVIEW_CAMERA_SOURCE_H20N_IR` - 红外镜头

#### 通用
- `DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT` - 默认摄像头源（推荐首次使用）

## API接口说明

### DjiTest_M350LiveviewForRK3588()

主示例函数，完整演示视频流拉取过程。

**返回值**: `T_DjiReturnCode` - 执行结果

**功能**:
1. 验证无人机型号和平台
2. 初始化视频流模块
3. 启动FPV和载荷摄像头视频流
4. 录制30秒视频
5. 停止视频流并保存文件
6. 清理资源

### DjiTest_M350GetCameraStream()

通用视频流获取函数，可指定摄像头位置、源和录制时长。

**参数**:
- `position` - 摄像头位置（1/2/3或FPV）
- `source` - 摄像头源（取决于云台型号）
- `duration` - 录制时长（秒）
- `filename` - 保存文件名

**返回值**: `T_DjiReturnCode` - 执行结果

## 注意事项

### Manifold 3平台限制

⚠️ **重要**: 在Manifold 3平台上，FPV摄像头和主载荷摄像头的DEFAULT源**不能同时订阅**。

如果需要同时获取两路视频流，建议：
1. 先录制FPV视频流
2. 停止FPV视频流
3. 再录制载荷摄像头视频流

或者使用不同的摄像头源（如H20T的红外+变焦）。

### 性能优化建议

1. **视频码率**: H264视频流码率通常在2-8Mbps，请确保存储空间充足
2. **处理延迟**: 回调函数应尽快处理数据，避免阻塞
3. **文件操作**: 建议使用缓冲写入，减少IO开销

### 常见问题

**Q: 运行时提示"获取无人机信息失败"？**
A: 检查Manifold 3与M350的物理连接，确保E-Port连接正常。

**Q: 视频流无法启动，返回错误代码？**
A: 
- 确认无人机已开机且遥控器已连接
- 检查应用信息配置是否正确
- 确认云台相机已安装并正常工作

**Q: 保存的.h264文件无法播放？**
A: 
- 确认录制时长足够（至少5秒）
- 使用VLC或FFmpeg播放，不要用系统默认播放器
- 检查文件大小，如果为0或很小说明没有接收到数据

**Q: 如何实时显示视频流而不是保存到文件？**
A: 可以在回调函数中使用OpenCV或FFmpeg进行实时解码和显示：

```c
// 示例：使用FFmpeg解码
static void RealtimeDisplayCallback(E_DjiLiveViewCameraPosition position, 
                                   const uint8_t *buf, uint32_t bufLen)
{
    // 将H264数据送入解码器
    // 显示解码后的图像
    // 具体实现请参考FFmpeg文档
}
```

## 技术支持

- **DJI开发者论坛**: https://djisdksupport.zendesk.com/hc/zh-cn
- **官方文档**: https://developer.dji.com/doc/payload-sdk-tutorial/cn/
- **API参考**: https://developer.dji.com/doc/payload-sdk-api-reference/cn/

## 许可证

本示例代码遵循DJI Payload SDK的MIT许可证。

## 更新日志

- **2024-01-20**: 创建M350 + RK3588视频流拉取示例
  - 添加完整的中文注释
  - 支持FPV和载荷摄像头
  - 支持多种云台相机型号

---

**开发提示**: 本示例基于DJI Payload SDK 3.15.0版本开发，适配RK3588处理器的Manifold 3平台。
