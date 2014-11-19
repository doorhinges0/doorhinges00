#!/bin/sh
cd /data/ais
num=`ps -ef|grep  monitor_redis_api|grep -v grep|wc -l`
if [ $num -lt 1 ]
then
  nohup ./monitor_redis_api.sh 7380
fi
