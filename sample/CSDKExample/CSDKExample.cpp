#include "CSDKExample.h"

#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qmessagebox.h>
#include <qglobal.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qfiledialog.h>
#include <qthread.h>

#include <Windows.h>

#include "SettingConfig.h"
#include "NativeTest.h"
#include "RTCSSDKForC.h"
#include "RTCSDefine.h"

static void  NotifyMessage_CallBack(int dwNotifyMsg, int wParam, int lParam, void* lpUserValue) {
    CSDKExample*  pRTCS_SDKProc = (CSDKExample*)lpUserValue;
    if (!pRTCS_SDKProc)
        return;
    switch (dwNotifyMsg) {
    case WM_GV_CONNECT:
        pRTCS_SDKProc->OnUserConnect(wParam);
        break;
    case WM_GV_LOGINSYSTEM:
        pRTCS_SDKProc->OnUserlogin(wParam, lParam);
        break;
    case WM_GV_ENTERROOM:
        pRTCS_SDKProc->OnEnterRoom(wParam, lParam);
        break;
    case WM_GV_LEAVEROOM:
        pRTCS_SDKProc->OnLeaveRoom(wParam, lParam);
        break;
    case WM_GV_USERATROOM:
        pRTCS_SDKProc->OnRemoteUserEnter(wParam, lParam);
        break;
    default:
        break;
    }
}

static void TransBuffer_CallBack(int dwUserid, const char* lpBuf, int dwLen, void* lpUserValue) {
    CSDKExample*  pTransBuffer = (CSDKExample*)lpUserValue;
    if (!pTransBuffer)
        return;
    switch (dwUserid)
    {
    case RTCS_SUCCESS:
        break;
    default:
        //pTransBuffer->OnTransBuffer(dwUserid, lpBuf, dwLen);
        break;
    }
}

CSDKExample::~CSDKExample() {

}

