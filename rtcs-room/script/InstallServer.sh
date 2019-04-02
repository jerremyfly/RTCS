#!/bin/bash
#scriptname:InstallServer

#使用问题反馈靖哥哥

#创建安装路径
sudo mkdir /opt/VideoConsultation
sudo mkdir /opt/VideoConsultation/record

echo "正在安装，请稍候-------------------"

sudo cp application.properties rtcscoreserver.tar.gz rtcs-room-1.2.4.jar libRTCSAuth.so RunServer.sh CloseServer.sh RunConsultationServer.sh UninstallServer.sh /opt/VideoConsultation
sudo chmod +x /opt/VideoConsultation/*.sh

#获取docker
sudo docker load < /opt/VideoConsultation/rtcscoreserver.tar.gz

#设置开机自启
sudo cp /opt/VideoConsultation/RunConsultationServer.sh /etc/init.d
sudo chmod +x /etc/init.d/RunConsultationServer.sh
sudo chown root:root /etc/init.d/RunConsultationServer.sh

cd /etc/init.d
sudo update-rc.d RunConsultationServer.sh defaults

echo "-----------------------------------"
echo "-------安装成功，请重启服务器-------"
echo "-----------------------------------"
