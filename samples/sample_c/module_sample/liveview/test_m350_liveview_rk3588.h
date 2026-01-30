/**
 ********************************************************************
 * @file    test_m350_liveview_rk3588.h
 * @brief   M350无人机通过RK3588(Manifold 3)拉取摄像头视频流示例头文件
 *          M350 drone camera video stream example header for RK3588 (Manifold 3)
 *
 * @copyright (c) 2021 DJI. All rights reserved.
 *
 *********************************************************************
 */

#ifndef TEST_M350_LIVEVIEW_RK3588_H
#define TEST_M350_LIVEVIEW_RK3588_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_liveview.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief M350无人机视频流拉取示例（RK3588平台）
 * @details 本函数演示如何在RK3588(Manifold 3)平台上从M350无人机拉取摄像头视频流
 * 
 * 功能说明:
 * - 初始化视频流模块
 * - 获取FPV摄像头的H264视频流
 * - 获取载荷摄像头的H264视频流
 * - 将视频流保存为.h264文件
 * - 可使用VLC或FFmpeg播放保存的视频文件
 * 
 * 使用方法:
 * 1. 确保M350无人机已连接到Manifold 3(RK3588)
 * 2. 在main函数中调用此函数
 * 3. 视频流将自动保存到当前目录
 * 
 * 播放保存的视频:
 * - 使用VLC: vlc m350_payload_stream_*.h264
 * - 使用FFmpeg: ffplay m350_payload_stream_*.h264
 * 
 * @note 需要先调用 DjiCore_Init() 和 DjiCore_ApplicationStart()
 * @return 执行结果
 */
T_DjiReturnCode DjiTest_M350LiveviewForRK3588(void);

/**
 * @brief 获取并保存特定摄像头源的视频流
 * @details 这是一个通用函数，可以获取任意摄像头位置和源的视频流
 * 
 * @param position: 摄像头位置
 *        - DJI_LIVEVIEW_CAMERA_POSITION_NO_1: 载荷位置1
 *        - DJI_LIVEVIEW_CAMERA_POSITION_NO_2: 载荷位置2
 *        - DJI_LIVEVIEW_CAMERA_POSITION_NO_3: 载荷位置3
 *        - DJI_LIVEVIEW_CAMERA_POSITION_FPV: FPV摄像头
 * 
 * @param source: 摄像头源（取决于安装的云台相机型号）
 *        M350常用的相机源:
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT: 默认摄像头源
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20_WIDE: H20广角镜头
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20_ZOOM: H20变焦镜头
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20T_WIDE: H20T广角镜头
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20T_ZOOM: H20T变焦镜头
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR: H20T红外镜头
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20N_WIDE: H20N广角镜头
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20N_ZOOM: H20N变焦镜头
 *        - DJI_LIVEVIEW_CAMERA_SOURCE_H20N_IR: H20N红外镜头
 * 
 * @param duration: 录制时长（秒）
 * @param filename: 保存的文件名（包含路径）
 * 
 * @return 执行结果
 * 
 * @note 使用示例:
 * @code
 * // 初始化PSDK
 * DjiCore_Init(&userInfo);
 * DjiCore_ApplicationStart();
 * DjiLiveview_Init();
 * 
 * // 获取H20T红外摄像头30秒视频
 * DjiTest_M350GetCameraStream(DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
 *                             DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR,
 *                             30,
 *                             "m350_h20t_ir_stream.h264");
 * 
 * // 清理
 * DjiLiveview_Deinit();
 * @endcode
 */
T_DjiReturnCode DjiTest_M350GetCameraStream(E_DjiLiveViewCameraPosition position,
                                            E_DjiLiveViewCameraSource source,
                                            uint32_t duration,
                                            const char *filename);

#ifdef __cplusplus
}
#endif

#endif // TEST_M350_LIVEVIEW_RK3588_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
