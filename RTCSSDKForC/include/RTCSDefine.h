#ifndef RTCS_DEFINE_H
#define RTCS_DEFINE_H

/////////////////////////消息码定义/////////////////////////////////////
#define WM_GV                       200
#define WM_GV_CONNECT               WM_GV + 1       ///< 客户端连接服务器，wParam（BOOL）表示是否连接成功
#define WM_GV_LOGINSYSTEM           WM_GV + 2       ///< 客户端登录系统，wParam（INT）表示自己的用户ID号，lParam（INT）表示登录结果：0 成功，否则为出错代码，参考出错代码定义
#define WM_GV_ENTERROOM             WM_GV + 3       ///< 客户端进入房间，wParam（INT）表示所进入房间的ID号，lParam（INT）表示是否进入房间：0成功进入，否则为出错代码
#define WM_GV_USERATROOM            WM_GV + 5       ///< 用户进入（离开）房间，wParam（INT）表示用户ID号，lParam（BOOL）表示该用户是进入（TRUE）或离开（FALSE）房间

#define WM_GV_LEAVEROOM             WM_GV + 51       ///< 用户离开房间，wParam（INT）表示房间ID号，lParam（INT）表示离开房间用户ID
#define WM_GV_PUBLISHSTATE          WM_GV + 55       ///< 自己与服务器建立连接成功，wParam（INT）表示userid，用户的ID
#define WM_GV_RECOGNIZEVOICECTR          WM_GV + 56       ///< 语音识别控制，wParam（INT）表示语音要控制的操作，具体见enum RecognizeVoiceStatus中所示

//////////////////////////////消息错误码定义/////////////////////////////////////
#define RTCS_SUCCESS                    0           ///< 成功
// < 系统没有初始化
#define RTCS_ERR_NOTINIT                2           ///< 系统没有初始化
#define RTCS_ERR_NOTINROOM              3           ///< 还未进入房间

#define RTCS_ERR_FUNCNOTALLOW           20          ///< 函数功能不允许
#define RTCS_ERR_FUNCOPTERROR           21          ///< 函数参数错误

////连接部分
#define RTCS_ERR_CONNECT_TIMEOUT        100         ///< 连接服务器超时
#define RTCS_ERR_CONNECT_ABORT          101         ///< 与服务器的连接中断
#define RTCS_ERR_RECONN_CUSTOMSERVER     102        ///< 与业务服务器正在重连
#define RTCS_ERR_RECONN_CONFSERVER       103        ///< 与会议服务器正在重连
#define RTCS_ERR_CONNECT_OVERFLOW       104         ///< 超过授权用户数
////登录部分
#define RTCS_ERR_CERTIFY_FAIL           200         ///< 认证失败，用户名或密码有误
#define RTCS_ERR_ALREADY_LOGIN          201         ///< 该用户已登录
#define RTCS_ERR_VISITOR_DENY           204         ///< 游客登录被禁止（登录时没有输入密码）
//#define RTCS_ERR_INVALID_USERID         205         ///< 无效的用户ID（用户不存在）
#define RTCS_ERR_SERVERSDK_FAIL         206         ///< 与业务服务器连接失败，认证功能失效
#define RTCS_ERR_SERVERSDK_TIMEOUT      207         ///< 业务服务器执行任务超时
#define RTCS_ERR_NOTLOGIN               208         ///< 没有登录

////进入、离开房间
#define RTCS_ERR_ROOM_ENTERFAIL         307         ///< 不能进入房间
#define RTCS_ERR_ROOM_ALREADIN          308         ///< 已经在房间里面了，本次进入房间请求忽略
#define RTCS_ERR_ROOM_LEAVEFAIL         309     ///<离开房间失败

////打开摄像头
#define RTCS_ERR_CAMERA_NULL            601          ///<没有摄像头
#define RTCS_ERR_CAMERA_OPENFAIL        602          ///<打开摄像头失败

////////////////////////////////////////////////////////////////////////

///////////////内核参数定义（API：RTCS_SetSDKOption、RTCS_GetSDKOption 传入参数）////////////
#define RTCS_SO_AUDIO_VADCTRL               1   ///< 音频静音检测控制（参数为：int型：1打开，0关闭）
#define RTCS_SO_AUDIO_NSCTRL                2   ///< 音频噪音抑制控制（参数为：int型：1打开，0关闭）
#define RTCS_SO_AUDIO_ECHOCTRL              3   ///< 音频回音消除控制（参数为：int型：1打开，0关闭）
#define RTCS_SO_AUDIO_AGCCTRL               4   ///< 音频自动增益控制（参数为：int型：1打开，0关闭）
#define RTCS_SO_AUDIO_CAPTUREMODE           5   ///< 音频采集模式设置（参数为：int型：0 发言模式，1 放歌模式，2 卡拉OK模式，3 线路输入模式）
#define RTCS_SO_AUDIO_MICBOOST              6   ///< 音频采集Mic增强控制（参数为：int型：0 取消，1 选中，2 设备不存在[查询时返回值]）
#define RTCS_SO_AUDIO_MONOBITRATE           8   ///< 设置单声道模式下音频编码目标码率（参数为：int型，单位：bps
#define RTCS_SO_AUDIO_STEREOBITRATE         9   ///< 设置双声道模式下音频编码目标码率(参数为：int型 单位：bps )

