#!/bin/sh
#scriptname:RunConsultationServer

#开机自启运行服务器

export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin

case "$1" in
  start)
	INSTALLDIR=/opt/VideoConsultation
        sleep 5
        cd ${INSTALLDIR}
        ./RunServer.sh

	;;
  stop)
	;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop}" >&2
	exit 3
	;;
esac


