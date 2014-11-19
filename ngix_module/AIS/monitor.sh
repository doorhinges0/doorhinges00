#!/bin/sh
cd /data/ais
num=`ps -ef|grep  monitor_api|grep -v grep|wc -l`
if [ $num -lt 1 ]
then
  nohup ./monitor_api.sh
fi
