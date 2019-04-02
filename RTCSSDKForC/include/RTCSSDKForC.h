#ifndef RTCS_SDK_H
#define RTCS_SDK_H

#ifdef _MSC_VER
#if !defined(_MT)
#error "Must compile with /MD, /MDd, /MT or /MTd"
#endif

#if defined(NDEBUG) && defined(_DEBUG)
#error "Inconsistent build settings (check for /MD[d])"
#endif

#ifdef RTCSSDK_EXPORTS
#define RTCS_C_API __declspec(dllexport)
#else
#define RTCS_C_API __declspec(dllimport)
#endif

#endif
#include "RTCSDefine.h"

////////////////////////回调函数命名在这里////////////////////////////////

//异步消息通知回调函数定义
typedef void(*RTCS_NotifyMessage_CallBack)(int dwNotifyMsg, int wParam,
        int lParam, void* lpUserValue);
//透明通道数据回调函数定义
typedef void(*RTCS_TransBuffer_CallBack)(int dwUserid, const char* lpBuf,
        int dwLen, void* lpUserValue);

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif
//////////////////////////////SDK 相关////////////////////////////
//初始化SDK
RTCS_C_API int RTCS_InitSDK(unsigned int dwFuncMode);
//释放SDK资源
RTCS_C_API int RTCS_Release();
//设置SDK内核参数
RTCS_C_API int RTCS_SetSDKOption(int optname, const char* optval, int optlen);
// 获取SDK内核参数
RTCS_C_API int RTCS_GetSDKOption(int optname, char* optval, int optlen);
/////////////////////////////////////////////////////////////////

/////////////////////////////回调函数注册//////////////////////////
//设置异步消息通知回调函数
RTCS_C_API int RTCS_SetNotifyMessageCallBack(RTCS_NotifyMessage_CallBack
        lpFunction, void* lpUserValue = 0);
// 设置扩展异步消息通知回调
RTCS_C_API int RTCS_SetNotifyMessageCallBackEx(int pluginID,
        RTCS_NotifyMessage_CallBack lpFunction, void* lpUserValue = 0);
//设置透明通道数据回调函数
RTCS_C_API int RTCS_SetTransBufferCallBack(RTCS_TransBuffer_CallBack lpFunction,
        void* lpUserValue = 0);
// 设置扩展透明通道数据回调函数
RTCS_C_API int RTCS_SetTransBufferCallBackEx(int pluginID,
        RTCS_TransBuffer_CallBack lpFunction, void* lpUserValue = 0);
/////////////////////////////////////////////////////////////////

/////////////////////////////业务流程/////////////////////////////
//连接服务器
RTCS_C_API int RTCS_Connect(const char* lpServerAddr, int port);
//登录服务器
RTCS_C_API int RTCS_Login(const char* username, const char* passwd);
// 单点登录方式登录服务器 上层传入json串
RTCS_C_API int RTCS_SSOLogin(const char* json);
//注销系统
RTCS_C_API int RTCS_Logout();
//进入房间
RTCS_C_API int RTCS_EnterRoom(int roomid, const char* lpRoomPass = 0);
//离开房间
RTCS_C_API int RTCS_LeaveRoom(int roomid);
//获取指定房间在线用户列表
/*
* roomid:房间id
*userIDArray:用户列表，可以为 NULL，表示只获取用户数量（注：需要自己申请空间，自己释放）
*dwUserNum：用户数量
*/
RTCS_C_API int RTCS_GetRoomOnlineUsers(int roomid, int* userIDArray,
                                       unsigned int& dwUserNum);
//查询用户状态
/*
* infoname可参考枚举RTCSQueryUserStateInfo
*/
RTCS_C_API int RTCS_QueryUserState(int userid, int infoname, char* infoval,
                                   int infolen);
//透明通道传送缓冲区
// userid:-1 向当前房间房间广播 不支持频率很高的调用
//        -2 向当前登录的用户广播，频率无所谓
//        =0 向服务器发送
//        >0 向指定用户发送

