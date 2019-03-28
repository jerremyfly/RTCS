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

    RTCS_InitSDK(0);  //��ʼ��SDK

    ///////////////////////��������־������ܵ����Ӻ���////////////////////////////
    qDebug() << connect(this, &CSDKExample::UpdateLog, this, [this](const QString& q) {
        AppenedLog(q);
    });
    /*************************�������ò˵���ʼ������ز���**************************/
    setting_ui_ = new SettingConfig();
    setting_ui_->ApplySetting();

    //emit UpdateLog(setting_ui_->vidoeparamsinfo);

    QAction* configAction = ui.menuBar->addAction(QString::fromLocal8Bit("��������"));
    qDebug() << connect(configAction, &QAction::triggered, this, [this]() {
        setting_ui_->show();
		setting_ui_->raise();
    });

    /*************************������Ƶ���Բ˵���ʼ������ز���**************************/
    native_ui_ = new NativeTest();
    QAction* nativeTestAction = ui.menuBar->addAction(QString::fromLocal8Bit("������Ƶ����"));
    qDebug() << connect(nativeTestAction, &QAction::triggered, this, [this]() {
        native_ui_->show();
        native_ui_->raise();
    });
    /*************************������������ü���ز���**************************/

    /////////////////////////��¼������Ĭ������//////////////////////////////
    ui.tex_username->setText("zhaoj");
    ui.tex_passward->setText("1");
    ui.tex_server->setText("10.0.8.146:8906");
    ui.text_desktopid->setText("0");
    ui.text_windowname->setText("CSDKExample");

    ///////////////////////�������а�ť�������Ӻ���////////////////////////////
    connect(ui.btn_login, SIGNAL(clicked()), this, SLOT(UserLogin()));
    connect(ui.btn_logout, SIGNAL(clicked()), this, SLOT(UserLogout()));
    connect(ui.btn_joinRoom, SIGNAL(clicked()), this, SLOT(JoinRoom()));
    connect(ui.btn_leave_room, SIGNAL(clicked()), this, SLOT(LeaveRoom()));
    connect(ui.btn_getusers, SIGNAL(clicked()), this, SLOT(GetUserList()));
    connect(ui.btn_transbuffer, SIGNAL(clicked()), this, SLOT(TransBuffer()));
    connect(ui.btn_transbufferroom, SIGNAL(clicked()), this, SLOT(TransBufferRoom()));
    connect(ui.btn_desktopshare, SIGNAL(clicked()), this, SLOT(UserDesktopShare()));
    connect(ui.btn_windowshare, SIGNAL(clicked()), this, SLOT(UserWindowShare()));

    ///////////////////////����Frame��ʼ��/////////////////////////////////
    initFrame(ui.localframe, ui.enableAudio_cb_local, ui.enableVideo_cb_local,ui.label, ui.local_audio_level);
    initFrame(ui.remoteframe, ui.enableAudio_cb_remote_1, ui.enableVideo_cb_remote_1, ui.remote_label_1, ui.remote_audio_level_1);
    initFrame(ui.remoteframe_2, ui.enableAudio_cb_remote_2, ui.enableVideo_cb_remote_2, ui.remote_label_2, ui.remote_audio_level_2);
    initFrame(ui.remoteframe_3, ui.enableAudio_cb_remote_3, ui.enableVideo_cb_remote_3, ui.remote_label_3, ui.remote_audio_level_3);
    initFrame(ui.remoteframe_4, ui.enableAudio_cb_remote_4, ui.enableVideo_cb_remote_4, ui.remote_label_4, ui.remote_audio_level_4);
    initFrame(ui.remoteframe_5, ui.enableAudio_cb_remote_5, ui.enableVideo_cb_remote_5, ui.remote_label_5, ui.remote_audio_level_5);
    initFrame(ui.remoteframe_6, ui.enableAudio_cb_remote_6, ui.enableVideo_cb_remote_6, ui.remote_label_6, ui.remote_audio_level_6);
    initFrame(ui.remoteframe_7, ui.enableAudio_cb_remote_7, ui.enableVideo_cb_remote_7, ui.remote_label_7, ui.remote_audio_level_7);

    /////////////////////localFrame��Ӧ��Checkbox�����Ӻ���//////////////////////
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

    ///////////////////////////////����/////////////////////////////////
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
        //�ر���Ƶʱ����ʼ��localframe
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
        //���ñ�����Ƶ���в���
        ConfigLocalVideoParam();
        //������ͷ
        RTCS_UserCameraControl(-1, ui.enableVideo_cb_local->checkState());
    }
}
void CSDKExample::RemoteMediaControl(QCheckBox* checkbox) {
    //�򿪹ر�Զ������Ƶ
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
        QMessageBox::information(this, "info", QString::fromLocal8Bit("��������ַδ����"),
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
        emit UpdateLog(QString::fromLocal8Bit("������Ϊ�գ�������Ҫ����ķ�����...."));
        return;
    }
    emit UpdateLog(QString::fromLocal8Bit("������뷿�� %1").arg(room_name));

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
    emit UpdateLog(QString::fromLocal8Bit("�����뿪���� %1").arg(room_name));
    RTCS_LeaveRoom(room_name_int);
}


