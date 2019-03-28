#include "NativeTest.h"

#include <QTime>
#include <QThread>

#include "RTCSSDKForC.h"

NativeTest::NativeTest(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);

    GetRecordingAudioDevices();
    GetPlayoutAudioDevices();

    SetRecordingVolumeSlider();
    SetPlayoutVolumeSlider();

    connect(ui.btn_getmicrophonevolume, SIGNAL(clicked()), this, SLOT(changeMicrophoneVolume()));
    connect(ui.btn_getspeakervolume, SIGNAL(clicked()), this, SLOT(changeSpeakerVolume()));

    ui.pgb_microphone_test->setRange(0, 255);
    ui.pgb_microphone_test->setValue(0);
    ui.pgb_speaker_test->setRange(0, 255);
    ui.pgb_speaker_test->setValue(0);
    connect(ui.btn_microphone_test, SIGNAL(clicked()), this, SLOT(setMicrophonePgbVolume()));
    connect(ui.btn_peaker_test, SIGNAL(clicked()), this, SLOT(setSpeakerPgbVolume()));

    connect(ui.btn_set_audio_devices, SIGNAL(clicked()), this, SLOT(setAudioDevice()));
    connect(ui.btn_get_recording_devices, SIGNAL(clicked()), this, SLOT(getCurRecordingDevice()));
    connect(ui.btn_get_playout_devices, SIGNAL(clicked()), this, SLOT(getCurPlayoutDevice()));

    connect(ui.btn_get_audio_devices, &QPushButton::clicked, [this] {
        GetRecordingAudioDevices();
        GetPlayoutAudioDevices();
    });

    connect(ui.btn_get_video_device, SIGNAL(clicked()), this, SLOT(getCurVideoDevice()));

    ui.cbx_ns->setChecked(true);
    connect(ui.cbx_ns, SIGNAL(stateChanged(int)), this, SLOT(setNsState(int)));
    ui.cbx_agc->setChecked(true);
    connect(ui.cbx_agc, SIGNAL(stateChanged(int)), this, SLOT(setAGCState(int)));
    ui.cbx_echo->setChecked(true);
    connect(ui.cbx_echo, SIGNAL(stateChanged(int)), this, SLOT(setEchoState(int)));


}

NativeTest::~NativeTest() {

}

void NativeTest::InitAudioMicDevice() {
    char recording_device_name[128] = { 0 };
    GetCurrentAudioRecordingDeviceName(recording_device_name);
    RTCS_SelectAudioCapture(recording_device_name);
    RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEIN, GetMicrophoneVolume());
}

void NativeTest::InitAudioPlayoutDevice() {
    char playout_device_name[128] = { 0 };
    GetCurrentAudioPlayoutDeviceName(playout_device_name);
    RTCS_SelectAudioPlayback(playout_device_name);
    RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEOUT, GetSpeakerVolume());
}

void NativeTest::GetRecordingAudioDevices() {
    //unsigned int voiceRecordingDeviceNumber = client->GetRecordingDevicesNum();
    unsigned int voiceRecordingDeviceNumber = 10;
    char* voiceRecordingDeviceNames[10] = { 0 };
    for (std::size_t i = 0; i < 10; i++)
        voiceRecordingDeviceNames[i] = new char[256];

    RTCS_EnumAudioCapture(voiceRecordingDeviceNames, voiceRecordingDeviceNumber);
    ui.cmb_recordingDevices->clear();
    //遍历音频采集设备列表
    for (std::size_t i = 0; i != voiceRecordingDeviceNumber; i++)
    {
        ui.cmb_recordingDevices->addItem(QString::fromUtf8(voiceRecordingDeviceNames[i]), NULL);
    }
    for (std::size_t i = 0; i < 10; i++)
    {
        delete[] voiceRecordingDeviceNames[i];
    }
}

