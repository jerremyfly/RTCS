#!/bin/bash
#scriptname:UninstallServer

#使用问题反馈靖哥哥

sudo pkill java
sudo pkill node

sudo docker container prune
sudo docker image rm rtcs/rtcscore:1.0

sudo rm -rf /opt/VideoConsultation

cd /etc/init.d
sudo update-rc.d -f RunConsultationServer.sh remove
sudo rm /etc/init.d/RunConsultationServer.sh

echo "-----------------------------------"
echo "--------------卸载成功--------------"
echo "-----------------------------------"
