#include <QtWidgets/QMainWindow>
#include "ui_CSDKExample.h"

#include <QMutex>
#include <QTimer>

class SettingConfig;
class NativeTest;

class CSDKExample : public QMainWindow
{
    Q_OBJECT

public:
    CSDKExample(QWidget *parent = Q_NULLPTR);
    virtual ~CSDKExample();
    void initFrame(QFrame* frame, QCheckBox* videobox,
                   QCheckBox* audiobox, QLabel* label, QProgressBar* processbar);

    void OnUserConnect(int result);
    void OnUserlogin(int userid, int result);
    void OnUserlogout(int result);
    void OnEnterRoom(int roomid, int result);
    void OnLeaveRoom(int roomid, int userid);
    void OnRemoteUserEnter(int userid, int state);

Q_SIGNALS:
    void UpdateLog(const QString& log);
    void OnEnterRoom_Sig(int roomid, int result);
    void OnLeaveRoom_Sig(int roomid, int userid);
    void OnRemoteStream_Sig(int dwUserid, QString username);


private slots:
    void UserLogin();
    void UserLogout();
    void JoinRoom();
    void LeaveRoom();
    void GetUserList();
    void TransBuffer();
    void TransBufferRoom();
    void EnterRoom_Slots(int roomid, int result);
    void LeaveRoom_Slots(int roomid, int userid);
    void RemoteStreamEnter_Slots(int userid, QString username);
    void UserDesktopShare();
    void UserWindowShare();

private slots:
void updateVolume(int userid);


private:
    void ConfigLocalVideoParam();
    void AppenedLog(const QString& log);
    //��ȡ���������������û�
    void GetUserList(int roomid, QStringList& user_list);
    void LocalMeidiaControl(QCheckBox* checkbox);
    void RemoteMediaControl(QCheckBox* checkbox);
    void RemoteUserLeave(int userid);
    void CreateTimer(int userid);
	void SetServerPorts();

private:
    Ui::QtCSDKExampleClass ui;
    SettingConfig* setting_ui_ = Q_NULLPTR;
    NativeTest* native_ui_ = Q_NULLPTR;

    struct FrameStruct {
        QFrame* frame;    //frame����
        QCheckBox* audio_box;    //��Ƶbox
        QCheckBox* video_box;    //��Ƶbox
        QLabel* username_lable;
        QProgressBar* audio_level; //��Ƶ������̬��ʾ
        bool audio_box_state = true;    //��Ƶbox״̬
        bool video_box_state = true;    //��Ƶbox״̬
        bool render_flag = false;    //frame�Ƿ�ռ�ã���ռ��Ϊtrue
        int userid = -1;    //ռ�ø�frame���û�id

    }frame_struct;

    QMap<int, QString> userid_name_map;
    QVector<FrameStruct> frame_vector;
    int login_user_id_ = -1;

    QMap<int, QTimer*> userid_timer_map;
    QMutex mutex;

	const int kBusinessPort = 8383;  //ҵ��������˿�
};