CSDKExample::CSDKExample(QWidget *parent)
    : QMainWindow(parent) {
    ui.setupUi(this);

    RTCS_InitSDK(0);  //初始化SDK

    ///////////////////////主界面日志输出功能的连接函数////////////////////////////
    qDebug() << connect(this, &CSDKExample::UpdateLog, this, [this](const QString& q) {
        AppenedLog(q);
    });
    /*************************参数配置菜单初始化及相关操作**************************/
    setting_ui_ = new SettingConfig();
    setting_ui_->ApplySetting();

    //emit UpdateLog(setting_ui_->vidoeparamsinfo);

    QAction* configAction = ui.menuBar->addAction(QString::fromLocal8Bit("参数配置"));
    qDebug() << connect(configAction, &QAction::triggered, this, [this]() {
        setting_ui_->show();
		setting_ui_->raise();
    });

    /*************************本地音频测试菜单初始化及相关操作**************************/
    native_ui_ = new NativeTest();
    QAction* nativeTestAction = ui.menuBar->addAction(QString::fromLocal8Bit("本地音频测试"));
    qDebug() << connect(nativeTestAction, &QAction::triggered, this, [this]() {
        native_ui_->show();
        native_ui_->raise();
    });
    /*************************主界面参数设置及相关操作**************************/

    /////////////////////////登录服务器默认设置//////////////////////////////
    ui.tex_username->setText("zhaoj");
    ui.tex_passward->setText("1");
    ui.tex_server->setText("10.0.8.146:8906");
    ui.text_desktopid->setText("0");
    ui.text_windowname->setText("CSDKExample");

    ///////////////////////界面所有按钮操作连接函数////////////////////////////
    connect(ui.btn_login, SIGNAL(clicked()), this, SLOT(UserLogin()));
    connect(ui.btn_logout, SIGNAL(clicked()), this, SLOT(UserLogout()));
    connect(ui.btn_joinRoom, SIGNAL(clicked()), this, SLOT(JoinRoom()));
    connect(ui.btn_leave_room, SIGNAL(clicked()), this, SLOT(LeaveRoom()));
    connect(ui.btn_getusers, SIGNAL(clicked()), this, SLOT(GetUserList()));
    connect(ui.btn_transbuffer, SIGNAL(clicked()), this, SLOT(TransBuffer()));
    connect(ui.btn_transbufferroom, SIGNAL(clicked()), this, SLOT(TransBufferRoom()));
    connect(ui.btn_desktopshare, SIGNAL(clicked()), this, SLOT(UserDesktopShare()));
    connect(ui.btn_windowshare, SIGNAL(clicked()), this, SLOT(UserWindowShare()));

    ///////////////////////界面Frame初始化/////////////////////////////////
    initFrame(ui.localframe, ui.enableAudio_cb_local, ui.enableVideo_cb_local,ui.label, ui.local_audio_level);
    initFrame(ui.remoteframe, ui.enableAudio_cb_remote_1, ui.enableVideo_cb_remote_1, ui.remote_label_1, ui.remote_audio_level_1);
    initFrame(ui.remoteframe_2, ui.enableAudio_cb_remote_2, ui.enableVideo_cb_remote_2, ui.remote_label_2, ui.remote_audio_level_2);
    initFrame(ui.remoteframe_3, ui.enableAudio_cb_remote_3, ui.enableVideo_cb_remote_3, ui.remote_label_3, ui.remote_audio_level_3);
    initFrame(ui.remoteframe_4, ui.enableAudio_cb_remote_4, ui.enableVideo_cb_remote_4, ui.remote_label_4, ui.remote_audio_level_4);
    initFrame(ui.remoteframe_5, ui.enableAudio_cb_remote_5, ui.enableVideo_cb_remote_5, ui.remote_label_5, ui.remote_audio_level_5);
    initFrame(ui.remoteframe_6, ui.enableAudio_cb_remote_6, ui.enableVideo_cb_remote_6, ui.remote_label_6, ui.remote_audio_level_6);
    initFrame(ui.remoteframe_7, ui.enableAudio_cb_remote_7, ui.enableVideo_cb_remote_7, ui.remote_label_7, ui.remote_audio_level_7);

    /////////////////////localFrame对应的Checkbox的连接函数//////////////////////
    connect(ui.enableAudio_cb_local, &QCheckBox::clicked, [this] {
        LocalMeidiaControl(ui.enableAudio_cb_local);
    });
    connect(ui.enableVideo_cb_local, &QCheckBox::clicked, [this] {
        LocalMeidiaControl(ui.enableVideo_cb_local);
    });
    connect(ui.enableAudio_cb_remote_1, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableAudio_cb_remote_1);
    });
    connect(ui.enableVideo_cb_remote_1, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableVideo_cb_remote_1);
    });
    connect(ui.enableAudio_cb_remote_2, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableAudio_cb_remote_2);
    });
    connect(ui.enableVideo_cb_remote_2, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableVideo_cb_remote_2);
    });
    connect(ui.enableAudio_cb_remote_3, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableAudio_cb_remote_3);
    });
    connect(ui.enableVideo_cb_remote_3, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableVideo_cb_remote_3);
    });
    connect(ui.enableAudio_cb_remote_4, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableAudio_cb_remote_4);
    });
    connect(ui.enableVideo_cb_remote_4, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableVideo_cb_remote_4);
    });
    connect(ui.enableAudio_cb_remote_5, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableAudio_cb_remote_5);
    });
    connect(ui.enableVideo_cb_remote_5, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableVideo_cb_remote_5);
    });
    connect(ui.enableAudio_cb_remote_6, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableAudio_cb_remote_6);
    });
    connect(ui.enableVideo_cb_remote_6, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableVideo_cb_remote_6);
    });
    connect(ui.enableAudio_cb_remote_7, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableAudio_cb_remote_7);
    });
    connect(ui.enableVideo_cb_remote_7, &QCheckBox::clicked, [this] {
        RemoteMediaControl(ui.enableVideo_cb_remote_7);
    });

    ///////////////////////////////其他/////////////////////////////////
    connect(this, SIGNAL(OnEnterRoom_Sig(int, int)), this, SLOT(EnterRoom_Slots(int, int)));
    connect(this, SIGNAL(OnLeaveRoom_Sig(int, int)), this, SLOT(LeaveRoom_Slots(int, int)));
    qDebug() << connect(this, SIGNAL(OnRemoteStream_Sig(int, QString)),
                        this, SLOT(RemoteStreamEnter_Slots(int, QString)));


}

void CSDKExample::initFrame(QFrame* frame, QCheckBox* audiobox,
                            QCheckBox* videobox, QLabel* label, QProgressBar* processbar) {
    frame->show();
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    frame->setPalette(palette);
    frame->setAutoFillBackground(true);
    frame_struct.username_lable = label;
    frame_struct.frame = frame;
    frame_struct.audio_box = audiobox;
    frame_struct.video_box = videobox;
    if (processbar) {
        processbar->setRange(0, 100);
        processbar->setValue(0);
    }
    frame_struct.audio_level = processbar;
    frame_vector.push_back(frame_struct);
}

