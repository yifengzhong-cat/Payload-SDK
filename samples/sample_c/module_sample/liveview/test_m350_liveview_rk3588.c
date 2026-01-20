/**
 ********************************************************************
 * @file    test_m350_liveview_rk3588.c
 * @brief   M350无人机通过RK3588(Manifold 3)拉取摄像头视频流示例
 *          M350 drone camera video stream example for RK3588 (Manifold 3)
 *
 * @copyright (c) 2021 DJI. All rights reserved.
 *
 *********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "test_m350_liveview_rk3588.h"
#include "dji_liveview.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "dji_aircraft_info.h"
#include "widget_interaction_test/test_widget_interaction.h"
#include <time.h>
#include <stdio.h>

/* Private constants ---------------------------------------------------------*/
// 视频流保存时长（秒）
#define M350_VIDEO_STREAM_DURATION_SECONDS    30

/* Private values -------------------------------------------------------------*/
// FPV摄像头视频流文件路径
static char s_m350FpvStreamFilePath[256];
// 载荷摄像头视频流文件路径
static char s_m350PayloadStreamFilePath[256];

/* Private functions declaration ---------------------------------------------*/
/**
 * @brief FPV摄像头H264视频流回调函数
 * @param position: 摄像头位置
 * @param buf: H264视频数据缓冲区
 * @param bufLen: 数据长度
 */
static void M350_FpvCameraH264Callback(E_DjiLiveViewCameraPosition position, 
                                       const uint8_t *buf, uint32_t bufLen);

/**
 * @brief 载荷摄像头H264视频流回调函数
 * @param position: 摄像头位置
 * @param buf: H264视频数据缓冲区
 * @param bufLen: 数据长度
 */
static void M350_PayloadCameraH264Callback(E_DjiLiveViewCameraPosition position, 
                                           const uint8_t *buf, uint32_t bufLen);

/* Exported functions definition ---------------------------------------------*/

/**
 * @brief M350无人机视频流拉取示例（RK3588平台）
 * @note 本示例适用于M350无人机通过Manifold 3（RK3588处理器）拉取摄像头视频流
 * @return 执行结果
 */
T_DjiReturnCode DjiTest_M350LiveviewForRK3588(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiAircraftInfoBaseInfo aircraftInfo = {0};
    time_t currentTime;
    struct tm *localTime;

    USER_LOG_INFO("=================================================");
    USER_LOG_INFO("M350 视频流拉取示例启动 (RK3588/Manifold 3)");
    USER_LOG_INFO("M350 Video Stream Example Start (RK3588/Manifold 3)");
    USER_LOG_INFO("=================================================");

    // 步骤1: 获取无人机信息，确认是M350
    USER_LOG_INFO("\n步骤 1: 获取无人机信息");
    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("获取无人机信息失败");
        return returnCode;
    }

    // 验证无人机型号
    if (aircraftInfo.aircraftSeries != DJI_AIRCRAFT_SERIES_M350) {
        USER_LOG_WARN("警告: 当前无人机不是M350，但仍可尝试获取视频流");
        USER_LOG_INFO("当前无人机系列: %d", aircraftInfo.aircraftSeries);
    } else {
        USER_LOG_INFO("确认无人机型号: M350");
    }

    // 验证是否为Manifold 3平台
    if (aircraftInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD) {
        USER_LOG_INFO("确认平台: Manifold 3 (RK3588)");
    } else {
        USER_LOG_WARN("当前平台不是Manifold 3");
    }

    // 步骤2: 初始化liveview模块
    USER_LOG_INFO("\n步骤 2: 初始化视频流模块");
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("视频流模块初始化失败, 错误代码: 0x%08X", returnCode);
        return returnCode;
    }
    USER_LOG_INFO("视频流模块初始化成功");

    // 步骤3: 开始拉取FPV摄像头视频流
    USER_LOG_INFO("\n步骤 3: 开始拉取FPV摄像头H264视频流");
    
    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    snprintf(s_m350FpvStreamFilePath, sizeof(s_m350FpvStreamFilePath),
             "m350_fpv_stream_%04d%02d%02d_%02d%02d%02d.h264",
             localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
             localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    // 注意：在Manifold 3上，FPV和主摄像头的DEFAULT源不能同时订阅
    // Note: On Manifold 3, FPV and main camera DEFAULT sources cannot be subscribed simultaneously
    if (aircraftInfo.mountPositionType != DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD) {
        returnCode = DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV,
                                                 DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                                 M350_FpvCameraH264Callback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("启动FPV视频流失败, 错误代码: 0x%08X", returnCode);
        } else {
            USER_LOG_INFO("FPV视频流启动成功，保存到文件: %s", s_m350FpvStreamFilePath);
        }
    } else {
        USER_LOG_INFO("跳过FPV视频流（Manifold 3平台限制）");
    }

    // 步骤4: 开始拉取载荷摄像头视频流
    USER_LOG_INFO("\n步骤 4: 开始拉取载荷摄像头H264视频流");
    
    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    snprintf(s_m350PayloadStreamFilePath, sizeof(s_m350PayloadStreamFilePath),
             "m350_payload_stream_%04d%02d%02d_%02d%02d%02d.h264",
             localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
             localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    // 使用载荷位置1的摄像头（可以根据实际情况修改为位置2或3）
    // Use payload position 1 camera (can be changed to position 2 or 3 as needed)
    returnCode = DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
                                             DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                             M350_PayloadCameraH264Callback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("启动载荷摄像头视频流失败, 错误代码: 0x%08X", returnCode);
        goto cleanup;
    }
    USER_LOG_INFO("载荷摄像头视频流启动成功，保存到文件: %s", s_m350PayloadStreamFilePath);

    // 步骤5: 持续接收视频流数据
    USER_LOG_INFO("\n步骤 5: 接收视频流数据 (持续 %d 秒)", M350_VIDEO_STREAM_DURATION_SECONDS);
    for (int i = 0; i < M350_VIDEO_STREAM_DURATION_SECONDS; i++) {
        USER_LOG_INFO("正在录制视频流... %d/%d 秒", i + 1, M350_VIDEO_STREAM_DURATION_SECONDS);
        osalHandler->TaskSleepMs(1000);
    }

    // 步骤6: 停止视频流
    USER_LOG_INFO("\n步骤 6: 停止视频流");
    
    if (aircraftInfo.mountPositionType != DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD) {
        returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV,
                                                DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("停止FPV视频流失败, 错误代码: 0x%08X", returnCode);
        }
    }

    returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
                                            DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("停止载荷摄像头视频流失败, 错误代码: 0x%08X", returnCode);
    }

    USER_LOG_INFO("视频流已停止");
    USER_LOG_INFO("\n保存的文件:");
    if (aircraftInfo.mountPositionType != DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD) {
        USER_LOG_INFO("  - FPV视频流: %s", s_m350FpvStreamFilePath);
    }
    USER_LOG_INFO("  - 载荷摄像头视频流: %s", s_m350PayloadStreamFilePath);