void CSDKExample::UserLogout(){
    if (-1 == login_user_id_) {
        emit UpdateLog(QString::fromLocal8Bit("��û�е�¼!!"));
        return;
    }

    LeaveRoom();
	ui.text_room_name->setText("");

    RTCS_Logout();
    login_user_id_ = -1;
    emit UpdateLog(QString::fromLocal8Bit("�˳���¼!!"));
}

void CSDKExample::TransBuffer(){
    QString json("{hello:loginuser}");
    QString json2("{hello:2}");
    RTCS_TransBuffer(-2, json.toStdString().c_str(), json.length());
    RTCS_TransBuffer(2, json2.toStdString().c_str(), json2.length());
    emit UpdateLog(QString::fromLocal8Bit("͸��ͨ����Ϣ�ѷ���"));
}

void CSDKExample::TransBufferRoom(){
    char buf[5] = { 0 };
    buf[0]= 0x0a;
    buf[1] = 0xAC;
    buf[2] = 0xA1;
    buf[3] = 0x02;
    buf[4] = 0xFE;
    RTCS_TransBuffer(-1, buf, sizeof(buf));
    emit UpdateLog(QString::fromLocal8Bit("͸��ͨ����Ϣ�ѷ���"));
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
        emit UpdateLog(QString::fromLocal8Bit("û�иô��壡��"));
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
    result ? emit UpdateLog(QString::fromLocal8Bit("���ӷ������ɹ�"))
           : emit UpdateLog(QString::fromLocal8Bit("���ӷ�����ʧ��"));
}

void CSDKExample::OnUserlogin(int userid, int result) {
    switch (result) {
    case RTCS_SUCCESS: {
        login_user_id_ = userid;
        emit UpdateLog(QString::fromLocal8Bit("�Ñ���¼�ɹ�"));
        emit UpdateLog(QString::fromLocal8Bit("�û�id:%1").arg(userid));
        break;
    }
    case RTCS_ERR_CERTIFY_FAIL: {
        emit UpdateLog(QString::fromLocal8Bit("�Ñ������ڣ���¼ʧ��"));
        break;
    }
    case RTCS_ERR_ALREADY_LOGIN: {
        emit UpdateLog(QString::fromLocal8Bit("���Ñ��ѵ�¼��������û�����"));
        break;
    }
    case RTCS_ERR_VISITOR_DENY: {
        emit UpdateLog(QString::fromLocal8Bit("�ο͵�¼����ֹ����������ȷ�����룡"));
        break;
    }
    case RTCS_ERR_SERVERSDK_FAIL: {
        emit UpdateLog(QString::fromLocal8Bit("���ӷ�����ʧ�ܣ�"));
        break;
    }
    case RTCS_ERR_SERVERSDK_TIMEOUT: {
        emit UpdateLog(QString::fromLocal8Bit("���ӷ�������ʱ��"));
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
        emit UpdateLog(QString::fromLocal8Bit("�û��ǳ�"));
        ui.text_room_name->clear();
        break;
    }
    case RTCS_ERR_NOTLOGIN: {
        emit UpdateLog(QString::fromLocal8Bit("�û������ڣ����ȵ�¼����"));
        break;
    }
    default: {
        break;
    }
    }
}

