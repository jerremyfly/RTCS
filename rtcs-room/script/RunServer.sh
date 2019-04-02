#!/bin/sh
#scriptname:RunServer
#安装前请修改JDKPATH为本机java路径，修改ARGS为本机IP地址

JDKPATH="/usr/local/jdk1.8.0_181/bin"
export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin:$JDKPATH

echo "启动核心服务器" >| /opt/VideoConsultation/server.log
ARGS="PUBLIC_IP=10.0.8.213"
docker run -d --mount type=bind,source=/opt/VideoConsultation/record,target=/opt/record -p 3000:3000 -p 8080:8080 -e $ARGS rtcs/rtcscore:1.0 >> /opt/VideoConsultation/server.log

echo "------------------------------------------------------------------------------------------------------------" >> /opt/VideoConsultation/server.log
echo "------------------------------------------------------------------------------------------------------------" >> /opt/VideoConsultation/server.log

cd /opt/VideoConsultation
echo "启动业务服务器" >> server.log
java -Dfile.encoding=utf-8 -jar rtcs-room-1.2.4.jar >> server.log 2>&1