cleanup:
    // 步骤7: 清理资源
    USER_LOG_INFO("\n步骤 7: 清理视频流模块");
    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("视频流模块清理失败, 错误代码: 0x%08X", returnCode);
    }

    USER_LOG_INFO("\n=================================================");
    USER_LOG_INFO("M350 视频流拉取示例结束");
    USER_LOG_INFO("=================================================");

    return returnCode;
}

/**
 * @brief 获取并保存特定摄像头源的视频流
 * @param position: 摄像头位置
 * @param source: 摄像头源
 * @param duration: 录制时长（秒）
 * @param filename: 保存文件名
 * @return 执行结果
 * 
 * @note 使用示例:
 *       // 获取H20T的红外摄像头视频流
 *       DjiTest_M350GetCameraStream(DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
 *                                   DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR,
 *                                   30,
 *                                   "m350_h20t_ir.h264");
 */
T_DjiReturnCode DjiTest_M350GetCameraStream(E_DjiLiveViewCameraPosition position,
                                            E_DjiLiveViewCameraSource source,
                                            uint32_t duration,
                                            const char *filename)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    FILE *streamFile = NULL;
    static char s_streamBuffer[256];

    USER_LOG_INFO("开始获取摄像头视频流");
    USER_LOG_INFO("  位置: %d, 源: %d", position, source);
    USER_LOG_INFO("  保存文件: %s", filename);

    // 内部回调函数，用于保存视频数据
    static FILE *s_currentFile = NULL;
    s_currentFile = fopen(filename, "wb");
    if (s_currentFile == NULL) {
        USER_LOG_ERROR("无法创建文件: %s", filename);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    // 定义回调函数
    void StreamCallback(E_DjiLiveViewCameraPosition pos, const uint8_t *buf, uint32_t len) {
        if (s_currentFile && buf && len > 0) {
            size_t written = fwrite(buf, 1, len, s_currentFile);
            if (written != len) {
                USER_LOG_ERROR("写入文件失败");
            }
            fflush(s_currentFile);
        }
    }

    // 启动视频流
    returnCode = DjiLiveview_StartH264Stream(position, source, StreamCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("启动视频流失败, 错误代码: 0x%08X", returnCode);
        fclose(s_currentFile);
        return returnCode;
    }

    // 录制指定时长
    for (uint32_t i = 0; i < duration; i++) {
        USER_LOG_INFO("录制中... %d/%d 秒", i + 1, duration);
        osalHandler->TaskSleepMs(1000);
    }

    // 停止视频流
    returnCode = DjiLiveview_StopH264Stream(position, source);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("停止视频流失败, 错误代码: 0x%08X", returnCode);
    }

    if (s_currentFile) {
        fclose(s_currentFile);
        s_currentFile = NULL;
    }

    USER_LOG_INFO("视频流录制完成: %s", filename);
    return returnCode;
}

/* Private functions definition-----------------------------------------------*/

static void M350_FpvCameraH264Callback(E_DjiLiveViewCameraPosition position, 
                                       const uint8_t *buf, uint32_t bufLen)
{
    FILE *fp = NULL;
    size_t size;

    // 打开文件并追加数据
    fp = fopen(s_m350FpvStreamFilePath, "ab+");
    if (fp == NULL) {
        USER_LOG_ERROR("无法打开FPV视频流文件: %s", s_m350FpvStreamFilePath);
        return;
    }

    // 写入H264数据
    size = fwrite(buf, 1, bufLen, fp);
    if (size != bufLen) {
        USER_LOG_ERROR("写入FPV视频数据失败");
    }

    fflush(fp);
    fclose(fp);
}

static void M350_PayloadCameraH264Callback(E_DjiLiveViewCameraPosition position, 
                                           const uint8_t *buf, uint32_t bufLen)
{
    FILE *fp = NULL;
    size_t size;

    // 打开文件并追加数据
    fp = fopen(s_m350PayloadStreamFilePath, "ab+");
    if (fp == NULL) {
        USER_LOG_ERROR("无法打开载荷摄像头视频流文件: %s", s_m350PayloadStreamFilePath);
        return;
    }

    // 写入H264数据
    size = fwrite(buf, 1, bufLen, fp);
    if (size != bufLen) {
        USER_LOG_ERROR("写入载荷摄像头视频数据失败");
    }

    fflush(fp);
    fclose(fp);
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