void CSDKExample::LocalMeidiaControl(QCheckBox* checkbox) {
    if (checkbox == ui.enableAudio_cb_local) {
        RTCS_UserSpeakControl(-1, ui.enableAudio_cb_local->checkState());
    } else {
        //关闭视频时，初始化localframe
        if (ui.enableVideo_cb_local->checkState() == Qt::CheckState::Unchecked) {
            RTCS_UserCameraControl(-1, ui.enableVideo_cb_local->checkState());

            QPalette palette;
            palette.setColor(QPalette::Background, Qt::black);
            ui.localframe->setPalette(palette);
            ui.localframe->setAutoFillBackground(true);
            ui.localframe->show();
            ui.localframe->update();
            return;
        }
        //配置本地视频所有参数
        ConfigLocalVideoParam();
        //打开摄像头
        RTCS_UserCameraControl(-1, ui.enableVideo_cb_local->checkState());
    }
}
void CSDKExample::RemoteMediaControl(QCheckBox* checkbox) {
    //打开关闭远端音视频
    for (std::size_t i = 0; i < frame_vector.size(); ++i) {
        if ((frame_vector[i].audio_box == checkbox) &&
            (frame_vector[i].render_flag)) {
            frame_vector[i].audio_box_state = frame_vector[i].audio_box->checkState() ==
                Qt::CheckState::Checked;
            RTCS_UserSpeakControl(frame_vector[i].userid, frame_vector[i].audio_box_state);
            break;
        }
        if ((frame_vector[i].video_box == checkbox) &&
            (frame_vector[i].render_flag)) {
            frame_vector[i].video_box_state = frame_vector[i].video_box->checkState() ==
                Qt::CheckState::Checked;
            RTCS_UserCameraControl(frame_vector[i].userid, frame_vector[i].video_box_state);

            if (frame_vector[i].video_box_state == false)
            {
                //frame_vector[i].frame->close();
                QPalette palette;
                palette.setColor(QPalette::Background, Qt::black);
                frame_vector[i].frame->setPalette(palette);
                frame_vector[i].frame->setAutoFillBackground(true);
                frame_vector[i].frame->update();
            }
            break;
        }
    }
}

void CSDKExample::AppenedLog(const QString& log) {
    ui.tex_conlog->append(log);
}

void CSDKExample::UserLogin() {
    QString address = ui.tex_server->toPlainText().trimmed();
    if (address.isEmpty()) {
        QMessageBox::information(this, "info", QString::fromLocal8Bit("服务器地址未输入"),
                                 QMessageBox::Ok);
        return;
    }
    QStringList splite = address.split(':');
    QString ip = splite.at(0);
    int port = splite.at(1).toInt();

    RTCS_Connect(ip.toStdString().c_str(), port);

	SetServerPorts();

    RTCS_SetNotifyMessageCallBack(&NotifyMessage_CallBack, this);
    RTCS_SetTransBufferCallBack(&TransBuffer_CallBack, this);

    QString username = ui.tex_username->toPlainText().trimmed();
    QString password = ui.tex_passward->toPlainText().trimmed();
    RTCS_Login(username.toStdString().c_str(), password.toStdString().c_str());

}

void CSDKExample::JoinRoom(){
    QString room_name = ui.text_room_name->toPlainText().trimmed();
    if (0 == QString::compare(room_name, "")) {
        emit UpdateLog(QString::fromLocal8Bit("房间名为空，请输入要加入的房间名...."));
        return;
    }
    emit UpdateLog(QString::fromLocal8Bit("申请加入房间 %1").arg(room_name));

    int room_name_int = room_name.toInt();
    RTCS_EnterRoom(room_name_int, "password");
}

void CSDKExample::LeaveRoom() {
    QMap<int, QTimer*>::iterator timer_iter;
    for (timer_iter = userid_timer_map.begin();
        timer_iter != userid_timer_map.end();
        ++timer_iter) {
        if (timer_iter.value()) {
            timer_iter.value()->stop();
            //delete timer_iter.value();
        }
    }
    userid_timer_map.clear();

    QString room_name = ui.text_room_name->toPlainText().trimmed();
    int room_name_int = room_name.toInt();
    emit UpdateLog(QString::fromLocal8Bit("申请离开房间 %1").arg(room_name));
    RTCS_LeaveRoom(room_name_int);
}


