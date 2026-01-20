/**
 * M350 + RK3588视频流集成示例
 * 
 * 本文件展示如何将M350视频流功能集成到Manifold 3平台的主程序中
 */

// ==================== 方法1: 在main函数中直接调用 ====================

#include <liveview/test_m350_liveview_rk3588.h>

int main(int argc, char **argv)
{
    T_DjiReturnCode returnCode;
    
    // ... 现有的系统初始化代码 ...
    
    // 1. 初始化PSDK核心
    returnCode = DjiCore_Init(&userInfo);
    // ...
    
    // 2. 启动应用
    returnCode = DjiCore_ApplicationStart();
    // ...
    
    // 3. 等待系统稳定
    sleep(3);
    
    // 4. 运行M350视频流示例
    USER_LOG_INFO("=== 启动M350视频流拉取示例 ===");
    returnCode = DjiTest_M350LiveviewForRK3588();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("视频流示例执行失败，错误代码: 0x%08X", returnCode);
    } else {
        USER_LOG_INFO("视频流示例执行成功");
    }
    
    // 程序继续运行或退出
    while (1) {
        sleep(1);
    }
}

// ==================== 方法2: 作为独立线程运行 ====================

#include <pthread.h>
#include <liveview/test_m350_liveview_rk3588.h>

// 视频流线程函数
static void *M350_VideoStreamThread(void *argument)
{
    T_DjiReturnCode returnCode;
    
    USER_UTIL_UNUSED(argument);
    
    // 等待系统初始化完成
    sleep(5);
    
    USER_LOG_INFO("视频流线程启动");
    
    // 运行视频流拉取
    returnCode = DjiTest_M350LiveviewForRK3588();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("视频流线程执行失败");
    }
    
    USER_LOG_INFO("视频流线程结束");
    return NULL;
}

int main(int argc, char **argv)
{
    T_DjiReturnCode returnCode;
    pthread_t videoThread;
    
    // ... 系统初始化 ...
    
    // 创建视频流线程
    if (pthread_create(&videoThread, NULL, M350_VideoStreamThread, NULL) != 0) {
        USER_LOG_ERROR("创建视频流线程失败");
    } else {
        USER_LOG_INFO("视频流线程创建成功");
        pthread_setname_np(videoThread, "m350_video");
    }
    
    // 主程序继续运行
    while (1) {
        sleep(1);
    }
}

// ==================== 方法3: 按需调用特定摄像头 ====================

#include <liveview/test_m350_liveview_rk3588.h>

// 自定义函数：只获取红外摄像头视频
T_DjiReturnCode GetInfraredCameraStream(void)
{
    T_DjiReturnCode returnCode;
    
    USER_LOG_INFO("开始获取H20T红外摄像头视频流");
    
    // 初始化liveview模块
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview初始化失败");
        return returnCode;
    }
    
    // 获取30秒红外视频
    returnCode = DjiTest_M350GetCameraStream(
        DJI_LIVEVIEW_CAMERA_POSITION_NO_1,      // 载荷位置1
        DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR,     // H20T红外源
        30,                                      // 30秒
        "m350_infrared.h264"                    // 文件名
    );
    
    // 清理
    DjiLiveview_Deinit();
    
    return returnCode;
}

int main(int argc, char **argv)
{
    // ... 系统初始化 ...
    
    // 调用自定义函数
    GetInfraredCameraStream();
    
    // ...
}

// ==================== 方法4: 循环录制视频 ====================

#include <liveview/test_m350_liveview_rk3588.h>

// 持续录制视频，每次10秒
void ContinuousVideoRecording(void)
{
    T_DjiReturnCode returnCode;
    char filename[256];
    time_t currentTime;
    struct tm *localTime;
    int recordCount = 0;
    
    // 初始化
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("初始化失败");
        return;
    }
    
    USER_LOG_INFO("开始循环录制视频...");
    
    while (1) {
        // 生成文件名
        currentTime = time(NULL);
        localTime = localtime(&currentTime);
        snprintf(filename, sizeof(filename),
                 "m350_continuous_%04d%02d%02d_%02d%02d%02d_%03d.h264",
                 localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
                 localTime->tm_hour, localTime->tm_min, localTime->tm_sec,
                 recordCount);
        
        // 录制10秒
        USER_LOG_INFO("录制第 %d 段视频: %s", recordCount + 1, filename);
        returnCode = DjiTest_M350GetCameraStream(
            DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
            DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
            10,  // 每段10秒
            filename
        );
        
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("录制失败，重试中...");
            sleep(2);
            continue;
        }
        
        recordCount++;
        
        // 可选：限制录制段数
        if (recordCount >= 100) {
            USER_LOG_INFO("达到录制上限，停止录制");
            break;
        }
        
        sleep(1);  // 间隔1秒
    }
    
    // 清理
    DjiLiveview_Deinit();
    USER_LOG_INFO("录制结束，共录制 %d 段视频", recordCount);
}

// ==================== 使用配置文件控制 ====================

typedef struct {
    bool enableFpv;                    // 是否启用FPV视频
    bool enablePayload;                // 是否启用载荷视频
    E_DjiLiveViewCameraSource source;  // 摄像头源
    uint32_t duration;                 // 录制时长
    char outputDir[256];               // 输出目录
} T_VideoConfig;

T_DjiReturnCode RecordVideoWithConfig(T_VideoConfig *config)
{
    T_DjiReturnCode returnCode;
    char filepath[512];
    
    if (config == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }
    
    if (config->enablePayload) {
        snprintf(filepath, sizeof(filepath), "%s/payload_video.h264", config->outputDir);
        USER_LOG_INFO("录制载荷视频到: %s", filepath);
        
        returnCode = DjiTest_M350GetCameraStream(
            DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
            config->source,
            config->duration,
            filepath
        );
    }
    
    DjiLiveview_Deinit();
    return returnCode;
}

int main(int argc, char **argv)
{
    T_VideoConfig config = {
        .enableFpv = false,
        .enablePayload = true,
        .source = DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
        .duration = 30,
        .outputDir = "/data/videos"
    };
    
    // ... 系统初始化 ...
    
    RecordVideoWithConfig(&config);
    
    // ...
}