#define RTCS_SO_CORESDK_EXTVIDEOINPUT		26	///< 外部扩展视频输入控制（参数为int型， 0 关闭外部视频输入[默认]， 1 启用外部视频输入）
#define RTCS_SO_CORESDK_EXTAUDIOINPUT		27	///< 外部扩展音频输入控制（参数为int型， 0 关闭外部音频输入[默认]， 1 启用外部音频输入）

#define RTCS_SO_LOCALVIDEO_BITRATECTRL      30  ///< 本地视频编码码率设置（参数为int型，单位bps，同服务器配置：VideoBitrate）
#define RTCS_SO_LOCALVIDEO_QUALITYCTRL      31///< 本地视频编码质量因子控制（参数为int型，同服务器配置：VideoQuality）
#define RTCS_SO_LOCALVIDEO_GOPCTRL          32  ///< 本地视频编码关键帧间隔控制（参数为int型，同服务器配置：VideoGOPSize）
#define RTCS_SO_LOCALVIDEO_FPSCTRL          33  ///< 本地视频编码帧率控制（参数为int型，同服务器配置：VideoFps）
#define RTCS_SO_LOCALVIDEO_PRESETCTRL       34	///< 本地视频编码预设参数控制（参数为int型，1-5）
#define RTCS_SO_LOCALVIDEO_APPLYPARAM       35  ///< 应用本地视频编码参数，使得前述修改即时生效（参数为int型：1 使用新参数，0 使用默认参数）
#define RTCS_SO_LOCALVIDEO_WIDTHCTRL        38  ///< 本地视频采集分辨率宽度控制（参数为int型，同服务器配置：VideoWidth）
#define RTCS_SO_LOCALVIDEO_HEIGHTCTRL       39  ///< 本地视频采集分辨率高度控制（参数为int型，同服务器配置：VideoHeight）
#define RTCS_SO_HTTP_SERVER_PORTS           40  ///< 设置HTTP的访问端口
#define RTCS_SO_BUSINESS_SERVER_PORTS       41  ///< 设置业务服务器的访问端口
#define RTCS_SO_CORE_SERVER_PORTS           42  ///< 设置核心服务器的访问端口


#define RTCS_SO_TIMEOUT_SEC                0xF001 /// http连接，socket连接超时设置
#define RTCS_SO_RENDERFLAG_SEC             0xF002 ///< 本地视频绘制展示控制标志位（参数为int型：0 绘制全窗口展示（原图像截取） 1 适应窗口比例展示）

/////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////// 用户状态标志定义（API：RTCS_QueryUserState 传入参数）/////////////////////////////
#define RTCS_USERSTATE_CAMERA               1   ///< 用户摄像头状态（参数为INT型）
#define RTCS_USERSTATE_HOLDMIC              2   ///< 用户音频设备状态（参数为INT型，返回值：0 音频采集关闭， 1 音频采集开启）
#define RTCS_USERSTATE_SPEAKVOLUME          3   ///< 用户当前说话音量（参数为INT类型（0 ~ 100））
#define RTCS_USERSTATE_RECORDING            4   ///< 用户录像（音）状态（参数为INT型）
#define RTCS_USERSTATE_LEVEL                5   ///< 用户级别（参数为INT型）
#define RTCS_USERSTATE_NICKNAME             6   ///< 用户昵称（参数为字符串类型）

#define RTCS_USERSTATE_VIDEOBITRATE         9   ///< 用户当前的视频码率（参数为INT类型，Bps）
#define RTCS_USERSTATE_VIDEOSIZE            13  ///< 查询指定用户的视频分辨率（参数为INT类型，返回值：低16位表示宽度，高16位表示高度）
#define RTCS_USERSTATE_SELFUSERSTATUS       16  ///< 查询本地用户的当前状态（参数为INT类型，返回值：0 Unknow，1 Connected，2 Logined，3 In Room，4 Logouted，5 Link Closed）

///////////////////////////////////////////////////////////////////////////////////////////////////////////


enum RTCS_AudioDevice {
    RTCS_AD_WAVEIN = 0,     ///< 输入设备：Mic
    RTCS_AD_WAVEOUT = 1     ///< 输出设备：Wave
};
enum VideoCaptureState {
    GV_CAMERA_STATE_NULL = 0,
    GV_CAMERA_STATE_NORMAL,
    GV_CAMERA_STATE_OPEN
};

enum RecognizeVoiceStatus {
    RECOGNIZE_ACTIVED,
    RECOGNIZE_NO_CLEAR,
    ENTER_ROOM,
    LEAVE_ROOM,
    OPEN_CAMERA,
    CLOSE_CAMERA
};
#endif //  RTCS_DEFINE_H