void NativeTest::GetPlayoutAudioDevices() {
    unsigned int voicePlayoutDeviceNumber = 10;;
    char* voicePlayoutDeviceNames[10] = { 0 };
    for (std::size_t i = 0; i < 10; i++)
        voicePlayoutDeviceNames[i] = new char[256];

    RTCS_EnumAudioPlayback(voicePlayoutDeviceNames, voicePlayoutDeviceNumber);
    ui.cmb_playoutDevices->clear();
    //遍历音频播放设备列表
    for (std::size_t i = 0; i != voicePlayoutDeviceNumber; i++)
    {
        ui.cmb_playoutDevices->addItem(QString::fromUtf8(voicePlayoutDeviceNames[i]), NULL);

    }
    for (std::size_t i = 0; i < 10; i++)
    {
        delete[] voicePlayoutDeviceNames[i];
    }
}

void NativeTest::SetRecordingVolumeSlider() {
    ui.set_recording_volume_slider->setMinimum(0);
    ui.set_recording_volume_slider->setMaximum(255);
    ui.set_recording_volume_slider->setValue(100);
    ui.line_recording_volume_text->setText(QString("%1").arg(ui.set_recording_volume_slider->value()));
    connect(ui.set_recording_volume_slider, SIGNAL(valueChanged(int)), this, SLOT(setMicrophoneVolume(int)));
}
void NativeTest::SetPlayoutVolumeSlider() {
    ui.set_playout_volume_slider->setMinimum(0);
    ui.set_playout_volume_slider->setMaximum(255);
    ui.set_playout_volume_slider->setValue(100);
    ui.line_playout_volume_text->setText(QString("%1").arg(ui.set_playout_volume_slider->value()));
    connect(ui.set_playout_volume_slider, SIGNAL(valueChanged(int)), this, SLOT(setSpeakerVolume(int)));
}

int NativeTest::GetCurrentAudioRecordingDeviceIndex() {
    return ui.cmb_recordingDevices->currentIndex();
}

int NativeTest::GetCurrentAudioPlayoutDeviceIndex() {
    return ui.cmb_playoutDevices->currentIndex();
}

int NativeTest::GetMicrophoneVolume() {
    return ui.set_recording_volume_slider->value();
}

int NativeTest::GetSpeakerVolume() {
    return ui.set_playout_volume_slider->value();
}

void NativeTest::setMicrophonePgbVolume() {
    //emit UpdateLog(QString::fromLocal8Bit("获取麦克风音量..."));
    unsigned int microphone_volume = 0;
    RTCS_AudioGetVolume(RTCS_AudioDevice::RTCS_AD_WAVEIN, microphone_volume);
    /*InitAudioDevice();
    RTCS_UserSpeakControl(-1, true);*/

    ui.pgb_microphone_test->setValue(microphone_volume);
	QThread::msleep(200);
    ui.pgb_microphone_test->reset();
}

void NativeTest::setSpeakerPgbVolume() {
    //emit UpdateLog(QString::fromLocal8Bit("获取扬声器音量..."));
    unsigned int speaker_volume = 0;
    RTCS_AudioGetVolume(RTCS_AudioDevice::RTCS_AD_WAVEOUT, speaker_volume);

    ui.pgb_speaker_test->setValue(speaker_volume);
	QThread::msleep(200);
    ui.pgb_speaker_test->reset();
}

void NativeTest::setMicrophoneVolume(int value) {
    int pos = ui.set_recording_volume_slider->value();
    QString str = QString("%1").arg(pos);
    ui.line_recording_volume_text->setText(str);

    RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEIN, pos);
}

void NativeTest::setSpeakerVolume(int value) {
    int pos = ui.set_playout_volume_slider->value();
    QString str = QString("%1").arg(pos);
    ui.line_playout_volume_text->setText(str);

    RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEOUT, pos);
}

void NativeTest::changeMicrophoneVolume() {
    QString mic_name = ui.btn_getmicrophonevolume->text();
    if (0 == QString::compare(QString::fromLocal8Bit("麦克风静音"), mic_name)) {  //静音状态 
        //RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEIN, 0);
        RTCS_UserSpeakControl(-1, false);
        ui.btn_getmicrophonevolume->setText(QString::fromLocal8Bit("麦克风原音"));

    } else {
        int pos = ui.set_recording_volume_slider->value();
        //RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEIN, pos);
        RTCS_UserSpeakControl(-1, true);
        ui.btn_getmicrophonevolume->setText(QString::fromLocal8Bit("麦克风静音"));

    }

}