// Equals RTCS_TransBufferEx(pluginID=0.....)
RTCS_C_API int RTCS_TransBuffer(int userid, const char* lpBuf, int dwLen);

// 扩展透明通道传送缓冲区
// userid 同上
// pluginID 基于pluginID的插件类型消息传送通道
// pluginID=0 will FallBack RTCS_TransBuffer(......)
RTCS_C_API int RTCS_TransBufferEx(int pluginID,int userid, const char* lpBuf,
                                  int dwLen);

////////////////////////////////////////////////////////////////

/////////////////////////////视频设备相关/////////////////////////
//枚举视频采集设备
RTCS_C_API int RTCS_EnumVideoCapture(char** lpDeviceName,
                                     unsigned int& dwDeviceNum);
//枚举视频采集设备
RTCS_C_API int RTCS_SelectVideoCapture(const char* szCaptureName);
//获取当前使用的视频采集设备
RTCS_C_API int RTCS_GetCurVideoCapture(char* deviceName, int len);
//获取指定视频采集设备属性
RTCS_C_API int RTCS_GetVideoCaptureAttributes(char* szCaptureName,
        char* szAttributes);
//设置视频显示位置
RTCS_C_API int RTCS_SetVideoPos(int userid, void* hWnd, unsigned int dwLeft,
                                unsigned int dwTop, unsigned int dwRight, unsigned int dwBottom);
//用户信息水印叠加
// asume the use data is encoded in utf-8
RTCS_C_API int RTCS_PaintUserValue(int userid, const char* data,
                                   const char* font = "msyh", int size = 40);
//操作用户视频
RTCS_C_API int RTCS_UserCameraControl(int userid, bool isopen);
//用户全屏幕共享
RTCS_C_API int RTCS_UserDesktopShare(int desktop_id = 0);
//用户窗体共享
RTCS_C_API int RTCS_UserWindowShare(void* hWnd);
//改变某用户视频绘制效果
// render_flag: 0 原视频根据窗口尺寸截取，全窗口展示
//              1 保留原视频所有信息，适应窗口比例展示
RTCS_C_API int RTCS_SetRenderFlag(int userid, int render_flag);
/////////////////////////////////////////////////////////////////

/////////////////////////////音频设备相关/////////////////////////
//选择音频播放设备
RTCS_C_API int RTCS_EnumAudioCapture(char** lpDeviceName,
                                     unsigned int& dwDeviceNum);
//选择音频采集设备
RTCS_C_API int RTCS_SelectAudioCapture(char* szCaptureName);
// 获取当前使用的音频采集设备
RTCS_C_API int RTCS_GetCurAudioCapture(char* deviceName, int len);
//枚举音频播放设备
RTCS_C_API int RTCS_EnumAudioPlayback(char** lpDeviceName,
                                      unsigned int& dwDeviceNum);
//选择音频播放设备
RTCS_C_API int RTCS_SelectAudioPlayback(char* szDeviceName);
// 获取当前使用的音频播放设备
RTCS_C_API int RTCS_GetCurAudioPlayback(char* deviceName, int len);
//获取指定音频设备的当前音量
RTCS_C_API int RTCS_AudioGetVolume(RTCS_AudioDevice device,
                                   unsigned int& dwVolume);
//设置指定音频设备的音量
RTCS_C_API int RTCS_AudioSetVolume(RTCS_AudioDevice device,
                                   unsigned int dwVolume);
//操作用户语音
RTCS_C_API int RTCS_UserSpeakControl(int userid, bool isopen);

// 控制用户音视频录制状态
RTCS_C_API int RTCS_UserRecordControl(int userid,bool record);

/////////////////////////////////////////////////////////////////
#ifdef __cplusplus
};
#endif // _cplusplus
//////////////////////////////////////////////////////////////////////
#endif // !RTCS_SDK_H
