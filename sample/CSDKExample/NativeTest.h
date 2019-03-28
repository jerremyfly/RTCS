#pragma once
#include <QWidget>
#include "ui_NativeTest.h"


class NativeTest : public QWidget
{
    Q_OBJECT

public:
    NativeTest(QWidget *parent = Q_NULLPTR);
    ~NativeTest();

    void InitAudioMicDevice();
    void InitAudioPlayoutDevice();
    void GetRecordingAudioDevices();
    void GetPlayoutAudioDevices();
    void SetRecordingVolumeSlider();
    void SetPlayoutVolumeSlider();

    int GetCurrentAudioRecordingDeviceIndex();
    int GetCurrentAudioPlayoutDeviceIndex();
    void GetCurrentAudioRecordingDeviceName(char* device_name);
    void GetCurrentAudioPlayoutDeviceName(char* device_name);
    int GetMicrophoneVolume();
    int GetSpeakerVolume();

 private slots:
    void setMicrophoneVolume(int value);
    void setSpeakerVolume(int value);
    void changeMicrophoneVolume();
    void changeSpeakerVolume();
    void setAudioDevice();
    void setMicrophonePgbVolume();
    void setSpeakerPgbVolume();
    void getCurRecordingDevice();
    void getCurPlayoutDevice();
    void getCurVideoDevice();
    void setNsState(int);
    void setAGCState(int);
    void setEchoState(int);

private:
    Ui::NativeTest ui;

};