void CSDKExample::UserLogout(){
    if (-1 == login_user_id_) {
        emit UpdateLog(QString::fromLocal8Bit("您没有登录!!"));
        return;
    }

    LeaveRoom();
	ui.text_room_name->setText("");

    RTCS_Logout();
    login_user_id_ = -1;
    emit UpdateLog(QString::fromLocal8Bit("退出登录!!"));
}

void CSDKExample::TransBuffer(){
    QString json("{hello:loginuser}");
    QString json2("{hello:2}");
    RTCS_TransBuffer(-2, json.toStdString().c_str(), json.length());
    RTCS_TransBuffer(2, json2.toStdString().c_str(), json2.length());
    emit UpdateLog(QString::fromLocal8Bit("透明通道消息已发送"));
}

void CSDKExample::TransBufferRoom(){
    char buf[5] = { 0 };
    buf[0]= 0x0a;
    buf[1] = 0xAC;
    buf[2] = 0xA1;
    buf[3] = 0x02;
    buf[4] = 0xFE;
    RTCS_TransBuffer(-1, buf, sizeof(buf));
    emit UpdateLog(QString::fromLocal8Bit("透明通道消息已发送"));
}

void CSDKExample::UserDesktopShare() {
    QString desktop_id = ui.text_desktopid->toPlainText().trimmed();
    int desktopid = desktop_id.toInt();
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    ui.localframe->setPalette(palette);
    ui.localframe->setAutoFillBackground(true);
    ui.localframe->show();
    ui.localframe->update();
    RTCS_UserDesktopShare(desktopid);
}

void CSDKExample::UserWindowShare() {
    
    QString windowname = ui.text_windowname->toPlainText().trimmed();
    HWND hwnd = FindWindowA(NULL, windowname.toStdString().c_str());
    if(hwnd == 0)
        emit UpdateLog(QString::fromLocal8Bit("没有该窗体！！"));
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Background, Qt::black);
        ui.localframe->setPalette(palette);
        ui.localframe->setAutoFillBackground(true);
        ui.localframe->show();
        ui.localframe->update();
        RTCS_UserWindowShare((void*)hwnd);
    }
    
}

void CSDKExample::GetUserList() {
    ui.cmb_users_list->clear();
    QString room_name = ui.text_room_name->toPlainText().trimmed();
    QStringList user_list;
    int room_id = room_name.toInt();
    GetUserList(room_id, user_list);

    ui.cmb_users_list->clear();
    ui.cmb_users_list->addItems(user_list);
}

void CSDKExample::OnUserConnect(int result) {
    result ? emit UpdateLog(QString::fromLocal8Bit("连接服务器成功"))
           : emit UpdateLog(QString::fromLocal8Bit("连接服务器失败"));
}

void CSDKExample::OnUserlogin(int userid, int result) {
    switch (result) {
    case RTCS_SUCCESS: {
        login_user_id_ = userid;
        emit UpdateLog(QString::fromLocal8Bit("用戶登录成功"));
        emit UpdateLog(QString::fromLocal8Bit("用户id:%1").arg(userid));
        break;
    }
    case RTCS_ERR_CERTIFY_FAIL: {
        emit UpdateLog(QString::fromLocal8Bit("用戶不存在，登录失败"));
        break;
    }
    case RTCS_ERR_ALREADY_LOGIN: {
        emit UpdateLog(QString::fromLocal8Bit("该用戶已登录，请更改用户名！"));
        break;
    }
    case RTCS_ERR_VISITOR_DENY: {
        emit UpdateLog(QString::fromLocal8Bit("游客登录被禁止，请输入正确的密码！"));
        break;
    }
    case RTCS_ERR_SERVERSDK_FAIL: {
        emit UpdateLog(QString::fromLocal8Bit("连接服务器失败！"));
        break;
    }
    case RTCS_ERR_SERVERSDK_TIMEOUT: {
        emit UpdateLog(QString::fromLocal8Bit("连接服务器超时！"));
        break;
    }
    default: {
        break;
    }
    }
}

void CSDKExample::OnUserlogout(int result){
    switch (result) {
    case RTCS_SUCCESS: {
        emit UpdateLog(QString::fromLocal8Bit("用户登出"));
        ui.text_room_name->clear();
        break;
    }
    case RTCS_ERR_NOTLOGIN: {
        emit UpdateLog(QString::fromLocal8Bit("用户不存在，请先登录！！"));
        break;
    }
    default: {
        break;
    }
    }
}