void CSDKExample::OnEnterRoom(int roomid, int result){
    emit OnEnterRoom_Sig(roomid, result);//���̲߳���
}

void test(int roomid, unsigned int num) {
    RTCS_GetRoomOnlineUsers(roomid, nullptr, num);
}

void CSDKExample::EnterRoom_Slots(int roomid, int result){
    if (result == RTCS_SUCCESS){
        emit UpdateLog(QString::fromLocal8Bit("���뷿��ɹ��������:%1").arg(roomid));

        //��ȡ�������������û�
        GetUserList();
        //ռ�õ�һ��frame
        frame_vector[0].render_flag = true;
        frame_vector[0].userid = login_user_id_;
        frame_vector[0].username_lable->setText(userid_name_map[login_user_id_]);
        //���ñ�����Ƶ���в���
        ConfigLocalVideoParam();
        //��ʼ��������Ƶ�豸������˷�
        native_ui_->InitAudioMicDevice();
        native_ui_->InitAudioPlayoutDevice();
        //��ʱ����������
        CreateTimer(-1);

        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this, roomid] {
            test(roomid, 0);
        });
        timer->start(10);

    } else if (result == RTCS_ERR_ROOM_ALREADIN) {
        emit UpdateLog(QString::fromLocal8Bit("�Ѿ��ڷ�����"));
    } else {
        emit UpdateLog(QString::fromLocal8Bit("���뷿��ʧ��"));
    }
}

void CSDKExample::ConfigLocalVideoParam() {
    //����Ƶ��������Ƶ��Ҫ�����в���
    setting_ui_->ApplySetting();
    int parameterapply = 1;
    char buffer[sizeof(int)] = { 0 };
    memcpy(buffer, &parameterapply, sizeof(int));
    RTCS_SetSDKOption(RTCS_SO_LOCALVIDEO_APPLYPARAM, buffer, sizeof(parameterapply));
    ////������Ƶ��ʾλ��
    RTCS_SetVideoPos(-1, (HWND)ui.localframe->winId(), 0, 0,
        ui.localframe->width(), ui.localframe->height());
    //�û���Ϣˮӡ����
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
    emit OnLeaveRoom_Sig(roomid, userid);//���̲߳���
}

void CSDKExample::LeaveRoom_Slots(int roomid, int userid)
{
    if (userid == login_user_id_) {
        //�����û��뿪���䣬��ʼ������ʹ�õ�frame
        emit UpdateLog(QString::fromLocal8Bit("�뿪����ɹ�"));
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
        //���������û��뿪����
        RemoteUserLeave(userid);
        GetUserList();
    }
}

void CSDKExample::RemoteUserLeave(int userid) {
    //���������û��뿪����
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
    emit UpdateLog(QString::fromLocal8Bit(" %1 �뿪����").arg(userid_name_map[userid]));
    userid_name_map.erase(iter);

    //�û��˳����䣬�ͷŸ��û�ռ�õ�frame
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
    // userid���뷿�䣬���ģ�չʾչʾԶ����
    char user_name[64] = { 0 };
    RTCS_QueryUserState(userid, RTCS_USERSTATE_NICKNAME, user_name, 64);
    QString username = user_name;
    userid_name_map[userid] = username;

    switch (state) {
    case 1: {
        emit UpdateLog(QString().fromLocal8Bit("%1 ���뷿�䣡��").arg(username));
        //���뷿�䣬����
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
        //������Ƶ��ʾλ��
        RTCS_SetVideoPos(userid, (HWND)frame_vector[i].frame->winId(), 0, 0,
            frame_vector[i].frame->width(), frame_vector[i].frame->height());
        //�û���Ϣˮӡ����
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

