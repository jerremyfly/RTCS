#ifndef CSDK_EXAMPLE_SETTING_CONFIG_H
#define CSDK_EXAMPLE_SETTING_CONFIG_H

#include <QWidget>
#include "ui_SettingConfig.h"

class SettingConfig : public QWidget {
    Q_OBJECT

public:
    SettingConfig(QWidget *parent = Q_NULLPTR);
    ~SettingConfig();

    void GetVideoCaptureDevices();
    void getRecordingAudioDevices();
    void getPlayoutAudioDevices();

    void getCurrentVideoDevice(char* const deviceTrueName, char* const deviceID);
    int getCurrentAudioRecordingDevice();
    int getCurrentAudioPlayoutDevice();
    //≤‚ ‘”√
    char vidoeparamsinfo[20000];
    void ApplySetting();
signals:
    //void ChangeSettings(rtcs::base::LocalCameraStreamParameters);
private:
    Ui::SettingConfig ui;
};


#endif // !CSDK_EXAMPLE_SETTING_CONFIG_H