void CSDKExample::OnEnterRoom(int roomid, int result){
    emit OnEnterRoom_Sig(roomid, result);//跨线程操作
}

void test(int roomid, unsigned int num) {
    RTCS_GetRoomOnlineUsers(roomid, nullptr, num);
}

void CSDKExample::EnterRoom_Slots(int roomid, int result){
    if (result == RTCS_SUCCESS){
        emit UpdateLog(QString::fromLocal8Bit("加入房间成功，房间号:%1").arg(roomid));

        //获取房间所有在线用户
        GetUserList();
        //占用第一个frame
        frame_vector[0].render_flag = true;
        frame_vector[0].userid = login_user_id_;
        frame_vector[0].username_lable->setText(userid_name_map[login_user_id_]);
        //配置本地视频所有参数
        ConfigLocalVideoParam();
        //初始化本地音频设备，打开麦克风
        native_ui_->InitAudioMicDevice();
        native_ui_->InitAudioPlayoutDevice();
        //定时更新音量条
        CreateTimer(-1);

        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this, roomid] {
            test(roomid, 0);
        });
        timer->start(10);

    } else if (result == RTCS_ERR_ROOM_ALREADIN) {
        emit UpdateLog(QString::fromLocal8Bit("已经在房间了"));
    } else {
        emit UpdateLog(QString::fromLocal8Bit("加入房间失败"));
    }
}

void CSDKExample::ConfigLocalVideoParam() {
    //打开视频，设置视频需要的所有参数
    setting_ui_->ApplySetting();
    int parameterapply = 1;
    char buffer[sizeof(int)] = { 0 };
    memcpy(buffer, &parameterapply, sizeof(int));
    RTCS_SetSDKOption(RTCS_SO_LOCALVIDEO_APPLYPARAM, buffer, sizeof(parameterapply));
    ////设置视频显示位置
    RTCS_SetVideoPos(-1, (HWND)ui.localframe->winId(), 0, 0,
        ui.localframe->width(), ui.localframe->height());
    //用户信息水印叠加
    QString username = ui.tex_username->toPlainText().trimmed();
    RTCS_PaintUserValue(-1, username.toStdString().c_str());
}

void CSDKExample::GetUserList(int roomid, QStringList& user_list) {
    unsigned int users_num = 20;
    int users_id[20] = { 0 };
    RTCS_GetRoomOnlineUsers(roomid, users_id, users_num);
    for (std::size_t i = 0; i < users_num; ++i) {
        if (!users_id[i]) break;

        int userid = users_id[i];
        char username[64] = { 0 };
        RTCS_QueryUserState(userid, RTCS_USERSTATE_NICKNAME, username, 64);
        QString user_name = username;
        user_list.append(user_name);
        userid_name_map[userid] = user_name;
    }
}

void CSDKExample::OnLeaveRoom(int roomid, int userid){
    emit OnLeaveRoom_Sig(roomid, userid);//跨线程操作
}

void CSDKExample::LeaveRoom_Slots(int roomid, int userid)
{
    if (userid == login_user_id_) {
        //本地用户离开房间，初始化所有使用的frame
        emit UpdateLog(QString::fromLocal8Bit("离开房间成功"));
        for (std::size_t i = 0; i < frame_vector.size(); ++i) {
            if (!frame_vector[i].render_flag) continue;

            frame_vector[i].username_lable->clear();
            frame_vector[i].audio_box->setCheckState(Qt::CheckState(false));
            frame_vector[i].video_box->setCheckState(Qt::CheckState(false));
            frame_vector[i].audio_box_state = true;
            frame_vector[i].video_box_state = true;
            frame_vector[i].render_flag = false;
            frame_vector[i].userid = -1;

            //init frame
            QPalette palette;
            palette.setColor(QPalette::Background, Qt::black);
            frame_vector[i].frame->setPalette(palette);
            frame_vector[i].frame->setAutoFillBackground(true);
            frame_vector[i].frame->show();
            frame_vector[i].frame->update();
        }

        ui.cmb_users_list->clear();
        userid_name_map.clear();
    }
    else {
        //房间其他用户离开房间
        RemoteUserLeave(userid);
        GetUserList();
    }
}