void NativeTest::changeSpeakerVolume() {
    QString speaker_name = ui.btn_getspeakervolume->text();
    if (0 == QString::compare(QString::fromLocal8Bit("播放静音"), speaker_name)) {  //静音状态
        RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEOUT, 0);
        ui.btn_getspeakervolume->setText(QString::fromLocal8Bit("播放原音"));

    } else {
        int pos = ui.set_playout_volume_slider->value();
        RTCS_AudioSetVolume(RTCS_AudioDevice::RTCS_AD_WAVEOUT, pos);
        ui.btn_getspeakervolume->setText(QString::fromLocal8Bit("播放静音"));

    }
}

void NativeTest::setAudioDevice() {
    char recording_device_name[128] = { 0 };
    char playout_device_name[128] = { 0 };
    GetCurrentAudioRecordingDeviceName(recording_device_name);
    GetCurrentAudioPlayoutDeviceName(playout_device_name);
    RTCS_SelectAudioCapture(recording_device_name);
    RTCS_SelectAudioPlayback(playout_device_name);
}

void NativeTest::GetCurrentAudioRecordingDeviceName(char* device_name) {
    QString name = ui.cmb_recordingDevices->currentText();
    strcpy(device_name, name.toStdString().c_str());
}

void NativeTest::GetCurrentAudioPlayoutDeviceName(char* device_name) {
    QString name = ui.cmb_playoutDevices->currentText();
    strcpy(device_name, name.toStdString().c_str());
}

void NativeTest::getCurRecordingDevice() {
    char device_name[64] = { 0 };
    RTCS_GetCurAudioCapture(device_name, 64);
    ui.cmb_get_recording_devices->clear();
    ui.cmb_get_recording_devices->addItem(QString::fromUtf8(device_name));
}
void NativeTest::getCurPlayoutDevice() {
    char device_name[64] = { 0 };
    RTCS_GetCurAudioPlayback(device_name, 64);
    ui.cmb_get_playout_devices->clear();
    ui.cmb_get_playout_devices->addItem(QString::fromUtf8(device_name));
}

void NativeTest::getCurVideoDevice() {
    char device_name[64] = { 0 };
    RTCS_GetCurVideoCapture(device_name, 64);
    ui.cmb_get_video_device->clear();
    ui.cmb_get_video_device->addItem(QString::fromUtf8(device_name));
}

//void NativeTest::getUserstate() {
//    char user_state[sizeof(int)] = { 0 };
//    RTCS_QueryUserState(-1, RTCS_USERSTATE_SELFUSERSTATUS, user_state, sizeof(int));
//    int state = 0;
//    memcpy(&state, user_state, sizeof(int));
//}

void NativeTest::setNsState(int state) {
    int ns_state = 0;  //close
    if (state == Qt::Checked) {
        ns_state = 1;
    }
    char nsstate[sizeof(int)] = { 0 };
    memcpy(nsstate, &ns_state, sizeof(int));
    RTCS_SetSDKOption(RTCS_SO_AUDIO_NSCTRL, nsstate, sizeof(int));
}

void NativeTest::setAGCState(int state) {
    int agc_state = 0;
    if (state == Qt::Checked) {
        agc_state = 1;
    }
    char agcstate[sizeof(int)] = { 0 };
    memcpy(agcstate, &agc_state, sizeof(int));
    RTCS_SetSDKOption(RTCS_SO_AUDIO_AGCCTRL, agcstate, sizeof(int));

}

void NativeTest::setEchoState(int state) {
    int echo_state = 0;
    if (state == Qt::Checked) {
        echo_state = 1;
    }
    char echostate[sizeof(int)] = { 0 };
    memcpy(echostate, &echo_state, sizeof(int));
    RTCS_SetSDKOption(RTCS_SO_AUDIO_ECHOCTRL, echostate, sizeof(int));

}