void CSDKExample::RemoteUserLeave(int userid) {
    //房间其他用户离开房间
    QMap<int, QTimer*>::iterator timer_iter;
    timer_iter = userid_timer_map.find(userid);
    if (timer_iter != userid_timer_map.end()) {
        if (timer_iter.value()) {
            timer_iter.value()->stop();
            /*delete timer_iter.value();
            userid_timer_map.erase(timer_iter);*/
        }
    }

    QMap<int, QString>::iterator iter = userid_name_map.find(userid);
    if (iter == userid_name_map.end())
        return;
    emit UpdateLog(QString::fromLocal8Bit(" %1 离开房间").arg(userid_name_map[userid]));
    userid_name_map.erase(iter);

    //用户退出房间，释放该用户占用的frame
    for (std::size_t i = 0; i < frame_vector.size(); ++i) {
        if (frame_vector[i].userid == userid) {
            frame_vector[i].userid = -1;
            frame_vector[i].render_flag = false;
            frame_vector[i].username_lable->clear();
            frame_vector[i].audio_box->setCheckState(Qt::CheckState(false));
            frame_vector[i].video_box->setCheckState(Qt::CheckState(false));

            QPalette palette;
            palette.setColor(QPalette::Background, Qt::black);
            frame_vector[i].frame->setPalette(palette);
            frame_vector[i].frame->setAutoFillBackground(true);
            frame_vector[i].frame->update();
            break;
        }
    }
}

void CSDKExample::OnRemoteUserEnter(int userid, int state) {
    // userid进入房间，订阅，展示展示远端流
    char user_name[64] = { 0 };
    RTCS_QueryUserState(userid, RTCS_USERSTATE_NICKNAME, user_name, 64);
    QString username = user_name;
    userid_name_map[userid] = username;

    switch (state) {
    case 1: {
        emit UpdateLog(QString().fromLocal8Bit("%1 进入房间！！").arg(username));
        //进入房间，绘制
        emit OnRemoteStream_Sig(userid, username);
        break;
    }
    default: {
        break;
    }
    }
}

void CSDKExample::RemoteStreamEnter_Slots(int userid, QString username) {
    CreateTimer(userid);

    for (std::size_t i = 0; i < frame_vector.size(); ++i) {
        if ((ui.localframe == frame_vector[i].frame) || frame_vector[i].render_flag)
            continue;

        frame_vector[i].userid = userid;
        frame_vector[i].render_flag = true;
        frame_vector[i].username_lable->setText(username);
        //设置视频显示位置
        RTCS_SetVideoPos(userid, (HWND)frame_vector[i].frame->winId(), 0, 0,
            frame_vector[i].frame->width(), frame_vector[i].frame->height());
        //用户信息水印叠加
        RTCS_PaintUserValue(userid, username.toStdString().c_str());

        //RTCS_UserCameraControl(userid, true);
        break;
    }
}

void CSDKExample::CreateTimer(int userid) {
    if (userid_timer_map.find(userid) != userid_timer_map.end()) {
        connect(userid_timer_map[userid], &QTimer::timeout, [this, userid] {
            updateVolume(userid);
        });
        userid_timer_map[userid]->start(20);
        return;
    }

    QTimer* timer = new QTimer(this);
    userid_timer_map[userid] = timer;
    connect(timer, &QTimer::timeout, [this, userid] {
        updateVolume(userid);
    });
    timer->start(20);
}

void CSDKExample::updateVolume(int userid) {
    QMutexLocker locker(&mutex);
    int volume = 0;
    char user_volume[sizeof(int)] = { 0 };
    memcpy(user_volume, &volume, sizeof(int));
    RTCS_QueryUserState(userid, RTCS_USERSTATE_SPEAKVOLUME, user_volume, sizeof(int));
    memcpy(&volume, user_volume, sizeof(int));
    if (0 == volume)
        return;

    if (-1 == userid) {
        userid = login_user_id_;
    }
    for (std::size_t i = 0; i < frame_vector.size(); ++i) {
        if (userid == frame_vector[i].userid) {
            frame_vector[i].audio_level->setValue(volume);
			QThread::msleep(100);
            frame_vector[i].audio_level->reset();
            break;
        }
    }
}

void CSDKExample::SetServerPorts() {
	char business_port[sizeof(int)] = { 0 };
	memcpy(business_port, &kBusinessPort, sizeof(int));
	RTCS_SetSDKOption(RTCS_SO_BUSINESS_SERVER_PORTS, business_port, sizeof(business_port));
}

